/*
 * Test capteur anémomètre sur le pin 34 (ADC1)
 * 1 tour/s = 2.4 km/h  —  capteur reed : impulsion LOW à chaque tour
 * Ouvrir le Moniteur Série à 115200 bauds
 */

#define WIND_PIN        34
#define WIND_SAMPLE_MS  1000   // fenêtre de mesure (ms)
#define WIND_THRESHOLD  40     // seuil alerte (km/h)

int mesurerVitesse() {
  int impulsions = 0;
  unsigned long fin = millis() + WIND_SAMPLE_MS;
  bool dernierEtatBas = false;

  while (millis() < fin) {
    bool bas = (analogRead(WIND_PIN) < 100);
    if (bas && !dernierEtatBas) impulsions++;
    dernierEtatBas = bas;
    delay(5);
  }
  return (int)(impulsions * 2.4f * (1000.0f / WIND_SAMPLE_MS));
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Test anemometre (pin 34) ===");
  Serial.println("Seuil alerte : " + String(WIND_THRESHOLD) + " km/h");
  Serial.println("--------------------------------");
}

void loop() {
  int kmh = mesurerVitesse();
  String ligne = String(kmh) + " km/h";
  if (kmh >= WIND_THRESHOLD) ligne += "  <<< ALERTE VENT";
  Serial.println(ligne);
}
