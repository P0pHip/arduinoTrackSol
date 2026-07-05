# arduinoTrackSol

Trackeur solaire à 2 axes (Est-Ouest + inclinaison) piloté par ESP32.

## Projet actif : `track_sol_esp32_bts7960_bh1750_mobile/`

Le tracker oriente 4 capteurs de luminosité BH1750 (Nord/Sud/Est/Ouest, via
un multiplexeur I²C TCA9548A) vers le soleil en pilotant deux moteurs
(axe Est-Ouest et axe d'inclinaison) via des drivers BTS7960, avec fins de
course sur chaque axe.

Le système fonctionne en **maître/esclave** sur deux ESP32 : seul le maître
porte un capteur de vent, et notifie l'esclave par HTTP pour mettre les deux
trackers en position de sécurité (à plat) en cas de vent fort, jusqu'à
stabilisation.

### Fonctionnalités

- Tracking automatique basé sur la luminosité (mode AUTO), avec bascule
  manuelle depuis l'interface web (mode MANUEL, arrêt de sécurité si plus de
  commande reçue)
- Mise en sécurité vent (maître) synchronisée sur l'esclave
- Interface web embarquée (contrôle, réglages, journal, code PIN admin)
- Réglages persistés en mémoire flash (`Preferences`)
- Mise à jour du firmware par OTA (Wi-Fi) via ElegantOTA, avec coupure des
  moteurs pendant la MAJ

### Bibliothèques requises (Gestionnaire de bibliothèques Arduino IDE)

- [BTS7960](https://github.com/1337encrypted/BTS7960_Motordriver) — driver moteur
- [BH1750](https://github.com/claws/BH1750) — capteur de luminosité
- [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA) — mise à jour OTA (mode synchrone)
- Inclus dans le core ESP32 : `WiFi`, `WebServer`, `HTTPClient`, `Preferences`, `Wire`

### Mise en route

1. Dans `track_sol_esp32_bts7960_bh1750_mobile/`, copier `config.example.h` →
   `config.h`, `config_master.example.h` → `config_master.h` et
   `config_slave.example.h` → `config_slave.h`.
2. Renseigner le Wi-Fi (`WIFI_SSID`, `WIFI_PASSWORD`) et les pins dans
   `config_master.h`/`config_slave.h`.
3. Dans `config.h`, inclure `config_slave.h` et flasher l'ESP32 esclave en
   premier ; noter son IP (moniteur série).
4. Renseigner cette IP dans `SLAVE_IP` (`config_master.h`), basculer
   `config.h` sur `config_master.h`, puis flasher le maître.
5. Les MAJ suivantes peuvent se faire par OTA (voir `docs/tutoriel-maj-ota-clion.md`).

Voir les commentaires en tête de
`track_sol_esp32_bts7960_bh1750_mobile.ino` pour le détail de chaque module.

## Structure du dépôt

- `track_sol_esp32_bts7960_bh1750_mobile/` — projet actif (ESP32, BTS7960, BH1750, OTA)
- `test/Esp32/` — tests en cours liés au projet actif
- `docs/` — documentation et specs du projet actif
- `historique/` — anciennes versions et expérimentations (voir ci-dessous)

### historique/

- `arduino_mega/` — versions pour carte Arduino ATmega2560
  - `track_sol_BTS7960/`, `track_sol_BTS7960_V2/` — versions successives
  - `variantes/` — variantes explorées (L298N, V_UNO, ...)
  - `tests/` — anciens sketchs de test
- `esp32/` — versions et essais ESP32 antérieurs au projet actif
  - `track_sol_ESP32_BTS7960_BH1750/` — première version ESP32
  - `experimentations/` — essais divers
  - `tests/` — anciens sketchs de test