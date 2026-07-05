#include <BTS7960.h>

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
#define RpwmEO    9     // Moteur Soleil
#define LpwmEO    10
#define LenEO     11
#define RenEO     12

#define RpwmIH    2     // Moteur Inclinaison
#define LpwmIH    3
#define LenIH     4
#define RenIH     5

//*************************//
// Constantes du programme //
//*************************//
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);  //EST-OUEST
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH);  //Inclinaison

//************************//
// Variables du programme //
//************************//
#define vitMotIH        205  // a toi de voir
#define vitMotEO        90

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  motorEO.begin();        // setup des pins du motor EST-OUEST    
  motorIH.begin();        // setup des pins du moteur INCLINAISON 
  
  motorEO.enable();       //mise en place des variables qui pouront changer les valeurs de vitesse
  motorIH.enable();

  motorEO.pwm = vitMotEO;  //mise en place de la vitesse des moteurs
  motorIH.pwm = vitMotIH;

  
}

void loop() {

  Serial.println("initialisation");
  Serial.println("Moteur soleil");
  delay(5000UL); //5sec
  Serial.println("sensAvant moteur");
  motorEO.front();     
  delay(5000UL); //5sec
  motorEO.stop();
  delay(2000UL); // 2sec
  Serial.println("sensArrière moteur");
  motorEO.back();
  delay(5000UL); //5sec
  motorEO.stop();
  delay(2000UL); // 2sec

  
  Serial.println("Moteur INCLINAISON");
  delay(3000UL); //5sec
  Serial.println("sensAvant moteur");
  motorIH.front();
  delay(5000UL); //5sec
  motorIH.stop(); 
  delay(2000UL); //2sec
  Serial.println("sensArrière moteur");
  motorIH.back();
  delay(5000UL); //5sec
  motorIH.stop(); 
  delay(2000UL); //2sec 
 
}
