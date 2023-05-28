
#include <BTS7960.h>

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
/*
#define borneRENA       31       // MOTEUR ROTATION EST_OUEST
#define borneLENA       30       // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneIN1A       11       // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneIN2A       10       // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneIN1B        8       // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneIN2B        9       // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneRENB       38       // On associe la borne "ENB" du L298N à la pin D11 de l'arduino
#define borneLENB       39       // MOTEUR INCLINAISON
*/

#define RpwmEO    9     // BTS1
#define LpwmEO    10
#define LenEO     11
#define RenEO     12

#define ENAIH    3     // BTS2
#define IN1IH    4
#define IN2IH    5

//TODO Potentiellement à redéfinir !!!
#define FdcIH           24       // On associe le Capteur de l'inclinaison vers l'Horizontale à la pin D4 de l'arduino
#define FdcIV           25       // On associe le Capteur de l'inclinaison vers la verticale à la pin D3 de l'arduino
#define FdcMV           26       // On associe le Capteur du moteur vers l'avant à la pin D2 de l'arduino
#define FdcMR           27       // On associe le Capteur du moteur vers l'arrière à la pin D1 de l'arduino
//TODO Potentiellement à redéfinir !!!

//TODO Potentiellement à redéfinir !!!
#define cptLumB         3      // On associe le Capteur de luminisoté situé en bas à la pin A1 de l'arduino SUD JAUNE
#define cptLumH         1      // On associe le Capteur de luminisoté situé en haut à la pin A2 de l'arduino  NORD BLANC
#define cptLumD         2      // On associe le Capteur de luminisoté situé en bas à droite au pin A3 de l'arduino EST  NOIR
#define cptLumG         4      // On associe le Capteur de luminisoté situé en bas à gauche au pin A4 de l'arduino  OUEST  MARRON
#define cptAnemo        7      // On associe le Capteur vent au pin A7 de l'arduino

//*************************//
// Constantes du programme //
//*************************//
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO);  //EST-OUEST
//BTS7960 motorB(borneLENB, borneRENB, borneIN2B, borneIN1B); //INCLINAISON
 
int captfdcIH; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMR; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captLumH;
int captLumB;
int captLumG;
int captLumD;
int captAnemo;
//TODO Potentiellement à redéfinir !!!


//************************//
// Variables du programme //
//************************//
#define vitesseMoteur   205  
#define vitMotEO        75
#define timeBoucle      1
#define seuilLum        130
#define seuilAnemo      150

const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
 
//*******//
// SETUP //
//*******//

void setup() {
 // Configuration de toutes les pins de l'Arduino en "sortie" et "entrée"
  
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  
  motorEO.begin();   // setup des pins du motor EST-OUEST
  //motorB.begin();   // setup des pins du moteur INCLINAISON
  pinMode(ENAIH, OUTPUT);
  pinMode(IN1IH, OUTPUT);
  pinMode(IN2IH, OUTPUT);
  
  motorEO.enable(); //mise en place des variables qui pouront changer les valeurs de vitesse
  
  motorEO.pwm = vitMotEO;  
  //TODO Potentiellement à redéfinir !!!


  
  pinMode(FdcIH,     INPUT);
  pinMode(FdcIV,     INPUT);
  pinMode(FdcMV,     INPUT);
  pinMode(FdcMR,     INPUT);
  pinMode(cptLumH,   INPUT);
  pinMode(cptLumD,   INPUT);
  pinMode(cptLumG,   INPUT);
  pinMode(cptLumB,   INPUT);
  pinMode(cptAnemo,  INPUT);
  
  //TODO Potentiellement à redéfinir !!!
}

