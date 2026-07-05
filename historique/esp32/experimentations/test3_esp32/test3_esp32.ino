//**********************//
// Appel des Librairies //
//**********************//
#include <Wire.h> // protocole i2c

//***********************************************************************//
// Association des entrées des fins de course, aux sorties utilisées sur l'ESP32 //
//***********************************************************************//
#define FdcIH           34     // Capteur de l'inclinaison vers l'Horizontale
#define FdcIV           35     // Capteur de l'inclinaison vers la verticale
#define FdcEst          32     // Capteur du moteur vers l'avant
#define FdcOuest        33     // Capteur du moteur vers l'arrière

void setup() {
  Serial.begin(9600);

  // Initialisation des pins des fins de courses comme entrées avec pull-up interne
  pinMode(FdcIH,    INPUT_PULLUP);
  pinMode(FdcIV,    INPUT_PULLUP);
  pinMode(FdcEst,   INPUT_PULLUP);
  pinMode(FdcOuest, INPUT_PULLUP);

  Serial.println("Début du test des fins de course");
}

void loop() {
  // Lecture des états des fins de courses
  int stateFdcIH = digitalRead(FdcIH);
  int stateFdcIV = digitalRead(FdcIV);
  int stateFdcEst = digitalRead(FdcEst);
  int stateFdcOuest = digitalRead(FdcOuest);

  // Affichage des états dans le moniteur série
  Serial.print("État FdcIH (Horizontal): ");
  Serial.println(stateFdcIH);
  
  Serial.print("État FdcIV (Vertical): ");
  Serial.println(stateFdcIV);
  
  Serial.print("État FdcEst (Avant): ");
  Serial.println(stateFdcEst);
  
  Serial.print("État FdcOuest (Arrière): ");
  Serial.println(stateFdcOuest);

  // Pause de 1 seconde avant la prochaine lecture
  delay(1000);
}
