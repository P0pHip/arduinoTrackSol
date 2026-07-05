# arduinoTrackSol
trackeur solaire arduino

Pour la librairie du BTS7960 j'utilise celle de Yash Herekar: 
https://github.com/1337encrypted/BTS7960_Motordriver

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