//**************************//
// Boucle principale : LOOP //
//**************************//

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println ("loop");
  delay(5000UL); //5sec
  captLumH = analogRead(cptLumH);
  captLumB = analogRead(cptLumB);
  captLumG = analogRead(cptLumG);
  captLumD = analogRead(cptLumD);
  captAnemo = analogRead(cptAnemo);
  captfdcIV = digitalRead(FdcIV);
  captfdcIH = digitalRead(FdcIH);
  captfdcMV = digitalRead(FdcMV);
  captfdcMR = digitalRead(FdcMR);
  
  Serial.print("valeur Haut: ");
  Serial.println (captLumH);
  Serial.print("Valeur Bas: ");
  Serial.println (captLumB);
  Serial.print("Valeur Gauche: ");
  Serial.println (captLumG);
  Serial.print("Valeur Droite: ");
  Serial.println (captLumD);
  Serial.print("Valeur anemo: ");
  Serial.println(captAnemo);
  
  Serial.print("valeur FDC IH: ");
  Serial.println (captfdcIH);
  Serial.print("Valeur FDC IV: ");
  Serial.println (captfdcIV);
  Serial.print("Valeur FDC MV: ");
  Serial.println (captfdcMV);
  Serial.print("Valeur FDC MR: ");
  Serial.println (captfdcMR);

  while(analogRead(cptLumB)>= seuilLum || analogRead(cptLumH)>= seuilLum || analogRead(cptLumG)>=seuilLum || analogRead(cptLumD)>=seuilLum){
    
    captLumH = analogRead(cptLumH);
    captLumB = analogRead(cptLumB);
    captLumG = analogRead(cptLumG);
    captLumD = analogRead(cptLumD);
    captAnemo = analogRead(cptAnemo);
    captfdcIV = digitalRead(FdcIV);
    captfdcIH = digitalRead(FdcIH);
    captfdcMV = digitalRead(FdcMV);
    captfdcMR = digitalRead(FdcMR);
  
    Serial.print("valeur Haut: ");
    Serial.println (captLumH);
    Serial.print("Valeur Bas: ");
    Serial.println (captLumB);
    Serial.print("Valeur Gauche: ");
    Serial.println (captLumG);
    Serial.print("Valeur Droite: ");
    Serial.println (captLumD);
    Serial.print("Valeur anemo: ");
    Serial.println(captAnemo);
  
    Serial.print("valeur FDC IH: ");
    Serial.println (captfdcIH);
    Serial.print("Valeur FDC IV: ");
    Serial.println (captfdcIV);
    Serial.print("Valeur FDC MV: ");
    Serial.println (captfdcMV);
    Serial.print("Valeur FDC MR: ");
    Serial.println (captfdcMR);
    
    while(captfdcIV ==0  && captfdcIH == 0){
      configurerSensDeRotationPontMoteur('V'); // avant ou arrière // a modifier peut etre
      changeVitesseMoteurPontMoteur(vitesseMoteur);  // ou back
    }

    while(captfdcMR ==0  && captfdcMV == 0){
      configurerSensDeRotationPontMoteur('R'); // avant ou arrière // a modifier peut etre
      changeVitesseMoteurPontMoteur(vitesseMoteur);  // ou back
    }
    
    if (analogRead(cptLumD)<= analogRead(cptLumG))
    {
      Serial.println("Soleil time");
      for(byte h=0; h<timeBoucle; h++) // h = 60 represente 60 min
      { 
        delay(60000UL);  //ceci motre une pause de 60sec 
      }
      if (analogRead(cptLumD)<analogRead(cptLumG))
      {
        captfdcMR = digitalRead(FdcMR); // lecture du signal du capteur
        Serial.println ("je suis le soleil");
        while (analogRead(cptLumD)<=analogRead(cptLumG))   // problème car je veux que si un des cpateur ou la formule est vrai stop while
        {        
          if (captfdcMR == 0){
            Serial.println("capt Activé Break");
            break;
          } 
          motorEO.front(); // est vers ouest   
          captfdcMR = digitalRead(FdcMR); // lecture du signal du capteur        
        }
        motorEO.stop(); 
        Serial.println("cycle je suis le soleil terminer");          
      }
    }

    if (analogRead(cptLumD)>= analogRead(cptLumG))
    {
      Serial.println("Retour time");
      for(byte h=0; h< timeBoucle ; h++) // h = 60 represente 60 min
      { 
        delay(60000UL);  //ceci motre une pause de 30sec 
      }
      if (analogRead(cptLumD)>analogRead(cptLumG))
      {
        captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur      
        Serial.println ("je retourne un petit peu a l'EST");
        while (analogRead(cptLumD)>=analogRead(cptLumG))   // problème car je veux que si un des cpateur ou la formule est vrai stop while
        {
        
          if (captfdcMV == 0){
            Serial.println("capt Activé Break");
            break;
          } 
          motorEO.back(); // est vers ouest   
          captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur        
        }
        motorEO.stop(); 
        Serial.println("cycle retour a l'EST terminer");          
      }
    } 

    if (analogRead(cptLumH)>= analogRead(cptLumB))
    {    
      Serial.println("Horizon time");
      for(byte h=0; h<timeBoucle; h++) // h = 60 represente 60 min
      { 
        delay(60000UL);  //ceci motre une pause de 30 sec
      }
      if (analogRead(cptLumH)>analogRead(cptLumB))
      {
        captfdcIH = digitalRead(FdcIH); // lecture du signal du capteur
        Serial.println ("je me met a l'horizontale");
        while (analogRead(cptLumH)>=analogRead(cptLumB)) // ||captfdcIH == 1||captfdcIV == 1 problème car je veux que si un des cpateur ou la formule est vrai stop while
        {
          if (captfdcIH == 0){
            Serial.println("capt Activé Break");
            break;
          }
          configurerSensDeRotationPontMoteur('R'); // avant ou arrière // a modifier peut etre
          changeVitesseMoteurPontMoteur(vitesseMoteur); 
          captfdcIH = digitalRead(FdcIH); // lecture du signal du capteur             
        }   
        changeVitesseMoteurPontMoteur(0); // remise a zero de la vitesse moteur donc arrêt moteur
        Serial.println("cycle mise a l'horizontale terminer");
      }
    }

    if (analogRead(cptLumH)<= analogRead(cptLumB))
    {
      Serial.println("Vertical time");
      for(byte h=0; h<timeBoucle; h++) // h = 60 represente 60 min
      { 
        delay(60000UL);  //ceci motre une pause de 30 sec 
      }
      if (analogRead(cptLumH)<analogRead(cptLumB))
      {      
        captfdcIV = digitalRead(FdcIV); // lecture du signal du capteur
        Serial.println ("je me met a la verticale ");
        while (analogRead(cptLumH) <= analogRead(cptLumB))   // problème car je veux que si un des cpateur ou la formule est vrai stop while
        {
          if (captfdcIV == 0){
            Serial.println("capt Activé Break");
            break;
          }
          configurerSensDeRotationPontMoteur('V'); // avant ou arrière // a modifier peut etre
          changeVitesseMoteurPontMoteur(vitesseMoteur);// front ou back       
          captfdcIV = digitalRead(FdcIV); // lecture du signal du capteur               
        }   
        changeVitesseMoteurPontMoteur(0);// remise a zero de la vitesse moteur donc arrêt moteur
        Serial.println("cycle mise a la verticale terminer");
      }
    }
    changeVitesseMoteurPontMoteur(0); 
    motorEO.stop();
 }       
   
  Serial.println ("je me remet a zéro");
    
  while(captfdcMV == 1)
  {
    motorEO.back();
    captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur
  }
  motorEO.stop();
  changeVitesseMoteurPontMoteur(0);
  Serial.println("cycle retour a zéro terminer");    
}  


void configurerSensDeRotationPontMoteur(char sensDeRotation) {

  if(sensDeRotation == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(IN1IH, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(IN2IH, LOW);                  // L'entrée IN2 doit être au niveau bas    
  }
  
  if(sensDeRotation == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(IN1IH, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(IN2IH, HIGH);                 // L'entrée IN2 doit être au niveau haut
  }
}

void changeVitesseMoteurPontMoteur(int nouvelleVitesse) {
  
  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(ENAIH, nouvelleVitesse);
}
