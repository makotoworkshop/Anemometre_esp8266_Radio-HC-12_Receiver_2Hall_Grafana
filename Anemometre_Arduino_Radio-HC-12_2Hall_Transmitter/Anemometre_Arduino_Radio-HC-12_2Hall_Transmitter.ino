/* Arduino Anemometer and Tachometer Éolienne
IN : capteur à effet Hall Anémomètre est connecté à la pin 2 = int0
IN : capteur à effet Hall Éolienne est connecté à la pin 3 = int1  
OUT : HC-12 Radio Default Mode 9600bps
Pour activer le mode d'économie d'énergie de l'arduino et du HC-12, décommenter les lignes :
// ModeVeille();
// Reveil();
*/
    
#include <SoftwareSerial.h>
#include <avr/sleep.h>
/****************/
/* DÉCLARATIONS */
/****************/
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
unsigned long rpmVent = 0;
unsigned long rpmEol = 0;
unsigned long vitVentKMH = 0;
unsigned long vitEolRPM = 0;
unsigned long dateDernierChangementVent = 0;
unsigned long dateDernierChangementEol = 0;
unsigned long dateDernierChangementRPM = 0;
unsigned long dateDernierChangementKMH = 0;
float intervalleKMH = 0;
float intervalleRPM = 0;
char MessageVent[] = " V\n"; // message to be sent; '\n' is a forced terminator char
char MessageEol[] = " E\n"; // message to be sent; '\n' is a forced terminator char
String chaine;
int rotaAnemo = 0;
int rotaEol = 0;
#define ATpin 7 // used to switch HC-12 to AT mode
int StatusVeille = 0;

/*********/
/* SETUP */
/*********/  
void setup() {
  HC12.begin(9600);               // Serial port to HC12
 //   HC12.begin(115200);               // Serial port to HC12
  // debug uniquement, penser à commenter toutes les lignes «Serial…» pour éviter les erreurs de valeur (calcul trop long pour l'interruption)
 // Serial.begin(9600);             // Serial port to computer
  // Pin capteurs
  attachInterrupt(digitalPinToInterrupt(2), rpm_vent, FALLING);  // le capteur à effet Hall Anémomètre est connecté à la pin 2 = int0
  attachInterrupt(digitalPinToInterrupt(3), rpm_eol, FALLING);  // le capteur à effet Hall Éolienne est connecté à la pin 3 = int1  
//  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
 // digitalWrite(LED_BUILTIN,HIGH);   //turning LED on
  pinMode(ATpin, OUTPUT);
  digitalWrite(ATpin, HIGH); // HC-12 en normal mode
}

/*************/
/* PROGRAMME */
/*************/
void loop() {
  RemiseZeroVitVentKMH ();
  RemiseZeroVitEolRPM ();
  chaine = String(vitVentKMH) + MessageVent + String(vitEolRPM) + MessageEol;  // construction du message
//  Serial.println ( "chaine String : " +chaine );
  HC12.print(chaine); // send radio data
  delay(100);
}

/*************/
/* FONCTIONS */
/*************/

void rpm_vent()   // appelée par l'interruption, Anémomètre vitesse du vent.
{ 
 // Reveil();   // Pour sortir du mode veille de l'Arduino et du HC-12
  unsigned long dateCourante = millis();
  intervalleKMH = (dateCourante - dateDernierChangementVent);
//  Serial.print ( "intervalle en s : " );
//  Serial.println (intervalleKMH/1000); // affiche l'intervalle de temps entre deux passages
  if (intervalleKMH != 0)  // attention si intervalle = 0, division par zero -> erreur
  {
    rpmVent = 60 / (intervalleKMH /1000);  
  }
  vitVentKMH = ( rpmVent + 6.174 ) / 8.367;
//  Serial.print ( "vitVentKMH : " );
//  Serial.println ( vitVentKMH ); // affiche les rpm  
//  Serial.println ( "" );
  dateDernierChangementVent = dateCourante;
  rotaAnemo = 1;
}

void rpm_eol()    // appelée par l'interruption, Tachymétre rotation éolienne.
{
  unsigned long dateCourante = millis();
  intervalleRPM = (dateCourante - dateDernierChangementEol);
//  Serial.print ( "intervalle en s : " );
//  Serial.println (intervalleRPM/1000); // affiche l'intervalle de temps entre deux passages
  if (intervalleRPM != 0)  // attention si intervalle = 0, division par zero -> erreur
  {  
    vitEolRPM = 60 / (intervalleRPM /1000);
  }
//  Serial.print ( "rpm : " );
//  Serial.println ( vitEolRPM ); // affiche les rpm  
 // Serial.println ( "" );
  dateDernierChangementEol = dateCourante;
  rotaEol = 1;
}

