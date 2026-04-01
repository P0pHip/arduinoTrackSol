#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "state.h"
#include "logger.h"
#include "sensors.h"
#include "motors.h"
#include "html_page.h"
#include "webserver.h"

#if IS_MASTER
  #include "wind.h"   // pour valeurVent dans handleData()
#endif

// ── Objet serveur (local à ce module) ────────────────────────────────
static WebServer server(80);

// =====================================================================
//   ROUTE  GET /
//   Sert la page HTML complète.
// =====================================================================
static void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

// =====================================================================
//   ROUTE  GET /data
//   Retourne un objet JSON avec l'état complet du tracker.
//   Interrogé par le JavaScript toutes les secondes.
// =====================================================================
static void handleData() {
  String json = "{";
  json += "\"luxN\":"      + String(luxNord,  1) + ",";
  json += "\"luxS\":"      + String(luxSud,   1) + ",";
  json += "\"luxE\":"      + String(luxEst,   1) + ",";
  json += "\"luxO\":"      + String(luxOuest, 1) + ",";
  json += "\"fdcIH\":"     + String(fdcIH)       + ",";
  json += "\"fdcIV\":"     + String(fdcIV)       + ",";
  json += "\"fdcES\":"     + String(fdcES)       + ",";
  json += "\"fdcOU\":"     + String(fdcOU)       + ",";
  json += "\"modeAuto\":"  + String(modeAuto   ? "true" : "false") + ",";
  json += "\"alerteVent\":" + String(alerteVent ? "true" : "false") + ",";
#if IS_MASTER
  json += "\"ventVal\":"   + String(valeurVent) + ",";
#endif
  json += "\"journal\":\""  + echapperJson(journal) + "\"";
  json += "}";

  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "application/json", json);
}

// =====================================================================
//   ROUTE  POST /mode?auto=1|0
//   Bascule entre mode automatique et mode manuel.
// =====================================================================
static void handleMode() {
  if (!server.hasArg("auto")) { server.send(400, "text/plain", "arg manquant"); return; }

  bool nouveau = (server.arg("auto") == "1");
  if (nouveau != modeAuto) {
    modeAuto = nouveau;
    if (!modeAuto) arreterMoteurs();
    ajouterLog(String("Mode -> ") + (modeAuto ? "AUTO" : "MANUEL"));
  }
  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   ROUTE  POST /moteur?cmd=EO_F|EO_B|IH_F|IH_B|STOP
//   Commande manuelle des moteurs.
//   Ignorée en mode auto ou pendant une alerte vent.
// =====================================================================
static void handleMoteur() {
  if (modeAuto || alerteVent) {
    server.send(403, "text/plain", "indisponible");
    return;
  }
  if (!server.hasArg("cmd")) { server.send(400, "text/plain", "arg manquant"); return; }

  String cmd  = server.arg("cmd");
  tDernierCmd = millis();
  cmdMoteur   = cmd;

  if      (cmd == "EO_F") { motorEO.front(); motorIH.stop(); }
  else if (cmd == "EO_B") { motorEO.back();  motorIH.stop(); }
  else if (cmd == "IH_F") { motorIH.front(); motorEO.stop(); }
  else if (cmd == "IH_B") { motorIH.back();  motorEO.stop(); }
  else                    { arreterMoteurs(); cmdMoteur = "STOP"; }

  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   ROUTE  POST /alerte_vent
//   Reçue par l'ESCLAVE depuis le MAÎTRE.
//   Déclenche la mise en sécurité si pas encore active.
// =====================================================================
static void handleAlerteVent() {
  if (!alerteVent) {
    alerteVent = true;
    ajouterLog("Alerte vent recue du maitre !");
    mettreEnSecurite();
  }
  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   ROUTE  POST /reset_alerte_vent
//   Reçue par l'ESCLAVE quand le vent est retombé.
// =====================================================================
static void handleResetAlerteVent() {
  if (alerteVent) {
    alerteVent = false;
    modeAuto   = true;
    ajouterLog("Alerte vent levee (info du maitre). Reprise auto.");
  }
  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   INTERFACE PUBLIQUE
// =====================================================================
void setupServeur() {
  server.on("/",                  HTTP_GET,  handleRoot);
  server.on("/data",              HTTP_GET,  handleData);
  server.on("/mode",              HTTP_POST, handleMode);
  server.on("/moteur",            HTTP_POST, handleMoteur);
  server.on("/alerte_vent",       HTTP_POST, handleAlerteVent);
  server.on("/reset_alerte_vent", HTTP_POST, handleResetAlerteVent);
  server.begin();
}

void tickServeur() {
  server.handleClient();
}
