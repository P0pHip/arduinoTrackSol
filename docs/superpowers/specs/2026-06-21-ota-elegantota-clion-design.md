# Spec — Mise à jour OTA (sans câble) via ElegantOTA + workflow CLion

- **Date** : 2026-06-21
- **Projet** : `track_sol_esp32_bts7960_bh1750_mobile`
- **Auteur** : Barret Mathieu (assisté)
- **Statut** : validé pour passage au plan d'implémentation

## 1. Objectif

Permettre le déploiement des mises à jour firmware **sans câble USB**, sur les
**deux trackers ESP32** (maître + esclave), quand ils sont sur le **même réseau
WiFi** (`BOX-BARRET`) que le PC de développement.

Le développement/compilation se fait depuis **CLion** avec le plugin
**Flexible For Arduino** ; l'upload du firmware se fait **via le navigateur**
(page `/update` servie par l'ESP32).

## 2. Contexte existant

- 2 ESP32 (`esp32:esp32:esp32da`, flash 4 Mo, schéma de partition `default`).
- Rôles via `config.h` qui inclut soit `config_master.h` soit `config_slave.h`.
- WiFi STA, serveur HTTP **synchrone** `WebServer server(80)` dans
  `webserver_routes.cpp`.
- Authentification admin déjà présente : `ADMIN_PIN` (8 chiffres).
- Code modulaire : `config`, `state`, `logger`, `sensors`, `motors`, `wind`,
  `webserver_routes`, `html_page`, `settings`.

## 3. Décisions de conception (validées)

| Sujet | Décision |
|-------|----------|
| Portée | LAN uniquement (même WiFi). Pas d'OTA depuis Internet. |
| Mécanisme | **ElegantOTA v3** (open-source, AGPL-3.0), upload `.bin` par navigateur. |
| Compatibilité serveur | Mode **synchrone** avec la `WebServer.h` existante — **pas** d'ESPAsyncWebServer/AsyncTCP. |
| Sécurité | Authentification HTTP Basic, **réutilise `ADMIN_PIN`** (identifiant `admin`, mot de passe = `ADMIN_PIN`). |
| Sécurité moteurs | Callback `onStart` → `arreterMoteurs()` + drapeau `otaEnCours` qui bloque tracking et commandes manuelles. |
| Traçabilité | `#define FW_VERSION` affichée dans l'interface + journal, pour vérifier qu'une MAJ a bien pris. |
| Interface | Lien « Mise à jour OTA » dans le panneau admin → `/update`. |
| IDE | CLion + plugin **Flexible For Arduino** (backend **Arduino CLI**, version gratuite). |

## 4. Architecture de la solution

ElegantOTA s'enregistre sur l'instance `server` existante et ajoute la route
`/update` (page web + endpoint POST du `.bin`). Aucune nouvelle tâche, aucun
nouveau serveur. La boucle principale appelle `ElegantOTA.loop()` (via
`tickServeur()`) pour gérer le redémarrage post-MAJ.

```
Navigateur (PC ou téléphone)
        │  http://<IP>/update  (Basic Auth: admin / ADMIN_PIN)
        ▼
WebServer:80  ──►  ElegantOTA  ──►  écrit la nouvelle image dans la
   (existant)                       partition OTA inactive, puis reboot
        │
        ├─ onStart()  : arreterMoteurs() + otaEnCours = true
        ├─ onProgress(): (optionnel) trace série
        └─ onEnd(ok)  : log ; si échec → otaEnCours = false
```

Le schéma de partition `default` (4 Mo) contient déjà deux partitions
applicatives (`app0`/`app1`) → **OTA compatible sans modification**. L'image
écrite va dans la partition inactive : un upload échoué ou une coupure
d'alimentation laisse l'**ancien firmware intact** (pas de brique).

## 5. Modifications firmware (réalisées par l'assistant)

### 5.1 `webserver_routes.cpp`
- `#include <ElegantOTA.h>`.
- Définir 3 callbacks statiques :
  ```cpp
  static void onOTAStart() {
    arreterMoteurs();
    otaEnCours = true;
    ajouterLog("OTA: debut MAJ, moteurs coupes");
  }
  static void onOTAProgress(size_t current, size_t final) {
    // optionnel : Serial.printf("OTA %u/%u\n", current, final);
  }
  static void onOTAEnd(bool success) {
    if (success) ajouterLog("OTA: succes, redemarrage...");
    else { otaEnCours = false; ajouterLog("OTA: ECHEC MAJ"); }
  }
  ```
- Dans `setupServeur()`, **avant** `server.begin()` :
  ```cpp
  ElegantOTA.setAuth("admin", ADMIN_PIN);
  ElegantOTA.begin(&server);          // enregistre les routes /update
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  ```
- Dans `tickServeur()` : ajouter `ElegantOTA.loop();`.
- `handleMoteur()` : refuser la commande si `otaEnCours` (ajout à la garde
  existante `modeAuto || alerteVent`).
- `handleData()` : ajouter `"fwVersion"` au JSON (voir 5.4).

### 5.2 `state.h` + fichier `.ino`
- `state.h` : `extern bool otaEnCours;`.
- `.ino` : définition `bool otaEnCours = false;` à côté des autres états.
- `.ino`, garde du tracking dans `loop()` :
  `if (modeAuto && !alerteVent && !otaEnCours && (t - tDerniersTracking > ...))`.

### 5.3 `config.h`
- Ajouter `#define FW_VERSION "2026.06.21"` (chaîne, partagée par les deux
  rôles). Convention proposée : date de build `AAAA.MM.JJ` (incrémentée à
  chaque MAJ pour vérification visuelle).

### 5.4 Traçabilité version
- `handleData()` : `json += "\"fwVersion\":\"" FW_VERSION "\",";`.
- Log au démarrage dans `setup()` : `ajouterLog("Firmware " FW_VERSION);`.

### 5.5 `html_page.cpp`
- Afficher la version firmware (pied de page ou en-tête).
- Ajouter dans le panneau admin un lien **« Mise à jour OTA »** ouvrant
  `/update` (utile depuis le téléphone). Récupérer `fwVersion` depuis `/data`.

## 6. Dépendances

- **ElegantOTA** (Ayush Sharma), v3.x — via le gestionnaire de bibliothèques
  Arduino / Arduino CLI. Mode synchrone compatible `WebServer.h` : **aucune**
  dépendance Async supplémentaire.
- Bibliothèques existantes conservées : BTS7960, BH1750.
- Licence ElegantOTA : **AGPL-3.0** (acceptable pour un usage personnel /
  académique).

## 7. Tutoriel CLion + Flexible For Arduino (livrable)

Document séparé : `docs/tutoriel-maj-ota-clion.md`. Contenu :

1. **Installer le plugin** Flexible For Arduino (Marketplace JetBrains, puis
   redémarrer CLion).
2. **Configurer le backend Arduino CLI** : Settings → Languages & Frameworks →
   Arduino/ESP-IDF (chemin du SDK). Installer le package carte **ESP32**
   (Espressif) et les bibliothèques **BTS7960, BH1750, ElegantOTA**.
3. **Ouvrir le sketch** `track_sol_esp32_bts7960_bh1750_mobile.ino` ;
   sélectionner la carte `esp32:esp32:esp32da`, partition `default`.
4. **Choisir le rôle** : décommenter `config_master.h` ou `config_slave.h`
   dans `config.h`. Mettre à jour `FW_VERSION`.
5. **Build** → localiser le fichier `....ino.bin` (chemin affiché dans la
   console de build ; fallback `arduino-cli compile --fqbn esp32:esp32:esp32da
   --output-dir build .`).
6. **Premier déploiement (une seule fois) en USB** pour installer le firmware
   contenant ElegantOTA.
7. **Déploiements suivants, sans câble** : navigateur → `http://<IP>/update`
   → login `admin` / `ADMIN_PIN` → déposer le `.bin` → barre de progression →
   reboot automatique. Vérifier que `FW_VERSION` a changé dans l'interface.
8. **Faire les deux trackers** : maître (son IP, vue au démarrage sur le
   moniteur série / box) et esclave (`192.168.1.35`).
9. **Note** : l'OTA *intégré au plugin* est réservé à la version **Pro
   (payante)** ; on ne l'utilise pas — l'upload navigateur ElegantOTA est
   gratuit.

## 8. Gestion des erreurs / sécurité

- **Moteurs** : coupés dès `onStart` ; tracking et commande manuelle bloqués
  tant que `otaEnCours`. En cas d'échec (`onEnd(false)`) le drapeau retombe.
- **Authentification** : `/update` protégée par `ADMIN_PIN`.
- **Robustesse flash** : image écrite sur la partition inactive ; échec/coupure
  ⇒ ancien firmware conservé. Récupération ultime toujours possible en USB.
- **Recommandation d'exploitation** : lancer une MAJ de préférence en mode
  **MANUEL** et hors alerte vent.

## 9. Vérification (plan de test manuel)

1. Build OK dans CLion (taille app < partition ~1,2 Mo).
2. Flash USB initial ; `http://<IP>/update` répond et demande l'authentification.
3. Modifier `FW_VERSION`, rebuild, upload navigateur ; après reboot la version
   affichée a changé.
4. Énergiser un moteur en mode manuel, démarrer un upload : le moteur s'arrête.
5. Rejouer le tout sur l'esclave.

## 10. Hors périmètre (YAGNI)

- OTA depuis Internet / hors réseau local (VPN, redirection de port, hébergement).
- OTA « pull » automatique depuis un serveur / GitHub Releases.
- Firmware signé/chiffré, rollback piloté depuis l'UI.
- Mise à jour simultanée de flotte.

## 11. Points d'attention

- **Premier flash en USB obligatoire** sur chaque carte.
- **IP du maître** : non fixe aujourd'hui. Pour faciliter les MAJ, envisager une
  réservation DHCP côté box (comme l'esclave en `.35`), sinon relever l'IP au
  démarrage (log « WiFi OK — http://… »).
- `FW_VERSION` doit être incrémentée à chaque build pour que la vérification
  visuelle ait du sens.
