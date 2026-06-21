# MAJ OTA via ElegantOTA + workflow CLion — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Permettre la mise à jour firmware sans câble (OTA) des deux trackers ESP32 sur le réseau local, via une page web ElegantOTA, avec un tutoriel pour CLion + plugin Flexible For Arduino.

**Architecture:** ElegantOTA (mode synchrone) se greffe sur le `WebServer server(80)` existant de `webserver_routes.cpp` et ajoute la route `/update`. Un drapeau d'état `otaEnCours` + le callback `onStart` coupent les moteurs et bloquent le tracking pendant le flash. Une version firmware (`FW_VERSION`) est exposée pour vérifier visuellement qu'une MAJ a pris effet.

**Tech Stack:** ESP32 Arduino core, `WebServer.h` (synchrone), bibliothèque ElegantOTA v3.x, build via Arduino CLI (utilisé par le plugin Flexible For Arduino dans CLion).

## Approche de vérification (lire avant de commencer)

Ce projet est du firmware embarqué **sans harnais de tests unitaires** et fortement couplé au matériel (moteurs, capteurs, WiFi). La vérification de chaque tâche repose donc sur :

1. **Gate de compilation (automatisable)** — commande canonique exécutée depuis la racine du dépôt :
   ```
   arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
   ```
   Attendu : `Sketch uses NNNNN bytes ... Global variables use ...` puis sortie sans erreur (code retour 0). Équivalent GUI : bouton **Build** du plugin Flexible For Arduino dans CLion.
2. **Vérification manuelle sur matériel (MANUEL)** — étapes marquées `MANUEL` nécessitant une carte flashée. Un agent qui exécute le plan sans matériel **coche le gate de compilation** et **laisse les étapes MANUEL à l'utilisateur** (les signaler dans le compte-rendu de tâche).

## Global Constraints

- Carte cible : `esp32:esp32:esp32da`, schéma de partition **`default`** (déjà compatible OTA `app0`/`app1` — **ne pas changer**).
- ElegantOTA en **mode synchrone** : le macro `ELEGANTOTA_USE_ASYNC_WEBSERVER` doit rester **non défini / à 0**. **Aucune** dépendance ESPAsyncWebServer ni AsyncTCP.
- Authentification OTA : **réutiliser `ADMIN_PIN`** — identifiant `"admin"`, mot de passe `ADMIN_PIN`.
- Pendant une MAJ : moteurs coupés (`arreterMoteurs()`), tracking auto et commandes manuelles bloqués tant que `otaEnCours == true`.
- `FW_VERSION` au format `"AAAA.MM.JJ"`, à incrémenter à chaque build pour que la vérification visuelle ait un sens.
- Portée : réseau local (LAN) uniquement.
- Licence ElegantOTA : AGPL-3.0 (usage personnel/académique).
- Style des messages de commit : anglais, impératif (`Add ...`), sans préfixe type `feat:`, sans body sauf nécessité (cf. historique du dépôt).

---

### Task 1: Ajouter la dépendance ElegantOTA et vérifier le build de référence

**Files:**
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino:15-18` (commentaire « BIBLIOTHÈQUES REQUISES »)

**Interfaces:**
- Consumes: rien.
- Produces: bibliothèque `ElegantOTA` installée dans l'environnement Arduino CLI ; en-tête du sketch documentant la dépendance.

- [ ] **Step 1: Vérifier que le core ESP32 est installé**

Run:
```
arduino-cli core list
```
Expected : la ligne `esp32:esp32` apparaît. Sinon : `arduino-cli core install esp32:esp32`.

- [ ] **Step 2: Installer ElegantOTA (et confirmer les libs existantes)**

Run:
```
arduino-cli lib install "ElegantOTA"
arduino-cli lib install "BH1750"
```
Expected : `Installed ElegantOTA@3.x.x` (et BH1750 déjà/maintenant installée). BTS7960 est installée manuellement depuis GitHub ; si absente, la signaler à l'utilisateur (lien dans l'en-tête du sketch).

- [ ] **Step 3: Confirmer le mode synchrone d'ElegantOTA**

Ouvrir `Documents/Arduino/libraries/ElegantOTA/src/ElegantOTA.h` et vérifier que la ligne du macro vaut `0` (mode synchrone, défaut) :
```cpp
#define ELEGANTOTA_USE_ASYNC_WEBSERVER 0
```
Ne rien modifier si c'est déjà `0` ou absent. **Ne pas** mettre `1` (cela exigerait ESPAsyncWebServer).

- [ ] **Step 4: Gate de compilation de référence (avant toute modif de code)**

Run:
```
arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
```
Expected : compilation réussie (`Sketch uses ... bytes`), code retour 0. Cela valide que la chaîne d'outils fonctionne sur le code actuel.

- [ ] **Step 5: Documenter la dépendance dans l'en-tête du sketch**

Dans `track_sol_esp32_bts7960_bh1750_mobile.ino`, remplacer le bloc :
```cpp
 * BIBLIOTHÈQUES REQUISES (Gestionnaire de bibliothèques Arduino IDE) :
 *   - BTS7960  : https://github.com/1337encrypted/BTS7960_Motordriver
 *   - BH1750   : https://github.com/claws/BH1750
 *   - WiFi, WebServer, HTTPClient : incluses dans le core ESP32
