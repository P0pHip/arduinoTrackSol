//#include <Arduino.h>

// Liste des pins à tester (exclut les pins réservés ou entrée seule comme 6-11, 34-39)
int pins[] = {2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
int numPins = sizeof(pins) / sizeof(pins[0]);

void setup() {
  Serial.begin(115200); // Initialise le moniteur série
  delay(1000);          // Petite attente pour la connexion
  Serial.println("Début du stress test des pins de l'ESP32");
}

void loop() {
  // 1. Test des pins en sortie (OUTPUT)
  Serial.println("Test des pins en sortie...");
  for (int i = 0; i < numPins; i++) {
    int pin = pins[i];
    pinMode(pin, OUTPUT);       // Configure le pin en sortie
    digitalWrite(pin, HIGH);    // Met le pin à HIGH (3.3V)
    delay(500);                 // Attend 500ms
    digitalWrite(pin, LOW);     // Met le pin à LOW (0V)
    delay(500);                 // Attend 500ms
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.println(" testé en sortie (HIGH puis LOW)");
  }

  // 2. Test des pins en entrée avec pull-up (INPUT_PULLUP)
  Serial.println("Test des pins en entrée avec pull-up...");
  for (int i = 0; i < numPins; i++) {
    int pin = pins[i];
    pinMode(pin, INPUT_PULLUP); // Configure le pin en entrée avec pull-up interne
    delay(100);                 // Petite attente pour stabiliser
    int value = digitalRead(pin); // Lit l'état du pin
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(" : ");
    Serial.println(value);      // Affiche 1 (HIGH) si rien n'est connecté, 0 (LOW) si connecté à GND
  }

  // Fin du cycle
  Serial.println("Test terminé. Reprise dans 5 secondes...");
  delay(5000); // Pause avant de recommencer
}
