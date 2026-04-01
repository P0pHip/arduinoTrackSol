#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include "config.h"
#include "logger.h"
#include "sensors.h"

// ── Définition des objets et variables (exportés via sensors.h) ───────
BH1750 captNord, captSud, captEst, captOuest;

float luxNord = 0, luxSud = 0, luxEst = 0, luxOuest = 0;
int   fdcIH = HIGH, fdcIV = HIGH, fdcES = HIGH, fdcOU = HIGH;

// ── Sélection du canal TCA9548A ───────────────────────────────────────
void tcaSelect(uint8_t canal) {
  if (canal > 7) return;
  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << canal);
  Wire.endTransmission();
}

// ── Initialisation ────────────────────────────────────────────────────
void setupCapteurs() {
  // Capteurs BH1750 via multiplexeur I²C
  tcaSelect(0); captNord.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  tcaSelect(1); captSud.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  tcaSelect(2); captEst.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  tcaSelect(3); captOuest.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // Fins de course (INPUT_PULLUP → état normal = HIGH)
  pinMode(PIN_FDC_IH,    INPUT_PULLUP);
  pinMode(PIN_FDC_IV,    INPUT_PULLUP);
  pinMode(PIN_FDC_EST,   INPUT_PULLUP);
  pinMode(PIN_FDC_OUEST, INPUT_PULLUP);
}

// ── Lecture de tous les capteurs ─────────────────────────────────────
void lireCapteurs() {
  // Luminosité
  tcaSelect(0); luxNord  = captNord.readLightLevel();
  tcaSelect(1); luxSud   = captSud.readLightLevel();
  tcaSelect(2); luxEst   = captEst.readLightLevel();
  tcaSelect(3); luxOuest = captOuest.readLightLevel();

  // Fins de course
  fdcIH = digitalRead(PIN_FDC_IH);
  fdcIV = digitalRead(PIN_FDC_IV);
  fdcES = digitalRead(PIN_FDC_EST);
  fdcOU = digitalRead(PIN_FDC_OUEST);

  ajouterLog("Lum N=" + String(luxNord,1) + " S=" + String(luxSud,1)
             + " E=" + String(luxEst,1) + " O=" + String(luxOuest,1) + " lx");
  ajouterLog("FDC IH=" + String(fdcIH) + " IV=" + String(fdcIV)
             + " ES=" + String(fdcES) + " OU=" + String(fdcOU));
}