void RemiseZeroVitVentKMH ()
{
  unsigned long dateCouranteKMH = millis();
  if (rotaAnemo == 1 ){   // teste si l'Anémomètre tourne
 //   Serial.println ( "Anémo tourne ");
    float dureeKMH = (dateCouranteKMH - dateDernierChangementKMH);
    if (dureeKMH > 10000) {
      rotaAnemo = 0;
      dateDernierChangementKMH = dateCouranteKMH; 
    }
  }
  else    // Si ça ne tourne plus (valeur plus mise à jour)
  {  
    float dureeKMH = (dateCouranteKMH - dateDernierChangementKMH);
    if (dureeKMH > 6000) // Si ça ne tourne plus depuis 6 secondes
    {
 //     Serial.print ( "dureeKMH : " );
  //    Serial.println ( dureeKMH ); // affiche les rpm  
      vitVentKMH = 0;  // Remsise à zero !
      dateDernierChangementKMH = dateCouranteKMH;   
  //    ModeVeille(); // Comme ça ne tourne plus, passer l'Arduino en veille
    }
  }
}

void RemiseZeroVitEolRPM ()
{
  unsigned long dateCouranteRPM = millis();
  if (rotaEol == 1 ){ // teste si l'éolienne tourne
//    Serial.println ( "Éolienne tourne ");
    float dureeRPM = (dateCouranteRPM - dateDernierChangementRPM);
    if (dureeRPM > 10000) {
      rotaEol = 0;
      dateDernierChangementRPM = dateCouranteRPM;
    }
  }
  else    // Si ça ne tourne plus (valeur plus mise à jour)
  {  
    float dureeRPM = (dateCouranteRPM - dateDernierChangementRPM);
    if (dureeRPM > 6000) // Si ça ne tourne plus depuis 6 secondes
    {
//      Serial.print ( "dureeRPM : " );
//      Serial.println ( dureeRPM ); // affiche les rpm  
      vitEolRPM = 0;  // Remsise à zero !
      dateDernierChangementRPM = dateCouranteRPM;    
    }
  }
}

void ModeVeille(){    // économie d'énergie
       //     Serial.println ( "MODE VEILLE ACTIF " );
  StatusVeille = 1;
  detachInterrupt(digitalPinToInterrupt(2));  // désactiver l'interruption pour laisser le temps à la fonction de s'exécuter
//  digitalWrite(LED_BUILTIN,LOW);    //turning LED off
  vitEolRPM = 0;  // À commenter selon le cas : Remsise à zero aussi de la rotation éolienne, car elle ne peut plus tourner si l'anémométre ne tourne plus, étant donné que le générateur freine beaucoup trop sa rotation inertielle. Et donc si l'arduino est déjà en veille, impossible de remttre à zero la rotation éolienne.
  chaine = String(vitVentKMH) + MessageVent + String(vitEolRPM) + MessageEol;
  HC12.print(chaine);   // pour envoyer la derniere valeur zero avant mise en veille.
  delay(100); // laisser le temps à la data d'être envoyée.
  digitalWrite(ATpin, LOW); // HC-12 en normal AT.
  delay(100); // laisser le temps au HC-12 de switcher de mode.
  HC12.print("AT+SLEEP"); // passe le HC-12 en mode veille. (économie d'énergie)
  delay(100); // laisser le temps au HC-12 de switcher de mode.
  digitalWrite(ATpin, HIGH); // HC-12 en normal mode
  sleep_enable(); //Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //Setting the Arduino full sleep mode
  sleep_cpu();  //activating sleep mode

  attachInterrupt(digitalPinToInterrupt(2), rpm_vent, FALLING);  // le capteur à effet Hall Anémomètre est connecté à la pin 2 = int0
}

  
void Reveil(){
  if (StatusVeille == 1)
  {
    detachInterrupt(digitalPinToInterrupt(2));  // désactiver l'interruption pour laisser le temps à la fonction de s'exécuter
    sleep_disable();  //Disable Arduino sleep mode
//    digitalWrite(LED_BUILTIN,HIGH);   //turning LED on
    digitalWrite(ATpin, LOW); // HC-12 en normal AT.
    delay(100);
    HC12.print("AT+DEFAULT"); // passe le HC-12 en mode par défaut pour réveiller le HC-12
    delay(100);
    digitalWrite(ATpin, HIGH); // HC-12 en normal mode
    StatusVeille = 0;
      //      Serial.println ( "REVEIL " );

    attachInterrupt(digitalPinToInterrupt(2), rpm_vent, FALLING);  // le capteur à effet Hall Anémomètre est connecté à la pin 2 = int0
  }
}
