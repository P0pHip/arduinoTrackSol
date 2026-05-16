#pragma once

// =====================================================================
//   CONFIGURATION TRACKER MAÎTRE — Template
//   Copier ce fichier en config_master.h et renseigner les valeurs.
// =====================================================================

#define IS_MASTER  true

// ── WiFi ─────────────────────────────────────────────────────────────
#define WIFI_SSID      "VOTRE_SSID"
#define WIFI_PASSWORD  "VOTRE_MOT_DE_PASSE"

// IP fixe de l'esclave : notée au démarrage sur son moniteur série.
#define SLAVE_IP  "192.168.1.XXX"

// ── Pins moteur EO (Est-Ouest) — BTS7960 ─────────────────────────────
#define RpwmEO  15
#define LpwmEO  2
#define LenEO   13  // GPIO0 évité (pin BOOT ESP32)
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

// ── Capteur de vent ───────────────────────────────────────────────────
#define WIND_THRESHOLD_KMH  50
#define WIND_PIN          34
#define WIND_SAMPLE_MS  1000

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
#define INTERVAL_CAPTEURS    2000UL
#define INTERVAL_TRACKING   90000UL
#define INTERVAL_VENT         500UL
#define TIMEOUT_MOTEUR_MAN    600UL

// ── Journal ───────────────────────────────────────────────────────────
#define JOURNAL_MAX_LIGNES  25
