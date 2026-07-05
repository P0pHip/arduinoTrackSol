//TODO Potentiellement à redéfinir !!!
#define FdcIH           5       // On associe le Capteur de l'inclinaison vers l'Horizontale à la pin D4 de l'arduino
#define FdcIV           4       // On associe le Capteur de l'inclinaison vers la verticale à la pin D3 de l'arduino
#define FdcMV           3       // On associe le Capteur du moteur vers l'avant à la pin D2 de l'arduino
#define FdcMR           2       // On associe le Capteur du moteur vers l'arrière à la pin D1 de l'arduino
//TODO Potentiellement à redéfinir !!!

//TODO Potentiellement à redéfinir !!!
#define cptLumB         3      // On associe le Capteur de luminisoté situé en bas à la pin A1 de l'arduino SUD JAUNE
#define cptLumH         1      // On associe le Capteur de luminisoté situé en haut à la pin A2 de l'arduino  NORD BLANC
#define cptLumD         2      // On associe le Capteur de luminisoté situé en bas à droite au pin A3 de l'arduino EST  NOIR
#define cptLumG         4      // On associe le Capteur de luminisoté situé en bas à gauche au pin A4 de l'arduino  OUEST  MARRON
#define cptAnemo        5      // On associe le Capteur vent au pin A7 de l'arduino

int seuilAnemo = 400;

int captLumH;
int captLumB;
int captLumG;
int captLumD;
int captAnemo;

int captfdcIH; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcIV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMV; //declaration de la variable 0 ou 1 soit ouvert ou fermée
int captfdcMR; //declaration de la variable 0 ou 1 soit ouvert ou fermée

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);            // Mise en place de bibliothèque série à 9600 bps 
  Serial.println( "test du setup" );
  

  //TODO Potentiellement à redéfinir !!!
  pinMode(FdcIH,     INPUT);
  pinMode(FdcIV,     INPUT);
  pinMode(FdcMV,     INPUT);
  pinMode(FdcMR,     INPUT);
  
 
}

void loop() {
Serial.println ("loop");
  captLumH = analogRead(cptLumH);
  captLumB = analogRead(cptLumB);
  captLumG = analogRead(cptLumG);
  captLumD = analogRead(cptLumD);
  captAnemo = analogRead(cptAnemo);
  captfdcIH = digitalRead(FdcIH);
  captfdcIV = digitalRead(FdcIV);
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

  delay(5000UL);
}
