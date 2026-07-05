void setup() {
  Serial.begin(115200); // Configurez la vitesse de communication série à 115200 bauds
  Serial.println("ESP32 ready"); // Envoyer un message de confirmation à la console série
}

void loop() {
  Serial.println("Hello, World!"); // Envoyer un message de test à la console série
  delay(1000); // Attendre 1 seconde avant de répéter
}
