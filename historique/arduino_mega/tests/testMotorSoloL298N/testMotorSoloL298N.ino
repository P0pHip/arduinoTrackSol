#define ENAIH    3     // Moteur Inclinaison
#define IN1IH    4
#define IN2IH    5


const char MARCHE_AVANT   = 'V';            // Défini une constante pour la "marche avant" (peu importe la valeur)
const char MARCHE_ARRIERE = 'R';            // Défini une constante pour la "marche arrière" (peu importe la valeur)
 
#define vitesseMoteur   245

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  // put your setup code here, to run once:
  

  pinMode(ENAIH, OUTPUT);
  pinMode(IN1IH, OUTPUT);
  pinMode(IN2IH, OUTPUT);
  
 

  
}

void loop() {

    Serial.println("Moteur INCLINAISON");
    delay(1000UL); //5sec
    configurerSensDeRotationPontMoteur('R'); // avant ou arrière // a modifier peut etre
    changeVitesseMoteurPontMoteur(vitesseMoteur); 
    delay(5000UL); //5sec
    changeVitesseMoteurPontMoteur(0); 
    delay(2000UL); //2sec
    Serial.println("sensArrière moteur");
    configurerSensDeRotationPontMoteur('V'); // avant ou arrière // a modifier peut etre
    changeVitesseMoteurPontMoteur(vitesseMoteur); 
    delay(5000UL); //5sec
    changeVitesseMoteurPontMoteur(0); 
    delay(2000UL); //2sec

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
