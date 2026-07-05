//**********************//
// Appel des Librairies //
//**********************//
#include <Wire.h> // protocole i2c

//***********************************************************************//
// Association des entrées des fins de course, aux sorties utilisées sur l'ESP32 //
//***********************************************************************//
#define FdcIH           25     // Capteur de l'inclinaison vers l'Horizontale
#define FdcIV           26     // Capteur de l'inclinaison vers la verticale
#define FdcEst          27     // Capteur du moteur vers l'avant
#define FdcOuest        14     // Capteur du moteur vers l'arrière

void setup() {
  
  Serial.begin(9600);
  Serial.print("setup");
  // Initialisation des pins des fins de courses comme entrées
  pinMode(FdcIH,    INPUT);
  pinMode(FdcIV,  INPUT);
  pinMode(FdcEst,   INPUT);
  pinMode(FdcOuest, INPUT);
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
  delay(3000);
}