```
par :
```cpp
 * BIBLIOTHÈQUES REQUISES (Gestionnaire de bibliothèques Arduino IDE) :
 *   - BTS7960    : https://github.com/1337encrypted/BTS7960_Motordriver
 *   - BH1750     : https://github.com/claws/BH1750
 *   - ElegantOTA : https://github.com/ayushsharma82/ElegantOTA (MAJ OTA, mode synchrone)
 *   - WiFi, WebServer, HTTPClient, Update : inclus dans le core ESP32
```

- [ ] **Step 6: Re-compiler pour confirmer (le commentaire ne casse rien)**

Run:
```
arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
```
Expected : compilation réussie, code retour 0.

- [ ] **Step 7: Commit**

```bash
git add track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino
git commit -m "Document ElegantOTA dependency in sketch header"
```

---

### Task 2: Exposer la version firmware (FW_VERSION)

**Files:**
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/config.h` (ajout `#define FW_VERSION`)
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino` (log de démarrage)
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/webserver_routes.cpp` (champ JSON `fwVersion` dans `handleData`)

**Interfaces:**
- Consumes: rien.
- Produces: macro `FW_VERSION` (chaîne) disponible partout où `config.h` est inclus ; champ `"fwVersion"` (string) dans la réponse de `GET /data`.

- [ ] **Step 1: Ajouter FW_VERSION dans config.h**

Dans `config.h`, après le bloc `#if !defined(IS_MASTER) ... #endif`, ajouter :
```cpp

// ── Version firmware (incrémenter à chaque build pour vérifier les MAJ OTA) ──
#define FW_VERSION  "2026.06.21"
```

- [ ] **Step 2: Logguer la version au démarrage**

Dans `track_sol_esp32_bts7960_bh1750_mobile.ino`, dans `setup()`, juste après `loadSettings();`, ajouter :
```cpp
  ajouterLog("Firmware " FW_VERSION);
```

- [ ] **Step 3: Ajouter fwVersion au JSON de /data**

Dans `webserver_routes.cpp`, fonction `handleData()`, juste après la ligne `String json = "{";`, ajouter :
```cpp
  json += "\"fwVersion\":\"" FW_VERSION "\",";
```
(Concaténation de littéraux à la compilation — `FW_VERSION` vient de `config.h`, déjà inclus dans ce fichier.)

- [ ] **Step 4: Gate de compilation**

Run:
```
arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
```
Expected : compilation réussie, code retour 0.

- [ ] **Step 5: MANUEL — Vérifier le champ version**

Après flash USB : ouvrir `http://<IP>/data`, confirmer la présence de `"fwVersion":"2026.06.21"`. Confirmer la ligne `Firmware 2026.06.21` dans le journal de l'interface.

- [ ] **Step 6: Commit**

```bash
git add track_sol_esp32_bts7960_bh1750_mobile/config.h track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino track_sol_esp32_bts7960_bh1750_mobile/webserver_routes.cpp
git commit -m "Add FW_VERSION exposed in log and /data endpoint"
```

---

### Task 3: Intégrer ElegantOTA + sécurité moteurs (cœur de la fonctionnalité)

