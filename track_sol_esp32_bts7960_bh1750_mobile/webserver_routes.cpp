#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "state.h"
#include "logger.h"
#include "sensors.h"
#include "motors.h"
#include "settings.h"
#include "html_page.h"
#include "webserver_routes.h"
#include <ElegantOTA.h>

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
  json += "\"fwVersion\":\"" FW_VERSION "\",";
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
  json += "\"action\":\""  + actionCourante + "\",";
#if IS_MASTER
  json += "\"ventVal\":"   + String(valeurVent) + ",";
#endif
  json += "\"journal\":\""  + echapperJson(journal) + "\",";
  json += "\"vitEO\":"     + String(vitMotEO)  + ",";
  json += "\"vitIH\":"     + String(vitMotIH)  + ",";
  json += "\"seuilLum\":"        + String(seuilLum)        + ",";
  json += "\"seuilVent\":"       + String(seuilVent)       + ",";
  json += "\"delaiRepriseMin\":" + String(delaiRepriseMin);
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
  autoStartPending = false;   // l'utilisateur prend la main, annuler le passage auto différé
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
  if (modeAuto || alerteVent || otaEnCours) {
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
    modeAuto   = modeAutoAvantAlerte;   // restaure le mode d'avant l'alerte
    ajouterLog("Alerte vent levee (info du maitre). Reprise mode "
               + String(modeAuto ? "AUTO" : "MANUEL") + ".");
  }
  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   ROUTE  POST /admin/login?pin=XXXXXXXX
//   Vérifie le PIN admin. Répond {"ok":true} ou {"ok":false}.
// =====================================================================
static void handleAdminLogin() {
  if (!server.hasArg("pin")) { server.send(400, "text/plain", "arg manquant"); return; }
  bool ok = (server.arg("pin") == ADMIN_PIN);
  server.send(200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false}");
}

// =====================================================================
//   ROUTE  POST /admin/config?pin=XX&vitEO=X&vitIH=X&seuilLum=X&seuilVent=X
//   Vérifie le PIN, met à jour les paramètres en RAM et les sauvegarde en NVS.
// =====================================================================
static void handleAdminConfig() {
  if (!server.hasArg("pin") || server.arg("pin") != ADMIN_PIN) {
    server.send(403, "text/plain", "PIN incorrect");
    return;
  }

  if (server.hasArg("vitEO"))         vitMotEO        = constrain(server.arg("vitEO").toInt(),         0, 255);
  if (server.hasArg("vitIH"))         vitMotIH        = constrain(server.arg("vitIH").toInt(),         0, 255);
  if (server.hasArg("seuilLum"))      seuilLum        = constrain(server.arg("seuilLum").toInt(),      0, 100000);
  if (server.hasArg("seuilVent"))     seuilVent       = constrain(server.arg("seuilVent").toInt(),     0, 300);
  if (server.hasArg("delaiReprise"))  delaiRepriseMin = constrain(server.arg("delaiReprise").toInt(),  1, 999);

  applyMotorSettings();
  saveSettings();

  ajouterLog("Admin: vitEO=" + String(vitMotEO) + " vitIH=" + String(vitMotIH)
             + " lumSeuil=" + String(seuilLum) + " ventSeuil=" + String(seuilVent)
             + " delaiReprise=" + String(delaiRepriseMin) + "min");

  server.send(200, "text/plain", "OK");
}

// =====================================================================
//   CALLBACKS OTA (ElegantOTA)
//   Sécurité : couper les moteurs et bloquer le tracking pendant le flash.
// =====================================================================
static void onOTAStart() {
  arreterMoteurs();
  otaEnCours = true;
  ajouterLog("OTA: debut MAJ, moteurs coupes");
}

static void onOTAProgress(size_t current, size_t final) {
  Serial.printf("OTA: %u / %u octets\n", (unsigned)current, (unsigned)final);
}

static void onOTAEnd(bool success) {
  if (success) {
    ajouterLog("OTA: succes, redemarrage...");
  } else {
    otaEnCours = false;
    ajouterLog("OTA: ECHEC de la MAJ");
  }
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
  server.on("/admin/login",       HTTP_POST, handleAdminLogin);
  server.on("/admin/config",      HTTP_POST, handleAdminConfig);

  // ── OTA (ElegantOTA) — page protégée /update ───────────────────────
  ElegantOTA.setAuth("admin", ADMIN_PIN);
  ElegantOTA.begin(&server);            // enregistre les routes /update
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
}

void tickServeur() {
  server.handleClient();
  ElegantOTA.loop();
}
