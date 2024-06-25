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

//************************//
// Variables du programme //
//************************//
#define vitMotIH        255  // a toi de voir
#define vitMotEO        80
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
  for(int i =0; i<3; i++){
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

void trackSol_step(BTS7960 motor, int fdc, int cpt1, int cpt2, BH1750 lum1, BH1750 lum2, bool front){

  // lecture des valeurs de lux
  tcaSelect(cpt1);
  float luxCpt1 = lum1.readLightLevel();

  tcaSelect(cpt2);
  float luxCpt2 = lum2.readLightLevel();

  if(luxCpt1 <= luxCpt2)
  {
    Serial.println("Action trackSol step debut pause");
    
    delay(30000UL * timeBoucle); 
    tcaSelect(cpt1);
    luxCpt1 = lum1.readLightLevel();
    
    tcaSelect(cpt2);
    luxCpt2 = lum2.readLightLevel();

        
    if(luxCpt1 <= luxCpt2)
    {
      int valFdc = digitalRead(fdc);
      Serial.println ("je fais l'action");
      tcaSelect(cpt1);
      float luxCpt1 = lum1.readLightLevel();

      tcaSelect(cpt2);
      float luxCpt2 = lum2.readLightLevel();
      
      while (luxCpt1 <= luxCpt2)
      {        
        if (valFdc == 0)
        {
          Serial.println("capt Activé Break");
          break;
        }
        if(front){
          motor.front();
        }
        else{
          motor.back();
        }
                  
        valFdc = digitalRead(fdc); // lecture du signal du capteur
          
        tcaSelect(cpt1);
        float luxCpt1 = lum1.readLightLevel();

        tcaSelect(cpt2);
        float luxCpt2 = lum2.readLightLevel();
      }
      motor.stop(); 
      Serial.println("cycle step terminer");          
    }
  }
}  

void printVal(){  

  tcaSelect(0);
  Serial.print("valeur Haut/nord: "); // affichage des valeurs pour le debugages
  Serial.println (nord.readLightLevel());

  tcaSelect(1);
  Serial.print("Valeur Bas/sud: ");
  Serial.println (sud.readLightLevel());

  tcaSelect(2);
  Serial.print("Valeur Gauche/est: ");
  Serial.println (est.readLightLevel());

  tcaSelect(3);
  Serial.print("Valeur Droite/ouest: ");
  Serial.println (ouest.readLightLevel());
  
  
  Serial.print("valeur FDC IH: ");
  Serial.println (digitalRead(FdcIH));
  Serial.print("Valeur FDC IV: ");
  Serial.println (digitalRead(FdcIV));
  Serial.print("Valeur FDC du moteur vers Est: ");
  Serial.println (digitalRead(FdcEst));
  Serial.print("Valeur FDC du moteur vers l'ouest: ");
  Serial.println (digitalRead(FdcOuest));
}

void safetyCheck() { // avoir ou l'utilser
    if (digitalRead(FdcEst) == LOW || digitalRead(FdcOuest) == LOW) {
        motorEO.stop();
    }
    if (digitalRead(FdcIH) == LOW || digitalRead(FdcIV) == LOW) {
        motorIH.stop();
    }
}


//*******//
// SETUP //
//*******//

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

void loop() {
  
    // le code arrive
  Serial.println ("loop");
  delay(5000UL); //5sec

  printVal();

  tcaSelect(0);
  float luxNord = nord.readLightLevel();

  tcaSelect(1);
  float luxSud = sud.readLightLevel();

  tcaSelect(2);
  float luxEst = est.readLightLevel();

  tcaSelect(3);
  float luxOuest = ouest.readLightLevel();

  while(luxSud>= seuilLum || luxNord>= seuilLum || luxEst>=seuilLum || luxOuest>=seuilLum)
  {
    printVal();
    //void trackSol_step(BTS7960 motor, int fdc, int cpt1, int cpt2, BH1750 lum1, BH1750 lum2)
    Serial.println("Soleil time");
    trackSol_step(motorEO,FdcOuest,3,2,ouest,est,true);
    Serial.println("Soleil time End");

    Serial.println("Retour time");
    trackSol_step(motorEO,FdcEst,2,3,est,ouest,false);
    Serial.println("Retour time End");

    Serial.println("Horizon time");
    trackSol_step(motorIH,FdcIH,1,0,sud,nord,false);
    Serial.println("Horizon time End");

    Serial.println("Vertical time");
    trackSol_step(motorIH,FdcIV,0,1,nord,sud,true);
    Serial.println("Vertical time End");
    
  }
    
  Serial.println ("je me remet a zéro");    
  while(digitalRead(FdcEst) == 1)
  {
    motorEO.back();
    digitalRead(FdcEst); // lecture du signal du capteur
  }
  motorEO.stop();
  motorIH.stop();
  
  while(digitalRead(FdcIH) == 1)
  {
    Serial.println("mise a plat");
    motorIH.back(); 
    digitalRead(FdcIH); // lecture du signal du capteur             
  }
  motorEO.stop();
  motorIH.stop();
  Serial.println("cycle retour a zéro terminer");
}