**Files:**
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/state.h` (déclaration `otaEnCours`)
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino` (définition `otaEnCours` + garde tracking)
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/webserver_routes.cpp` (include, callbacks, wiring serveur, loop, garde commande manuelle)

**Interfaces:**
- Consumes: `arreterMoteurs()` (de `motors.h`, déjà inclus dans `webserver_routes.cpp`) ; `ajouterLog(String)` (de `logger.h`) ; `ADMIN_PIN` (de `config.h`) ; instance locale `WebServer server`.
- Produces: variable globale `bool otaEnCours` (extern dans `state.h`) ; route `/update` (page + endpoint POST) servie par le serveur existant ; callbacks `onOTAStart/onOTAProgress/onOTAEnd`.

- [ ] **Step 1: Déclarer le drapeau otaEnCours dans state.h**

Dans `state.h`, après `extern String journal;`, ajouter :
```cpp

extern bool otaEnCours;     // true = MAJ OTA en cours — coupe moteurs et bloque le tracking
```

- [ ] **Step 2: Définir otaEnCours dans le .ino**

Dans `track_sol_esp32_bts7960_bh1750_mobile.ino`, dans le bloc de définitions d'état, après la ligne `bool   modeAutoAvantAlerte  = false;  // mode sauvegardé avant mise en sécurité vent`, ajouter :
```cpp
bool   otaEnCours           = false;  // MAJ OTA en cours — coupe moteurs et bloque le tracking
```

- [ ] **Step 3: Bloquer le tracking auto pendant une MAJ**

Dans `track_sol_esp32_bts7960_bh1750_mobile.ino`, fonction `loop()`, remplacer :
```cpp
  if (modeAuto && !alerteVent && (t - tDerniersTracking > INTERVAL_TRACKING)) {
```
par :
```cpp
  if (modeAuto && !alerteVent && !otaEnCours && (t - tDerniersTracking > INTERVAL_TRACKING)) {
```

- [ ] **Step 4: Inclure ElegantOTA dans webserver_routes.cpp**

Dans `webserver_routes.cpp`, après la ligne `#include "webserver_routes.h"`, ajouter :
```cpp
#include <ElegantOTA.h>
```

- [ ] **Step 5: Ajouter les callbacks OTA**

Dans `webserver_routes.cpp`, juste avant la section `// INTERFACE PUBLIQUE` (au-dessus de `void setupServeur()`), ajouter :
```cpp
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
```

- [ ] **Step 6: Câbler ElegantOTA dans setupServeur()**

Dans `webserver_routes.cpp`, fonction `setupServeur()`, remplacer la ligne `server.begin();` (la dernière de la fonction) par :
```cpp
  // ── OTA (ElegantOTA) — page protégée /update ───────────────────────
  ElegantOTA.setAuth("admin", ADMIN_PIN);
  ElegantOTA.begin(&server);            // enregistre les routes /update
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
```

- [ ] **Step 7: Appeler ElegantOTA.loop() dans tickServeur()**

Dans `webserver_routes.cpp`, fonction `tickServeur()`, remplacer :
```cpp
void tickServeur() {
  server.handleClient();
}
```
par :
```cpp
void tickServeur() {
  server.handleClient();
  ElegantOTA.loop();
}
```

- [ ] **Step 8: Bloquer la commande manuelle pendant une MAJ**

Dans `webserver_routes.cpp`, fonction `handleMoteur()`, remplacer :
```cpp
  if (modeAuto || alerteVent) {
    server.send(403, "text/plain", "indisponible");
    return;
  }
```
par :
```cpp
  if (modeAuto || alerteVent || otaEnCours) {
    server.send(403, "text/plain", "indisponible");
    return;
  }
```

- [ ] **Step 9: Gate de compilation**

Run:
```
arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
```
Expected : compilation réussie, code retour 0. (Si erreur mentionnant `AsyncWebServer` → ElegantOTA est en mode async ; revenir à Task 1 Step 3.)

- [ ] **Step 10: MANUEL — Vérifier l'OTA et la sécurité moteurs**

Après flash USB initial :
1. Ouvrir `http://<IP>/update` → le navigateur demande identifiant/mot de passe → entrer `admin` / `ADMIN_PIN` → la page ElegantOTA s'affiche.
2. En mode MANUEL, lancer un moteur (maintenir un bouton), puis démarrer un upload `.bin` : vérifier que le moteur s'arrête immédiatement et que le journal affiche `OTA: debut MAJ, moteurs coupes`.
3. Laisser l'upload se terminer : la carte redémarre sur le nouveau firmware.

- [ ] **Step 11: Commit**

```bash
git add track_sol_esp32_bts7960_bh1750_mobile/state.h track_sol_esp32_bts7960_bh1750_mobile/track_sol_esp32_bts7960_bh1750_mobile.ino track_sol_esp32_bts7960_bh1750_mobile/webserver_routes.cpp
git commit -m "Add ElegantOTA web updates with motor safety during flash"
```

