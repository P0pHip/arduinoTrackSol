
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
 *   - BTS7960  : https://github.com/1337encrypted/BTS7960_Motordriver
 *   - BH1750   : https://github.com/claws/BH1750
 *   - WiFi, WebServer, HTTPClient : incluses dans le core ESP32
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
#include "state.h"
#include "logger.h"
#include "sensors.h"
#include "motors.h"
#include "webserver.h"
#include "wind.h"

// =====================================================================
//   DÉFINITION DES VARIABLES D'ÉTAT PARTAGÉES
//   (déclarées extern dans state.h — un seul endroit de définition)
// =====================================================================
bool   modeAuto    = true;
bool   alerteVent  = false;
String cmdMoteur   = "STOP";
unsigned long tDernierCmd = 0;
String journal     = "";

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

  // ── Sécurité moteur manuel : arrêt si plus de commande depuis TIMEOUT
  if (!modeAuto && cmdMoteur != "STOP") {
    if (t - tDernierCmd > TIMEOUT_MOTEUR_MAN) {
      arreterMoteurs();
      cmdMoteur = "STOP";
    }
  }

  // ── Tracking automatique ───────────────────────────────────────────
  if (modeAuto && !alerteVent && (t - tDerniersTracking > INTERVAL_TRACKING)) {
    tDerniersTracking = t;

    float maxLux = luxNord;
    if (luxSud   > maxLux) maxLux = luxSud;
    if (luxEst   > maxLux) maxLux = luxEst;
    if (luxOuest > maxLux) maxLux = luxOuest;

    if (maxLux >= SEUIL_LUM) {
      ajouterLog("--- Tracking step ---");
      trackSun(motorEO, fdcES, fdcOU, luxEst,  luxOuest);  // axe Est-Ouest

      if (modeAuto && !alerteVent) {
        trackSun(motorIH, fdcIH, fdcIV, luxNord, luxSud);  // axe Inclinaison
      }
    } else {
      // Nuit ou ciel très couvert → position de repos
      ajouterLog("Lum < seuil. Position repos.");
      miseAPlat();
      retourEst();
    }
  }
}
