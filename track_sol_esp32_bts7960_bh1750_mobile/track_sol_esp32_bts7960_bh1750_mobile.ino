
/*
 * Tracker Solaire ESP32 – Version avec interface web embarquée
 *
 * STRUCTURE DES FICHIERS :
 *   config.h       — tous les #define (WiFi, pins, seuils, timing)
 *   state.h        — variables d'état partagées entre modules
 *   logger.h/.cpp  — journalisation (Serial + buffer web)
 *   sensors.h/.cpp — capteurs BH1750 (lux) et fins de course
 *   motors.h/.cpp  — commande des moteurs BTS7960 + tracking
 *   wind.h/.cpp    — capteur de vent (MAÎTRE uniquement)
 *   webserver.h/.cpp — serveur HTTP + routes de l'API
 *   html_page.h/.cpp — page HTML stockée en flash
 *
 * BIBLIOTHÈQUES REQUISES (Gestionnaire de bibliothèques Arduino IDE) :
 *   - BTS7960    : https://github.com/1337encrypted/BTS7960_Motordriver
 *   - BH1750     : https://github.com/claws/BH1750
 *   - ElegantOTA : https://github.com/ayushsharma82/ElegantOTA (MAJ OTA, mode synchrone)
 *   - WiFi, WebServer, HTTPClient, Update : inclus dans le core ESP32
 *
 * AVANT DE FLASHER :
 *   1. Ouvrir config.h
 *   2. Modifier WIFI_SSID, WIFI_PASSWORD
 *   3. Choisir IS_MASTER (true sur le tracker avec capteur de vent)
 *   4. Flasher l'esclave en premier, noter son IP, la mettre dans SLAVE_IP
 *   5. Flasher le maître
 */

#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "settings.h"
#include "state.h"
#include "logger.h"
#include "sensors.h"
#include "motors.h"
#include "webserver_routes.h"
#include "wind.h"

// =====================================================================
//   DÉFINITION DES VARIABLES D'ÉTAT PARTAGÉES
//   (déclarées extern dans state.h — un seul endroit de définition)
// =====================================================================
bool   modeAuto             = false;  // Démarrage en MANUEL — évite tout tracking auto après un reboot
bool   alerteVent           = false;
bool   enPositionRepos      = false;
bool   autoStartPending     = true;   // passage AUTO différé après DELAY_AUTO_START_MS
bool   modeAutoAvantAlerte  = false;  // mode sauvegardé avant mise en sécurité vent
bool   otaEnCours           = false;  // MAJ OTA en cours — coupe moteurs et bloque le tracking
String cmdMoteur        = "STOP";
unsigned long tDernierCmd = 0;
String journal     = "";
String actionCourante    = "REPOS";

// ── Timers internes au loop ───────────────────────────────────────────
static unsigned long tDerniersCapteurs = 0;
static unsigned long tDerniersTracking = 0;
static unsigned long tDernierVent      = 0;

// =====================================================================
//   SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // GPIO15 (RpwmEO) et GPIO5 (LpwmIH) sont des strapping pins ESP32 avec
  // pull-up interne → HIGH au boot → moteurs partent avant begin().
  // GPIO17 (RpwmIH) et GPIO2 (LpwmEO) flottent ou sont pull-down : risque
  // plus faible mais on force les 4 PWM à LOW par sécurité.
  pinMode(RpwmEO, OUTPUT); digitalWrite(RpwmEO, LOW);
  pinMode(LpwmEO, OUTPUT); digitalWrite(LpwmEO, LOW);
  pinMode(RpwmIH, OUTPUT); digitalWrite(RpwmIH, LOW);
  pinMode(LpwmIH, OUTPUT); digitalWrite(LpwmIH, LOW);

  loadSettings();
  ajouterLog("Firmware " FW_VERSION);
  setupMoteurs();
  setupCapteurs();

#if IS_MASTER
  setupVent();
#endif

  // Connexion WiFi
  Serial.print("Connexion WiFi a '" WIFI_SSID "' ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    String ip = WiFi.localIP().toString();
    Serial.println("Connecte ! IP : " + ip);
    Serial.println("Interface web  : http://" + ip);
    ajouterLog("WiFi OK — http://" + ip);
  } else {
    Serial.println("ECHEC WiFi. Fonctionnement sans reseau.");
    ajouterLog("WiFi ECHEC. Interface web indisponible.");
  }

  setupServeur();
  lireCapteurs();
  arreterMoteurs();  // filet de sécurité final avant le loop

  Serial.println("Demarrage termine.");
}

// =====================================================================
//   LOOP
// =====================================================================
void loop() {
  tickServeur();                        // traiter les requêtes HTTP en attente
  unsigned long t = millis();

  // ── Lecture périodique des capteurs ────────────────────────────────
  if (t - tDerniersCapteurs > INTERVAL_CAPTEURS) {
    lireCapteurs();
    tDerniersCapteurs = t;
  }

  // ── Vérification du capteur de vent (MAÎTRE uniquement) ────────────
#if IS_MASTER
  if (t - tDernierVent > INTERVAL_VENT) {
    verifierVent();
    tDernierVent = t;
  }
#endif

  // ── Passage AUTO différé au démarrage ─────────────────────────────
  if (autoStartPending && !alerteVent && t >= DELAY_AUTO_START_MS) {
    autoStartPending = false;
    modeAuto         = true;
    ajouterLog("Demarrage : passage AUTO apres " + String(DELAY_AUTO_START_MS / 60000UL) + " min.");
  }

  // ── Sécurité moteur manuel : arrêt si plus de commande depuis TIMEOUT
  if (!modeAuto && cmdMoteur != "STOP") {
    if (t - tDernierCmd > TIMEOUT_MOTEUR_MAN) {
      arreterMoteurs();
      cmdMoteur = "STOP";
    }
  }

  // ── Tracking automatique ───────────────────────────────────────────
  if (modeAuto && !alerteVent && !otaEnCours && (t - tDerniersTracking > INTERVAL_TRACKING)) {
    tDerniersTracking = t;

    float maxLux = luxNord;
    if (luxSud   > maxLux) maxLux = luxSud;
    if (luxEst   > maxLux) maxLux = luxEst;
    if (luxOuest > maxLux) maxLux = luxOuest;

    if (maxLux >= seuilLum) {
      enPositionRepos = false;   // le soleil est là, la prochaine nuit relancera le retour
      ajouterLog("--- Tracking step ---");
      actionCourante = "TRACKING";
      trackSun(motorEO, fdcES, fdcOU, luxEst,  luxOuest);  // axe Est-Ouest

      if (modeAuto && !alerteVent) {
        trackSun(motorIH, fdcIH, fdcIV, luxNord, luxSud);  // axe Inclinaison
      }
      actionCourante = "REPOS";
    } else {
      // Nuit ou ciel très couvert → position de repos (une seule fois jusqu'au prochain tracking)
      if (!enPositionRepos) {
        ajouterLog("Lum < seuil. Position repos.");
        actionCourante = "PLAT";
        miseAPlat();
        actionCourante = "EST";
        retourEst();
        actionCourante = "REPOS";
        enPositionRepos = true;
      }
    }
  }
}
