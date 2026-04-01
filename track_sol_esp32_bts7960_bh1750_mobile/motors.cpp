#include <Arduino.h>
#include <BTS7960.h>
#include "config.h"
#include "state.h"
#include "logger.h"
#include "sensors.h"
#include "webserver.h"  // uniquement pour tickServeur()
#include "motors.h"

// ── Définition des objets moteurs ─────────────────────────────────────
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH);

// ── Initialisation ────────────────────────────────────────────────────
void setupMoteurs() {
  motorEO.begin(); motorEO.enable(); motorEO.pwm = VIT_MOT_EO;
  motorIH.begin(); motorIH.enable(); motorIH.pwm = VIT_MOT_IH;
}

// ── Arrêt d'urgence ───────────────────────────────────────────────────
void arreterMoteurs() {
  motorEO.stop();
  motorIH.stop();
}

// ── Mouvements de sécurité ────────────────────────────────────────────

// Inclinaison → position à plat (FDC_IH déclenché = position repos)
void miseAPlat() {
  ajouterLog("Mise a plat...");
  while (digitalRead(PIN_FDC_IH) == HIGH) {
    tickServeur();
    motorIH.back();
    delay(10);
  }
  motorIH.stop();
  delay(300);
}

// Rotation → orientation Est (FDC_EST déclenché = position matin)
void retourEst() {
  ajouterLog("Retour vers l'Est...");
  while (digitalRead(PIN_FDC_EST) == HIGH) {
    tickServeur();
    motorEO.back();
    delay(10);
  }
  motorEO.stop();
  delay(300);
}

// Séquence complète : forcer le mode manuel, arrêter, plat, Est
void mettreEnSecurite() {
  ajouterLog("Mise en securite : arret + plat + Est.");
  modeAuto = false;
  arreterMoteurs();
  miseAPlat();
  retourEst();
  ajouterLog("Position securite atteinte.");
}

// ── Tracking solaire ──────────────────────────────────────────────────
// Déplace un moteur jusqu'à équilibrer lux1 et lux2, dans la limite des FDC.
// tickServeur() est appelé à chaque itération pour ne pas bloquer le web server.
void trackSun(BTS7960& motor, int& fdc1, int& fdc2, float& lux1, float& lux2) {
  if (lux1 > lux2) {
    // lux1 est plus forte → avancer vers fdc1
    while (fdc1 == HIGH && modeAuto && !alerteVent) {
      tickServeur();
      motor.back();
      lireCapteurs();         // met à jour lux1, lux2, fdc1, fdc2
      if (lux1 <= lux2) break;
    }
  } else {
    // lux2 est plus forte → avancer vers fdc2
    while (fdc2 == HIGH && modeAuto && !alerteVent) {
      tickServeur();
      motor.front();
      lireCapteurs();
      if (lux1 >= lux2) break;
    }
  }
  motor.stop();
}
