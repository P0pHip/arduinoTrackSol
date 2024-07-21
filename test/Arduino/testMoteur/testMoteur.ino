#include <BTS7960.h>

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
#define RpwmEO    9     // BTS1
#define LpwmEO    10
#define LenEO     11
#define RenEO     12

#define ENAIH    3     // BTS2
#define IN1IH    4
#define IN2IH    5


#define vitesseMoteur   511 
#define vitMotEO        90
const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
 
BTS7960 motorEO(LenEO, RenEO, LpwmEO, RpwmEO); 
//BTS7960 motorIH(LenIH, RenIH, LpwmIH, RpwmIH); 



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  motorEO.begin();

  pinMode(ENAIH, OUTPUT);
  pinMode(IN1IH, OUTPUT);
  pinMode(IN2IH, OUTPUT);
  
  //motorIH.begin();

  motorEO.enable();
  //motorIH.enable();

  motorEO.pwm = vitMotEO ;
  //motorIH.pwm = vitesseMoteur ;

  
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
  delay(1000UL); //5sec
  configurerSensDeRotationPontMoteur('V'); // avant ou arrière // a modifier peut etre
  changeVitesseMoteurPontMoteur(vitesseMoteur); 
  delay(5000UL); //5sec
  changeVitesseMoteurPontMoteur(0); 
  delay(2000UL); //2sec
  Serial.println("sensArrière moteur");
  configurerSensDeRotationPontMoteur('R'); // avant ou arrière // a modifier peut etre
  changeVitesseMoteurPontMoteur(vitesseMoteur); 
  delay(5000UL); //5sec
  changeVitesseMoteurPontMoteur(0); 
  delay(2000UL); //2sec
  
  /*
  Serial.println("Moteur INCLINAISON");
  delay(1000UL); //5sec
  Serial.println("sensAvant moteur");
  motorIH.front();       
  delay(5000UL); //5sec
  motorIH.stop();
  delay(2000UL); //2sec
  Serial.println("sensArrière moteur");
  motorIH.back();
  delay(5000UL);//5sec
  motorIH.stop();
  delay(2000UL);//2sec 

  */
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
