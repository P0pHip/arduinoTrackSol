//**********************//
// Appel des Librairies //
//**********************//
#include <BTS7960.h> //biblio des ponts en H

#include <Wire.h> // protocole i2c
#include <BH1750.h> // biblio pour la gestion des capteur de lumonisité

// Adresse du TCA9548A
#define TCA9548A_ADDR 0x70  // carte qui permet l'utilisation des capteurs de lumières

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

#define FdcIH           34     // On associe le Capteur de l'inclinaison vers l'Horizontale à la pin D24 de l'arduino
#define FdcIV           35     // On associe le Capteur de l'inclinaison vers la verticale à la pin D25 de l'arduino
#define FdcEst          32     // On associe le Capteur du moteur vers l'avant à la pin D26 de l'arduino
#define FdcOuest        33     // On associe le Capteur du moteur vers l'arrière à la pin D27 de l'arduino

BH1750 nord;  // num 0 sur TCA
BH1750 sud;   // num 1 sur TCA
BH1750 est;   // num 2 sur TCA
BH1750 ouest; // num 3 sur TCA

//*************************//
// Constantes du programme //
//*************************//
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);  // EST-OUEST
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH);  // Inclinaison

int captfdcIH; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcES; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcOU; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captLumH;  // valeur entre 0 et 1023
int captLumB;  // valeur entre 0 et 1023
int captLumG;  // valeur entre 0 et 1023
int captLumD;  // valeur entre 0 et 1023
// int captAnemo; // valeur entre 0 et 1023

//************************//
// Variables du programme //
//************************//
#define vitMotIH        255  // a toi de voir
#define vitMotEO        255
#define timeBoucle      1 // nombre de paquet de 30 sec
#define seuilLum        200


//***********************//
// Fonction du Programme //
//***********************//

void tcaSelect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void tcaSetup(){
  for(int i =0; i<4; i++){
    tcaSelect(i);
    switch (i){
      case 0:
        if (nord.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
          Serial.println("Capteur de lumière BH1750 nord prêt");
        } else {
          Serial.println("Erreur lors de l'initialisation du capteur BH1750 nord. Vérifiez vos connexions.");
          while (1);
        }
        break;
        
      case 1:
        if (sud.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
          Serial.println("Capteur de lumière BH1750 sud prêt");
        } else {
          Serial.println("Erreur lors de l'initialisation du capteur BH1750 sud. Vérifiez vos connexions.");
          while (1);
        }
        break;

      case 2:
        if (est.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
          Serial.println("Capteur de lumière BH1750 est prêt");
        } else {
          Serial.println("Erreur lors de l'initialisation du capteur BH1750 est. Vérifiez vos connexions.");
          while (1);
        }
        break;

      case 3:
        if (ouest.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
          Serial.println("Capteur de lumière BH1750 ouest prêt");
        } else {
          Serial.println("Erreur lors de l'initialisation du capteur BH1750 ouest. Vérifiez vos connexions.");
          while (1);
        }
        break;
        
    }
  }
}

void DirectionEst(){
  
}



void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  tcaSetup();
  
  motorEO.begin();        // setup des pins du motor EST-OUEST    
  motorIH.begin();        // setup des pins du moteur INCLINAISON 
  
  motorEO.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse
  motorIH.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse

  motorEO.pwm = vitMotEO;  // mise en place de la vitesse des moteurs
  motorIH.pwm = vitMotIH;
  
  pinMode(FdcIH,    INPUT);
  pinMode(FdcIV,    INPUT);
  pinMode(FdcEst,   INPUT);
  pinMode(FdcOuest, INPUT); 
  
}
