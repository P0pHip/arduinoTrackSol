#pragma once

// =====================================================================
//   CONFIGURATION — Copier ce fichier en config.h et remplir les valeurs
// =====================================================================

// ── Rôle de ce tracker ───────────────────────────────────────────────
// true  → ce tracker a le capteur de vent (MAÎTRE)
// false → ce tracker reçoit les alertes vent (ESCLAVE)
#define IS_MASTER  false

// ── WiFi ─────────────────────────────────────────────────────────────
#define WIFI_SSID      "VOTRE_SSID"
#define WIFI_PASSWORD  "VOTRE_MOT_DE_PASSE"

// IP fixe de l'esclave : notée au démarrage sur le moniteur série de l'esclave
// (seulement utile si IS_MASTER = true)
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
#define PIN_FDC_IH     32   // Haut / Nord  →  position à plat (repos)
#define PIN_FDC_IV     33   // Bas  / Sud
#define PIN_FDC_EST    16   // Est           →  position matin (repos)
#define PIN_FDC_OUEST  23   // Ouest

// ── Capteur de vent (MAÎTRE uniquement) ──────────────────────────────
#if IS_MASTER
  #define WIND_PIN            34    // Pin ADC1 analogique (0 – 4095 sur ESP32)
  #define WIND_SAMPLE_MS     1000   // durée de mesure par lecture (ms) — 1s = mesure propre
  #define WIND_THRESHOLD_KMH   50   // vitesse maxi avant alerte (km/h) — 1 tr/s = 2.4 km/h
#endif

// ── Multiplexeur I²C ─────────────────────────────────────────────────
#define TCA9548A_ADDR  0x70

// ── Vitesses moteurs (0 – 255) ────────────────────────────────────────
#define VIT_MOT_EO  255
#define VIT_MOT_IH  255

// ── Seuil de luminosité pour le tracking (lux) ────────────────────────
#define SEUIL_LUM  300

// ── Intervalles de temps (millisecondes) ─────────────────────────────
#define INTERVAL_CAPTEURS    2000UL
#define INTERVAL_TRACKING   20000UL
#define INTERVAL_VENT         500UL
#define TIMEOUT_MOTEUR_MAN    600UL

// ── Journal ───────────────────────────────────────────────────────────
#define JOURNAL_MAX_LIGNES  25
