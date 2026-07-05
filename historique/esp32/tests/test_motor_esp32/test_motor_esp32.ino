//**********************//
// Appel des Librairies //
//**********************//
#include <BTS7960.h> //biblio des ponts en H

//***********************************************************************//
// Association des entrées du BTS7960, aux sorties utilisées sur l'ESP32 //
//***********************************************************************//

#define RpwmEO    15     // Moteur Soleil
#define LpwmEO    2
#define LenEO     0
#define RenEO     4

#define RpwmIH    17     // Moteur Inclinaison
#define LpwmIH    5
#define LenIH     18
#define RenIH     19

//*************************//
// Constantes du programme //
//*************************//
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);  // EST-OUEST
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH);  // Inclinaison

#define vitMotIH  255  // Vitesse du moteur d'inclinaison
#define vitMotEO  255  // Vitesse du moteur Est-Ouest

//*******//
// SETUP //
//*******//
void setup() {
  Serial.begin(9600);

  // Initialisation des moteurs
  motorEO.begin();        // Setup des pins du moteur EST-OUEST    
  motorIH.begin();        // Setup des pins du moteur INCLINAISON 
  
  motorEO.enable();       // Activation du moteur EST-OUEST
  motorIH.enable();       // Activation du moteur INCLINAISON

  motorEO.pwm = vitMotEO; // Définition de la vitesse du moteur EST-OUEST
  motorIH.pwm = vitMotIH; // Définition de la vitesse du moteur INCLINAISON

  Serial.println("Test des moteurs");
}

//********//
// LOOP //
//********//
void loop() {
  // Test du moteur EST-OUEST
  Serial.println("Test du moteur EST-OUEST avant");
  motorEO.front();
  delay(3000); // Tourne vers l'avant pendant 2 secondes
  motorEO.stop();
  delay(1000); // Pause d'une seconde
  
  Serial.println("Test du moteur EST-OUEST arrière");
  motorEO.back();
  delay(3000); // Tourne vers l'arrière pendant 2 secondes
  motorEO.stop();
  delay(1000); // Pause d'une seconde

  // Test du moteur INCLINAISON
  Serial.println("Test du moteur INCLINAISON avant");
  motorIH.front();
  delay(2000); // Tourne vers l'avant pendant 2 secondes
  motorIH.stop();
  delay(1000); // Pause d'une seconde
  
  Serial.println("Test du moteur INCLINAISON arrière");
  motorIH.back();
  delay(2000); // Tourne vers l'arrière pendant 2 secondes
  motorIH.stop();
  delay(1000); // Pause d'une seconde

  // Fin du test
  Serial.println("Test des moteurs terminé");
  delay(5000);
}
