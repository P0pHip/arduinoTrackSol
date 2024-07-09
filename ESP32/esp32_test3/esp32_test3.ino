#include <Wire.h>
#include <BTS7960.h>
#include <BH1750.h>

// Pins pour le contrôle des BTS7960
#define RpwmEO    15   // Moteur Soleil
#define LpwmEO    2
#define LenEO     0
#define RenEO     4

#define RpwmIH    17   // Moteur Inclinaison
#define LpwmIH    5
#define LenIH     18
#define RenIH     19

#define FdcIH           25 //Nord   
#define FdcIV           26 //Sud    
#define FdcEst          27    
#define FdcOuest        14     

// Pins pour le multiplexeur TCA9548A
#define TCA9548A_ADDR 0x70

//*************************//
// Constantes du programme //
//*************************//

// Déclaration des moteurs BTS7960
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO); // EST-OUEST
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH); // Inclinaison

// Déclaration des capteurs BH1750
BH1750 nord;
BH1750 sud;
BH1750 est;
BH1750 ouest;

float luxNord;
float luxSud; 
float luxEst;
float luxOuest;

int captfdcIH; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcES; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcOU; // declaration de la variable 0 ou 1 soit ouvert ou fermée

//************************//
// Variables du programme //
//************************//
#define vitMotIH        255  // a toi de voir
#define vitMotEO        255
#define timeBoucle      1 // nombre de paquet de 30 sec
#define seuilLum        300


void tcaSelect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println( "test du setup" );

  // Initialisation des moteurs BTS7960
  motorEO.begin();
  motorIH.begin();

  motorEO.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse
  motorIH.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse

  motorEO.pwm = vitMotEO;  // mise en place de la vitesse des moteurs
  motorIH.pwm = vitMotIH;
  
  pinMode(FdcIH,    INPUT_PULLUP);
  pinMode(FdcIV,    INPUT_PULLUP);
  pinMode(FdcEst,   INPUT_PULLUP);
  pinMode(FdcOuest, INPUT_PULLUP); 

  // Initialisation des capteurs BH1750 sur le multiplexeur TCA9548A
  tcaSelect(0); // Canal pour le capteur nord
  nord.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  tcaSelect(1); // Canal pour le capteur sud
  sud.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  tcaSelect(2); // Can+al pour le capteur est
  est.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  tcaSelect(3); // Canal pour le capteur ouest
  ouest.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // Autres initialisations de pins si nécessaire
}

void loop() {
  delay(10000);

  //Recupération des valeurs des capteurs lum
  getValCpt();

  //Recuperation des valeurs des fins de courses
  getValFdc();
  
  // Exemple de logique de suivi solaire
  if (luxSud >= seuilLum || luxNord >= seuilLum || luxEst >= seuilLum || luxOuest >= seuilLum) {
    // Logique pour ajuster les moteurs en fonction des capteurs de luminosité
    // Ici, vous pouvez implémenter votre algorithme pour le suivi solaire
    // Utilisez les fonctions des moteurs BTS7960 (motorEO et motorIH) pour déplacer les moteurs en conséquence
    // Assurez-vous de prendre en compte les capteurs de fin de course (FDC) pour la sécurité
    delay(10000);
    // Exemple d'utilisation :
    Serial.println("MotorSun step");
    trackSun(motorEO, captfdcES, captfdcOU, luxEst, luxOuest);

    delay(10000);
    Serial.println("MotorInclinaison Step");
    trackSun(motorIH, captfdcIH, captfdcIV, luxNord, luxSud);
  }else{
    resetTrackSun();
  }

  //resetTrackSun();
    
  // D'autres actions ou logiques peuvent être placées ici en fonction de vos besoins
}

// Fonction de suivi solaire
void trackSun(BTS7960 &motor, int &fdc1, int &fdc2, float &lux1, float &lux2) {
  // Votre algorithme de suivi solaire ici
  // Utilisez les paramètres pour déterminer la direction et l'action des moteurs
  // Assurez-vous de gérer les capteurs de fin de course (FDC) pour éviter les collisions ou les dépassements
  // Exemple simplifié :
  
  if (lux1 > lux2) {
    Serial.println("Retour time / Horizon time");    
    while (fdc1 == HIGH) {
      // Attente ou autre logique
      if(lux1 <= lux2){
        motor.stop();
        break;
      }
      motor.back(); // Avancer dans la direction lux1 > lux2
      getValCpt();
      getValFdc();
    }
    motor.stop();
    
  } else {
    Serial.println("Sun time / Vertical time");  
    while (fdc2 == HIGH) {
      // Attente ou autre logique
      if(lux1 >= lux2){
        motor.stop();
        break;
      }
      motor.front(); // Reculer dans la direction lux1 <= lux2
      getValCpt();
      getValFdc();
    }
    motor.stop();    
  }
  Serial.println("End Step");
}

void getValCpt(){
  tcaSelect(0); // Capteur nord
  luxNord = nord.readLightLevel();
  Serial.print("LuxNord: ");
  Serial.println(luxNord);

  tcaSelect(1); // Capteur sud
  luxSud = sud.readLightLevel();
  Serial.print("LuxSud: ");
  Serial.println(luxSud);
  
  tcaSelect(2); // Capteur est
  luxEst = est.readLightLevel();
  Serial.print("LuxEst: ");
  Serial.println(luxEst);
  
  tcaSelect(3); // Capteur ouest
  luxOuest = ouest.readLightLevel();
  Serial.print("LuxOuest: ");
  Serial.println(luxOuest);
}

void getValFdc(){
  
  captfdcIH = digitalRead(FdcIH);
  Serial.print("valeur FDC IH: ");
  Serial.println (captfdcIH);

  captfdcIV = digitalRead(FdcIV);
  Serial.print("Valeur FDC IV: ");
  Serial.println (captfdcIV);

  captfdcES = digitalRead(FdcEst);
  Serial.print("Valeur FDC du moteur vers Est: ");
  Serial.println (captfdcES);

  captfdcOU = digitalRead(FdcOuest);
  Serial.print("Valeur FDC du moteur vers l'ouest: ");
  Serial.println (captfdcOU);
  
}

void stopAllMotors() {
  motorEO.stop();
  motorIH.stop();
}

void resetTrackSun() {
  Serial.println("je me remet a zéro");  
  
  // Move motorIH back until FdcIH is LOW
  Serial.println("mise a plat");
  while (digitalRead(FdcIH) == HIGH) {
    motorIH.back();
  }  
  motorIH.stop();
  delay(2000);

  // Move motorEO back until FdcEst is LOW
  Serial.println("retour à l'est");
  while (digitalRead(FdcEst) == HIGH) {
    motorEO.back();
  }
  motorEO.stop();
  delay(2000);
  
  // Ensure all motors are stopped
  stopAllMotors();
  
  Serial.println("cycle retour a zéro terminer");
}
