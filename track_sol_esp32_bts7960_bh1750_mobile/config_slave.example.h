#pragma once

// =====================================================================
//   CONFIGURATION TRACKER ESCLAVE — Template
//   Copier ce fichier en config_slave.h et renseigner les valeurs.
// =====================================================================

#define IS_MASTER  false

// ── WiFi ─────────────────────────────────────────────────────────────
#define WIFI_SSID      "VOTRE_SSID"
#define WIFI_PASSWORD  "VOTRE_MOT_DE_PASSE"

// IP fixe de l'esclave (utilisée par le maître).
#define SLAVE_IP  "192.168.1.XXX"

// ── Pins moteur EO (Est-Ouest) — BTS7960 ─────────────────────────────
#define RpwmEO  15
#define LpwmEO  2
#define LenEO   13
#define RenEO   4

// ── Pins moteur IH (Inclinaison) — BTS7960 ───────────────────────────
#define RpwmIH  17
#define LpwmIH  5
#define LenIH   18
#define RenIH   19

// ── Pins fins de course ───────────────────────────────────────────────
#define PIN_FDC_IH     32
#define PIN_FDC_IV     33
#define PIN_FDC_EST    16
#define PIN_FDC_OUEST  23

// ── Capteur de vent (seuil partagé — défaut settings.cpp) ────────────
#define WIND_THRESHOLD_KMH  50
#define WIND_CALM_DELAY_MS  300000UL  // délai stabilisation vent (géré par le maître)

// ── Code PIN admin (8 chiffres) ───────────────────────────────────────
#define ADMIN_PIN  "00000000"

// ── Multiplexeur I²C ─────────────────────────────────────────────────
#define TCA9548A_ADDR  0x70

// ── Vitesses moteurs (0 – 255) ────────────────────────────────────────
#define VIT_MOT_EO  255
#define VIT_MOT_IH  255

// ── Seuil de luminosité pour le tracking (lux) ────────────────────────
#define SEUIL_LUM  300

// ── Intervalles de temps (millisecondes) ─────────────────────────────
#define INTERVAL_CAPTEURS      2000UL
#define INTERVAL_TRACKING     90000UL
#define INTERVAL_VENT           500UL
#define TIMEOUT_MOTEUR_MAN      600UL
#define DELAY_AUTO_START_MS  120000UL   // délai avant passage auto en AUTO au démarrage (2 min)

// ── Journal ───────────────────────────────────────────────────────────
#define JOURNAL_MAX_LIGNES  25
