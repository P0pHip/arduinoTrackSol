#include <Arduino.h>
#include "config.h"

#if IS_MASTER

#include <HTTPClient.h>
#include "state.h"
#include "logger.h"
#include "motors.h"
#include "wind.h"

int valeurVent = 0;

void setupVent() {
  pinMode(WIND_PIN, INPUT);
  ajouterLog("[MAITRE] Capteur de vent actif, pin=" + String(WIND_PIN)
             + ", seuil=" + String(WIND_THRESHOLD));
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

void verifierVent() {
  valeurVent = analogRead(WIND_PIN);

  if (valeurVent > WIND_THRESHOLD && !alerteVent) {
    // Vent trop fort : mise en sécurité + notification esclave
    alerteVent = true;
    ajouterLog("ALERTE VENT ! val=" + String(valeurVent)
               + " > seuil=" + String(WIND_THRESHOLD));
    mettreEnSecurite();
    notifierEsclave(true);

  } else if (valeurVent <= WIND_THRESHOLD && alerteVent) {
    // Vent retombé : reprendre le mode automatique + notification esclave
    alerteVent = false;
    modeAuto   = true;
    ajouterLog("Vent retombe (val=" + String(valeurVent) + "). Reprise auto.");
    notifierEsclave(false);
  }
}

#endif // IS_MASTER