---

### Task 4: Interface web — affichage version + lien OTA

**Files:**
- Modify: `track_sol_esp32_bts7960_bh1750_mobile/html_page.cpp` (footer version + lien `/update` dans le modal admin + maj JS)

**Interfaces:**
- Consumes: champ `d.fwVersion` de `GET /data` (Task 2) ; route `/update` (Task 3).
- Produces: aucun (UI terminale).

- [ ] **Step 1: Ajouter un style pour le pied de page version**

Dans `html_page.cpp`, dans le `<style>`, juste avant la ligne `</style>`, ajouter :
```css
    .fw-foot { text-align:center; color:#8b949e; font-size:0.7em; margin:4px 0 12px; }
```

- [ ] **Step 2: Ajouter le lien OTA dans le formulaire admin**

Dans `html_page.cpp`, dans le bloc `<!-- Étape 2 : formulaire de configuration -->`, juste avant `<div class="admin-save-msg" id="admin-save-msg"></div>`, ajouter :
```html
      <label class="admin-label">Mise &agrave; jour firmware (OTA)</label>
      <a class="btn-admin-cancel" style="display:block;text-align:center;text-decoration:none;margin-top:4px;"
         href="/update" target="_blank" rel="noopener">&#128228; Ouvrir la page OTA</a>
```

- [ ] **Step 3: Ajouter le pied de page version dans le HTML**

Dans `html_page.cpp`, juste après le bloc du bouton admin (`<button class="btn-admin-open" ...>...</button>`), ajouter :
```html
<div class="fw-foot" id="fw-foot">firmware &#8212;</div>
```

- [ ] **Step 4: Renseigner la version via le polling JS**

Dans `html_page.cpp`, fonction JS `majInterface(d)`, juste après `dernierData = d;`, ajouter :
```javascript
  if (d.fwVersion) document.getElementById('fw-foot').textContent = 'firmware ' + d.fwVersion;
```

- [ ] **Step 5: Gate de compilation**

Run:
```
arduino-cli compile --fqbn esp32:esp32:esp32da track_sol_esp32_bts7960_bh1750_mobile
```
Expected : compilation réussie, code retour 0.

- [ ] **Step 6: MANUEL — Vérifier l'UI**

Après flash : la page d'accueil affiche `firmware 2026.06.21` en pied de page. Dans Admin → après saisie du PIN, le bouton « Ouvrir la page OTA » ouvre `/update` (qui redemande `admin`/PIN via le navigateur).

- [ ] **Step 7: Commit**

```bash
git add track_sol_esp32_bts7960_bh1750_mobile/html_page.cpp
git commit -m "Show firmware version and add OTA link in admin panel"
```

---

### Task 5: Rédiger le tutoriel CLion + Flexible For Arduino

**Files:**
- Create: `docs/tutoriel-maj-ota-clion.md`

**Interfaces:**
- Consumes: l'ensemble de la fonctionnalité OTA livrée (Tasks 1–4).
- Produces: documentation utilisateur.

- [ ] **Step 1: Écrire le tutoriel**

Créer `docs/tutoriel-maj-ota-clion.md` avec le contenu suivant :

````markdown
# Déployer les mises à jour OTA depuis CLion (Flexible For Arduino)

Ce guide explique comment mettre à jour les deux trackers ESP32 **sans câble USB**,
sur le réseau WiFi local, en compilant dans **CLion** avec le plugin
**Flexible For Arduino** et en envoyant le firmware via le **navigateur** (ElegantOTA).

## Prérequis (une seule fois)

1. **Plugin** : CLion → Settings → Plugins → onglet Marketplace → chercher
   « Flexible For Arduino » → Install → redémarrer CLion.
