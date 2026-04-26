#include <Arduino.h>
#include "config.h"

#if IS_MASTER

#include <HTTPClient.h>
#include "state.h"
#include "logger.h"
#include "motors.h"
#include "settings.h"
#include "wind.h"

int valeurVent = 0;

void setupVent() {
  pinMode(WIND_PIN, INPUT);
  ajouterLog("[MAITRE] Capteur de vent actif, pin=" + String(WIND_PIN)
             + ", seuil=" + String(seuilVent) + " km/h"
             + ", fenetre=" + String(WIND_SAMPLE_MS) + "ms");
}

void notifierEsclave(bool enAlerte) {
  HTTPClient http;
  String url = String("http://") + SLAVE_IP
               + (enAlerte ? "/alerte_vent" : "/reset_alerte_vent");

  http.begin(url);
  http.setTimeout(1500);
  int code = http.POST("");
  http.end();

  ajouterLog("Esclave notifie : " + url + " -> HTTP " + String(code));
}

// Mesure la vitesse du vent sur WIND_SAMPLE_MS ms.
// Capteur reed : chaque impulsion (front descendant vers 0) = 1 tour = 2.4 km/h à 1 tr/s.
// Vitesse (km/h) = impulsions * 2.4 * (1000 / WIND_SAMPLE_MS)
static int mesurerVitesse() {
  int impulsions = 0;
  unsigned long fin = millis() + WIND_SAMPLE_MS;
  bool dernierEtatBas = false;

  while (millis() < fin) {
    bool bas = (analogRead(WIND_PIN) < 100);
    if (bas && !dernierEtatBas) impulsions++;   // front descendant = 1 tour
    dernierEtatBas = bas;
    delay(5);
  }
  return (int)(impulsions * 2.4f * (1000.0f / WIND_SAMPLE_MS));
}

void verifierVent() {
  valeurVent = mesurerVitesse();   // km/h

  if (valeurVent > seuilVent && !alerteVent) {
    alerteVent = true;
    ajouterLog("ALERTE VENT ! " + String(valeurVent)
               + " km/h > seuil=" + String(seuilVent) + " km/h");
    mettreEnSecurite();
    notifierEsclave(true);

  } else if (valeurVent <= seuilVent && alerteVent) {
    alerteVent = false;
    modeAuto   = true;
    ajouterLog("Vent retombe (" + String(valeurVent) + " km/h). Reprise auto.");
    notifierEsclave(false);
  }
}

#endif // IS_MASTER
