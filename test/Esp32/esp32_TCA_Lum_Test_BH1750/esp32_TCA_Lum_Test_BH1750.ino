#include <Wire.h>
#include <BH1750.h>

// Adresse du TCA9548A
#define TCA9548A_ADDR 0x70

BH1750 lightMeter;

void tcaSelect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  Serial.println("TCA9548A et Capteur de Lumière BH1750 Test");

  Wire.begin();

  tcaSelect(0); // Sélectionner la sortie 0 du TCA9548A

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Capteur de lumière BH1750 prêt");
  } else {
    Serial.println("Erreur lors de l'initialisation du capteur BH1750. Vérifiez vos connexions.");
    while (1);
  }
}

void loop() {
  tcaSelect(1); // Sélectionner de nouveau la sortie 0

  float lux = lightMeter.readLightLevel();
  Serial.print("Luminosité: ");
  Serial.print(lux);
  Serial.println(" lux");

  delay(1000);
}
