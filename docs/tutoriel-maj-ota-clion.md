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
4. **Build** (bouton Build du plugin, ou la commande ci-dessous qui compile
   puis ne garde que le `.bin` utile à l'OTA, en supprimant le bootloader,
   les partitions, l'image fusionnée, et les fichiers de debug `.elf`/`.map`) :

   En **PowerShell** (Windows) :
   ```powershell
   arduino-cli compile --fqbn esp32:esp32:esp32da --output-dir build --verbose track_sol_esp32_bts7960_bh1750_mobile
   if ($?) { Remove-Item build\*.bootloader.bin, build\*.partitions.bin, build\*.merged.bin, build\*.elf, build\*.map -ErrorAction SilentlyContinue }
   ```

   En **Bash** (Git Bash / WSL / macOS / Linux) :
   ```bash
   arduino-cli compile --fqbn esp32:esp32:esp32da --output-dir build --verbose track_sol_esp32_bts7960_bh1750_mobile \
     && rm -f build/*.bootloader.bin build/*.partitions.bin build/*.merged.bin build/*.elf build/*.map
   ```

   (`--verbose`/`-v` affiche le détail des commandes de compilation — utile en
   cas d'erreur pour voir précisément où ça bloque.)
   Il ne reste alors dans `build/` que
   **`track_sol_esp32_bts7960_bh1750_mobile.ino.bin`** — c'est le firmware
   applicatif, le seul fichier nécessaire pour l'OTA. Les fichiers supprimés
   (`....bootloader.bin`, `....partitions.bin`, `....merged.bin`) ne servent
   que pour un flash USB complet à blanc.
5. **Uploader via le navigateur** :
   - Ouvrir `http://<IP_DE_LA_CARTE>/update`.
   - S'authentifier : identifiant `admin`, mot de passe = votre `ADMIN_PIN`.
   - Sélectionner **`track_sol_esp32_bts7960_bh1750_mobile.ino.bin`**, type
     **Firmware** → l'upload démarre, barre de progression, puis redémarrage
     automatique.
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