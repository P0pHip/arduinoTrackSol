//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
#define borneRENA       31       // MOTEUR ROTATION EST_OUEST
#define borneLENA       30       // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneIN1A       11       // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneIN2A       10       // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneIN1B        8       // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneIN2B        9       // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneRENB       38       // On associe la borne "ENB" du L298N à la pin D11 de l'arduino
#define borneLENB       39       // MOTEUR INCLINAISON

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
#define cptAnemo        7      // On associe le Capteur vent au pin A5 de l'arduino

//*************************//
// Constantes du programme //
//*************************//
#define vitesseMoteur   255 
const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
 
int captfdcIH; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMR; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captLumH;
int captLumB;
int captLumG;
int captLumD;
//TODO Potentiellement à redéfinir !!!


//************************//
// Variables du programme //
//************************//
int vitesse;              // Spécifie la vitesse de rotation du moteur, entre son minimum (0) et son maximum (255) <= signal PWM

//*******//
// SETUP //
//*******//

void setup() {
 // Configuration de toutes les pins de l'Arduino en "sortie" et "entrée"
  
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  

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
  captLumH = analogRead(cptLumH);
  captLumB = analogRead(cptLumB);
  captLumG = analogRead(cptLumG);
  captLumD = analogRead(cptLumD);
  Serial.println (captLumH);
  Serial.println (captLumB);
  Serial.println (captLumG);
  Serial.println (captLumD);
  if (analogRead(cptLumH)<analogRead(cptLumB))
  {
    
    Serial.println("premier time");
    for(byte h=0; h<1; h++) // h = 60 represente 60 min
    { 
      delay(30000UL);  //ceci motre une pause de 1 min 
    }
    if (analogRead(cptLumH)<analogRead(cptLumB))
    {
      captfdcIH = digitalRead(FdcIH); // lecture du signal du capteur
      Serial.println ("je me met a l'horizontale");
      while (!(analogRead(cptLumH) >= analogRead(cptLumB))||captfdcIH == 1||captfdcIV == 1)
      {
        
        configurerSensDeRotationPontINCLINAISON('V'); // avant ou arrière // a modifier peut etre
        changeVitesseMoteurPontINCLINAISON(vitesseMoteur);        
        captfdcIH = digitalRead(FdcIH); // lecture du signal du capteur
        captfdcIV = digitalRead(FdcIV); // lecture du signal du capteur
      }   
     changeVitesseMoteurPontINCLINAISON(0); // remise a zero de la vitesse moteur donc arrêt moteur
    }
  }

  if (analogRead(cptLumH)>analogRead(cptLumB))
  {
    Serial.println("deuxième time");
    for(byte h=0; h<1; h++) // h = 60 represente 60 min
    { 
      delay(30000UL);  //ceci motre une pause de 1 min 
    }
    if (analogRead(cptLumH)>analogRead(cptLumB))
    {
      captfdcIV = digitalRead(FdcIV); // lecture du signal du capteur
      Serial.println ("je me met a la verticale ");
      while (!(analogRead(cptLumH) <= analogRead(cptLumB))||captfdcIH == 1||captfdcIV == 1)
      {
        
        configurerSensDeRotationPontINCLINAISON('R'); // avant ou arrière // a modifier peut etre
        changeVitesseMoteurPontINCLINAISON(vitesseMoteur);        
        captfdcIH = digitalRead(FdcIH); // lecture du signal du capteur
        captfdcIV = digitalRead(FdcIV); // lecture du signal du capteur
      }   
     changeVitesseMoteurPontINCLINAISON(0); // remise a zero de la vitesse moteur donc arrêt moteur
    }
  }


  if (analogRead(cptLumD)<analogRead(cptLumG))
  {
    Serial.println("3 time");
    for(byte h=0; h<1; h++) // h = 60 represente 60 min
    { 
      delay(30000UL);  //ceci motre une pause de 1 min 
    }
    if (analogRead(cptLumD)<analogRead(cptLumG))
    {
      captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur
      Serial.println ("je suis le soleil");
      while (!(analogRead(cptLumD) >= analogRead(cptLumG))||captfdcMV == 1)
      {
        
        configurerSensDeRotationPontMOTEUR('V'); // 'V' avant ou 'R' arrière // à modifier peut etre
        changeVitesseMoteurPontMOTEUR(vitesseMoteur);        
        captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur
      }
      changeVitesseMoteurPontMOTEUR(0);           
    }
  }

  if (captfdcMV == 1)
  {
    Serial.println ("je me remet a zéro");
    while(captfdcMR == 0)
    {
      
      configurerSensDeRotationPontMOTEUR('R'); // 'V' avant ou 'R' arrière // à modifier peut etre
      changeVitesseMoteurPontMOTEUR(vitesseMoteur);        
      captfdcMV = digitalRead(FdcMV); // lecture du signal du capteur
    }
    if(captfdcMR == 1)
    {
        changeVitesseMoteurPontMOTEUR(0);
    }
  }

  /*
  if (captfdcM1 == 1)
  {
    while(captfdcM2 == 0)
    {
      Serial.print ("je me remet a zéro");
      configurerSensDeRotationPontINCLINAISON('R'); // 'V' avant ou 'R' arrière // à modifier peut etre
      changeVitesseMoteurPontINCLINAISON(vitesseMoteur);        
      captfdcM1 = digitalRead(FdcM1); // lecture du signal du capteur
    }
    if(captfdcM2 == 1)
    {
        changeVitesseMoteurPontINCLINAISON(0);
    }
  }
  */

}


void configurerSensDeRotationPontINCLINAISON(char sensDeRotation) {

  if(sensDeRotation == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN3, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneIN4, LOW);                  // L'entrée IN2 doit être au niveau bas    
  }
  
  if(sensDeRotation == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN3, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneIN4, HIGH);                 // L'entrée IN2 doit être au niveau haut
  }
}


void configurerSensDeRotationPontMOTEUR(char sensDeRotation) {

  if(sensDeRotation == MARCHE_AVANT) {
    // Configuration du L298N en "marche avant", pour le moteur connecté au pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, HIGH);                 // L'entrée IN1 doit être au niveau haut
    digitalWrite(borneIN2, LOW);                  // L'entrée IN2 doit être au niveau bas    
  }
  
  if(sensDeRotation == MARCHE_ARRIERE) {
    // Configuration du L298N en "marche arrière", pour le moteur câblé sur le pont A. Selon sa table de vérité, il faut que :
    digitalWrite(borneIN1, LOW);                  // L'entrée IN1 doit être au niveau bas
    digitalWrite(borneIN2, HIGH);                 // L'entrée IN2 doit être au niveau haut
  }
}

void changeVitesseMoteurPontINCLINAISON(int nouvelleVitesse) {
  
  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(borneENB, nouvelleVitesse);
}


void changeVitesseMoteurPontMOTEUR(int nouvelleVitesse) {
  
  // Génère un signal PWM permanent, de rapport cyclique égal à "nouvelleVitesse" (valeur comprise entre 0 et 255)
  analogWrite(borneENA, nouvelleVitesse);
}
