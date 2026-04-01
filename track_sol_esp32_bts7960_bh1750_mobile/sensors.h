#pragma once
#include <BH1750.h>

// ── Valeurs de luminosité (lux) ───────────────────────────────────────
extern float luxNord, luxSud, luxEst, luxOuest;

// ── États des fins de course (HIGH = non déclenché, LOW = déclenché) ──
extern int fdcIH, fdcIV, fdcES, fdcOU;

// ── Objets capteurs ───────────────────────────────────────────────────
extern BH1750 captNord, captSud, captEst, captOuest;

// ── Fonctions ─────────────────────────────────────────────────────────
void tcaSelect(uint8_t canal);  // sélectionne un canal du multiplexeur TCA9548A
void setupCapteurs();           // initialise BH1750 + pins FDC
void lireCapteurs();            // lit BH1750 + FDC et alimente les variables globales
