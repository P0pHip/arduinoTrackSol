#pragma once

// =====================================================================
//   CONFIGURATION — TOUT CE QUI EST À MODIFIER EST ICI
// =====================================================================

// ── Rôle de ce tracker ───────────────────────────────────────────────
// true  → ce tracker a le capteur de vent (MAÎTRE)
// false → ce tracker reçoit les alertes vent (ESCLAVE)
#define IS_MASTER  true

// ── WiFi ─────────────────────────────────────────────────────────────
#define WIFI_SSID      "TON_SSID"
#define WIFI_PASSWORD  "TON_MOT_DE_PASSE"

// IP fixe de l'esclave : notée au démarrage sur le moniteur série de l'esclave
// (seulement utile si IS_MASTER = true)
#define SLAVE_IP  "192.168.1.100"

// ── Pins moteur EO (Est-Ouest) — BTS7960 ─────────────────────────────
#define RpwmEO  15
#define LpwmEO  2
#define LenEO   0
#define RenEO   4

// ── Pins moteur IH (Inclinaison) — BTS7960 ───────────────────────────
#define RpwmIH  17
#define LpwmIH  5
#define LenIH   18
#define RenIH   19

// ── Pins fins de course ───────────────────────────────────────────────
#define PIN_FDC_IH     25   // Haut / Nord  →  position à plat (repos)
#define PIN_FDC_IV     26   // Bas  / Sud
#define PIN_FDC_EST    27   // Est           →  position matin (repos)
#define PIN_FDC_OUEST  14   // Ouest

// ── Capteur de vent (MAÎTRE uniquement) ──────────────────────────────
#if IS_MASTER
  #define WIND_PIN        34    // Pin ADC analogique (0 – 4095 sur ESP32)
  #define WIND_THRESHOLD  100   // Valeur brute ADC au-delà de laquelle on alerte
#endif

// ── Multiplexeur I²C ─────────────────────────────────────────────────
#define TCA9548A_ADDR  0x70

// ── Vitesses moteurs (0 – 255) ────────────────────────────────────────
#define VIT_MOT_EO  255
#define VIT_MOT_IH  80

// ── Seuil de luminosité pour le tracking (lux) ────────────────────────
#define SEUIL_LUM  300

// ── Intervalles de temps (millisecondes) ─────────────────────────────
#define INTERVAL_CAPTEURS    2000UL   // lecture des capteurs de lum + FDC
#define INTERVAL_TRACKING   20000UL   // entre deux steps de tracking auto
#define INTERVAL_VENT         500UL   // lecture du capteur de vent
#define TIMEOUT_MOTEUR_MAN    600UL   // arrêt auto moteur si plus de commande web

// ── Journal ───────────────────────────────────────────────────────────
#define JOURNAL_MAX_LIGNES  25        // nombre de lignes conservées en mémoire
