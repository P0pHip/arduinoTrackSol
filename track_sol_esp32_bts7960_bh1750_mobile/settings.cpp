#include <Preferences.h>
#include "settings.h"
#include "config.h"

// =====================================================================
//   DÉFINITION DES VARIABLES RUNTIME
//   Initialisées avec les valeurs par défaut de config.h.
//   Écrasées par loadSettings() si des valeurs NVS existent.
// =====================================================================

int vitMotEO  = VIT_MOT_EO;
int vitMotIH  = VIT_MOT_IH;
int seuilLum  = SEUIL_LUM;
int seuilVent = WIND_THRESHOLD_KMH;

// ── Chargement depuis la NVS ─────────────────────────────────────────
void loadSettings() {
  Preferences prefs;
  prefs.begin("tracker", true);  // lecture seule
  vitMotEO  = prefs.getInt("vitMotEO",  VIT_MOT_EO);
  vitMotIH  = prefs.getInt("vitMotIH",  VIT_MOT_IH);
  seuilLum  = prefs.getInt("seuilLum",  SEUIL_LUM);
  seuilVent = prefs.getInt("seuilVent", WIND_THRESHOLD_KMH);
  prefs.end();
}

// ── Sauvegarde dans la NVS ───────────────────────────────────────────
void saveSettings() {
  Preferences prefs;
  prefs.begin("tracker", false);  // lecture-écriture
  prefs.putInt("vitMotEO",  vitMotEO);
  prefs.putInt("vitMotIH",  vitMotIH);
  prefs.putInt("seuilLum",  seuilLum);
  prefs.putInt("seuilVent", seuilVent);
  prefs.end();
}
