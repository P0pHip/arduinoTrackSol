#pragma once
#include <Arduino.h>

// =====================================================================
//   PARAMÈTRES CONFIGURABLES EN LIVE (admin web)
//
//   Définies dans settings.cpp.
//   Chargées depuis la NVS au boot (fallback sur les #define de config.h).
//   Sauvegardées en NVS via saveSettings() à chaque modification admin.
// =====================================================================

extern int vitMotEO;   // vitesse PWM moteur Est-Ouest  (0 – 255)
extern int vitMotIH;   // vitesse PWM moteur Inclinaison (0 – 255)
extern int seuilLum;   // seuil de luminosité pour le tracking (lux)
extern int seuilVent;  // seuil vitesse vent avant alerte (km/h)

// Charge les valeurs depuis la NVS ; à appeler une seule fois dans setup().
void loadSettings();

// Sauvegarde les valeurs courantes dans la NVS.
void saveSettings();