2. **Backend Arduino CLI** : Settings → Languages & Frameworks → Arduino/ESP-IDF →
   renseigner le chemin du SDK Arduino CLI (le plugin peut l'installer).
3. **Core + bibliothèques** (via le gestionnaire du plugin ou en ligne de commande) :
   - Core : `esp32:esp32` (Espressif).
   - Libs : `BTS7960`, `BH1750`, `ElegantOTA`.
4. **Premier flash en USB obligatoire** : le firmware contenant ElegantOTA doit
   être installé une première fois par câble sur **chaque** carte. Ensuite, toutes
   les MAJ se font sans fil.

> ⚠️ L'OTA *intégré au plugin* Flexible For Arduino est réservé à la version **Pro
> (payante)**. On ne l'utilise pas : l'upload via la page web ElegantOTA est gratuit.

## À chaque mise à jour

1. **Choisir le rôle** dans `config.h` : décommenter `config_master.h` **ou**
   `config_slave.h` (un seul).
2. **Incrémenter** `FW_VERSION` dans `config.h` (format `AAAA.MM.JJ`) — c'est ce
   qui permettra de vérifier que la MAJ a pris.
3. **Sélectionner la carte** `esp32:esp32:esp32da`, partition `default`
   (déjà compatible OTA, ne rien changer).
4. **Build** (bouton Build du plugin, ou
   `arduino-cli compile --fqbn esp32:esp32:esp32da --output-dir build track_sol_esp32_bts7960_bh1750_mobile`).
   Récupérer le fichier `…ino.bin` (chemin affiché dans la console de build, ou
   dans `build/`).
5. **Uploader via le navigateur** :
   - Ouvrir `http://<IP_DE_LA_CARTE>/update`.
   - S'authentifier : identifiant `admin`, mot de passe = votre `ADMIN_PIN`.
   - Sélectionner le fichier `…ino.bin`, type **Firmware** → l'upload démarre,
     barre de progression, puis redémarrage automatique.
6. **Vérifier** : la page d'accueil doit afficher la nouvelle `FW_VERSION` en pied
   de page.
7. **Répéter pour l'autre tracker** :
   - Maître : son IP (relevée au démarrage sur le moniteur série, log
     « WiFi OK — http://… », ou dans l'interface de la box).
   - Esclave : `192.168.1.35` (IP fixe).

## Sécurité & bonnes pratiques

- Faire les MAJ de préférence en mode **MANUEL** et **hors alerte vent**.
- Les moteurs sont coupés automatiquement dès le début du flash ; le tracking et
  les commandes manuelles sont bloqués jusqu'au redémarrage.
- Un upload interrompu (coupure réseau/alimentation) laisse l'**ancien firmware**
  intact : il suffit de relancer l'upload. En dernier recours, re-flasher en USB.

## Dépannage

- **Erreur de compilation mentionnant `AsyncWebServer`** : ElegantOTA est passé en
  mode asynchrone. Ouvrir `…/libraries/ElegantOTA/src/ElegantOTA.h` et remettre
  `#define ELEGANTOTA_USE_ASYNC_WEBSERVER 0`.
- **`/update` inaccessible** : vérifier que la carte est bien connectée au WiFi
  (log série), et que vous utilisez la bonne IP.
- **Identifiants refusés** : le mot de passe OTA est votre `ADMIN_PIN`
  (identifiant `admin`).
- **IP du maître inconnue / changeante** : envisager une réservation DHCP sur la
  box (comme l'esclave en `.35`).
````

- [ ] **Step 2: Commit**

```bash
git add docs/tutoriel-maj-ota-clion.md
git commit -m "Add CLion OTA deployment tutorial"
```

---

## Self-Review (rempli par l'auteur du plan)

**Couverture spec :**
- §5.1 webserver_routes (include/callbacks/wiring/loop/garde moteur/handleData) → Tasks 2 & 3 ✔
- §5.2 state.h + .ino (flag + garde tracking) → Task 3 ✔
- §5.3 config.h FW_VERSION → Task 2 ✔
- §5.4 traçabilité (log + /data) → Task 2 ✔
- §5.5 html_page (version + lien OTA) → Task 4 ✔
- §6 dépendances (ElegantOTA, mode sync, pas d'Async) → Task 1 ✔
- §7 tutoriel CLion → Task 5 ✔
- §8 sécurité (moteurs/auth/robustesse) → Task 3 (moteurs/auth) + Task 5 (bonnes pratiques) ✔
- §9 vérification → étapes MANUEL des Tasks 2/3/4 ✔
- §11 points d'attention (1er flash USB, IP maître, bump version) → Task 5 ✔

**Placeholders :** aucun TODO/TBD ; tout le code est fourni.

**Cohérence des noms/types :** `otaEnCours` (bool) déclaré state.h / défini .ino / utilisé webserver_routes.cpp ; `onOTAStart/onOTAProgress/onOTAEnd` cohérents entre définition et enregistrement ; `FW_VERSION` (config.h) utilisé en log, /data, et lu côté JS via `d.fwVersion` ; route `/update` produite (Task 3) puis consommée (Task 4).