/*    Arduino Anemometer and ÉolienneRPM Radio Transmitter using HC-12 and 2 Hall caption    */
    
#include <SoftwareSerial.h>
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
char MessageVent[] = " VENT\n"; // alarm message to be sent; '\n' is a forced terminator char
char MessageTest[] = " EOLIENNE\n"; // alarm message to be sent; '\n' is a forced terminator char
String chaine;
int rotaAnemo = 0;
int rotaEol = 0;

/*********/
/* SETUP */
/*********/  
void setup() {
  HC12.begin(9600);               // Serial port to HC12
  // debug uniquement, penser à commenter toutes les lignes «Serial…» pour éviter les erreurs de valeur (calcul trop long pour l'interruption)
//  Serial.begin(115200);             // Serial port to computer
  // Pin capteurs
  attachInterrupt(0, rpm_vent, FALLING);  // le capteur à effet Hall Anémomètre est connecté à la pin 2 = int0
  attachInterrupt(1, rpm_eol, FALLING);  // le capteur à effet Hall Éolienne est connecté à la pin 3 = int1      
}

/*************/
/* PROGRAMME */
/*************/
void loop() {
  RemiseZeroVitVentKMH ();
  RemiseZeroVitEolRPM ();
  
  chaine = String(vitVentKMH) + MessageVent + String(vitEolRPM) + MessageTest;
//  Serial.println ( "chaine String : " +chaine );
  HC12.print(chaine); 
  
  delay(100);
}

/*************/
/* FONCTIONS */
/*************/
void rpm_vent()   // appelée par l'interruption, Anémomètre vitesse du vent.
{ 
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
 // Serial.print ( "rpm : " );
 // Serial.println ( vitEolRPM ); // affiche les rpm  
 // Serial.println ( "" );
  dateDernierChangementEol = dateCourante;
  rotaEol = 1;
}


void RemiseZeroVitVentKMH ()
{
  unsigned long dateCouranteKMH = millis();
  if (rotaAnemo == 1 ){ // teste si l'anémemètre tourne
//    Serial.println ( "Anémo tourne ");
    rotaAnemo = 0;
  }
  else    // Si ça ne tourne plus (valeur plus mise à jour)
  {  
    float dureeKMH = (dateCouranteKMH - dateDernierChangementKMH);
    if (dureeKMH > 10000) // Si ça ne tourne plus depuis 10 secondes
    {
//      Serial.print ( "dureeKMH : " );
//      Serial.println ( dureeKMH ); // affiche les rpm  
      vitVentKMH = 0;  // Remsise à zero !
      dateDernierChangementKMH = dateCouranteKMH;    
    }
  }
}


void RemiseZeroVitEolRPM ()
{
  unsigned long dateCouranteRPM = millis();
  if (rotaEol == 1 ){ // teste si l'éolienne tourne
//    Serial.println ( "Éolienne tourne ");
    rotaEol = 0;
  }
  else    // Si ça ne tourne plus (valeur plus mise à jour)
  {  
    float dureeRPM = (dateCouranteRPM - dateDernierChangementRPM);
    if (dureeRPM > 65000) // Si ça ne tourne plus depuis 65 secondes (soit moins de 1 rpm)
    {
//      Serial.print ( "dureeRPM : " );
//      Serial.println ( dureeRPM ); // affiche les rpm  
      vitEolRPM = 0;  // Remsise à zero !
      dateDernierChangementRPM = dateCouranteRPM;    
    }
  }
}

