//**********************//
// Appel des Librairies //
//**********************//
#include <BTS7960.h> 	// https://github.com/1337encrypted/BTS7960_Motordriver

//*********************************************************************************//
// Association des entrées du BTS7960, aux sorties utilisées sur notre Arduino Uno //
//*********************************************************************************//

#define RpwmEO    9     // Moteur Soleil
#define LpwmEO    10
#define LenEO     11
#define RenEO     12

#define RpwmIH    2     // Moteur Inclinaison
#define LpwmIH    3
#define LenIH     4
#define RenIH     5

#define FdcIH           24     // On associe le Capteur de l'inclinaison vers l'Horizontale à la pin D24 de l'arduino
#define FdcIV           25     // On associe le Capteur de l'inclinaison vers la verticale à la pin D25 de l'arduino
#define FdcEst          26     
#define FdcOuest        27     

#define sud         A3      // On associe le Capteur de luminisoté situé en bas à la pin A3 de l'arduino SUD JAUNE
#define nord        A1      // On associe le Capteur de luminisoté situé en haut à la pin A2 de l'arduino  NORD BLANC
#define ouest       A2      // On associe le Capteur de luminisoté situé en bas à droite au pin A2 de l'arduino EST  NOIR
#define est         A4      // On associe le Capteur de luminisoté situé en bas à gauche au pin A4 de l'arduino  OUEST  MARRON
// #define cptAnemo    A7      // On associe le Capteur vent au pin A7 de l'arduino

//*************************//
// Constantes du programme //
//*************************//
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);  // EST-OUEST
BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH);  // Inclinaison

int captfdcIH; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcES; // declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcOU; // declaration de la variable 0 ou 1 soit ouvert ou fermée

int luxNord;
int luxSud; 
int luxEst;
int luxOuest;

// int captAnemo; // valeur entre 0 et 1023

//************************//
// Variables du programme //
//************************//
#define vitMotIH        255  // a toi de voir
#define vitMotEO        80
#define timeBoucle      1
#define seuilLum        200
#define seuilAnemo      150

//*******//
// SETUP //
//*******//

void setup() {
  // Configuration de toutes les pins de l'Arduino en "sortie" et "entrée"
  
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  
  motorEO.begin();        // setup des pins du motor EST-OUEST    
  motorIH.begin();        // setup des pins du moteur INCLINAISON 
  
  motorEO.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse
  motorIH.enable();       // mise en place des variables qui pouront changer les valeurs de vitesse

  motorEO.pwm = vitMotEO;  // mise en place de la vitesse des moteurs
  motorIH.pwm = vitMotIH;

  pinMode(FdcIH, 	INPUT);
  pinMode(FdcIV, 	INPUT);
  pinMode(FdcEst, 	INPUT);
  pinMode(FdcOuest, INPUT);   
 
}

//**************************//
// Boucle principale : LOOP //
//**************************//

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
    //resetTrackSun();
    overhaul();
    eastReturn();
    stopAllMotors();
  } 
    
}

// Fonction de suivi solaire
void trackSun(BTS7960 &motor, int &fdc1, int &fdc2, int &lux1, int &lux2) {

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
  
    luxNord = analogRead(nord);
    luxSud = analogRead(sud); //lecture des valeurs
    luxEst = analogRead(est);
    luxOuest = analogRead(ouest);
    // captAnemo = analogRead(cptAnemo);
	  Serial.print("valeur Haut: ");
    Serial.println (luxNord);      //affichage des valeurs
    Serial.print("Valeur Bas: ");
    Serial.println (luxSud);
    Serial.print("Valeur Gauche: ");
    Serial.println (luxEst);
    Serial.print("Valeur Droite: ");
    Serial.println (luxOuest);
    // Serial.print("Valeur anemo: ");
    // Serial.println(captAnemo);
}

void getValFdc(){
  
  	captfdcIV = digitalRead(FdcIV);
    captfdcIH = digitalRead(FdcIH);
    captfdcES = digitalRead(FdcEst);
    captfdcOU = digitalRead(FdcOuest);
	  Serial.print("valeur FDC IH: ");
  	Serial.println (captfdcIH);
  	Serial.print("Valeur FDC IV: ");
  	Serial.println (captfdcIV);
  	Serial.print("Valeur FDC MV: ");
  	Serial.println (captfdcES);
  	Serial.print("Valeur FDC MR: ");
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

void overhaul() {
  Serial.println("mise a plat");
  while (digitalRead(FdcIH) == HIGH) {
    motorIH.back();
    delay(100);
  }  
  motorIH.stop();
  delay(2000);
}

void eastReturn(){
  Serial.println("retour à l'est");
  while (digitalRead(FdcEst) == HIGH) {
    motorEO.back();
    delay(100);
  }
  motorEO.stop();
  delay(2000);
}
