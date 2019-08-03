/*    Arduino Anemometer Radio Receiver using HC-12, 2 Hall caption and LCD Module   */

/*  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)*/

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

SoftwareSerial HC12(8, 9); // HC-12 TX Pin, HC-12 RX Pin

char acquis_data;
String chaine;
//https://plaisirarduino.fr/moniteur-serie/

int vent;
int eolienne;
//https://forum.arduino.cc/index.php?topic=553743.0

/*********/
/* SETUP */
/*********/  
void setup() {
  Serial.begin(115200);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12

  lcd.begin(20, 2);
  lcd.setCursor(0, 0);  
  // Print a message to the LCD.
  lcd.print("    Anemometer"); 
  lcd.setCursor(0, 1);  
  lcd.print("    Test");           
  delay(1000); 
  lcd.clear(); 
}

/*************/
/* PROGRAMME */
/*************/
void loop() {
// surtout pas de delay dans cette boucle, sinon les data reçues sont erronnées.
  while (HC12.available()) {      // If HC-12 has data
    acquis_data = HC12.read();
    chaine = chaine + acquis_data;
      Serial.println (chaine);           // debug
/* message reçu de la forme 
114 VENT
937 EOLIENNE
pour chaque ligne on fait :*/
 //   if (acquis_data == 10) {  //détection de fin de ligne : en ascii
    if (chaine.endsWith("\n")) {  //détection de fin de ligne : méthodes String
//      Serial.println ("fin de ligne");   // debug


        if (chaine.endsWith("V\n")) {  //détection de fin de ligne méthodes String
//          Serial.println ( "vent détecté");    // debug
          char resultat[chaine.length()+1];   // déclare une char "resultat" avec la longueur+1 de la String
          chaine.toCharArray(resultat, chaine.length()+1);  // convertis la String en chaine
          sscanf(resultat, "%d VENT", &vent); // parse le texte de la char, %d pour le texte avant le mot VENT, mis dans la variable vent
 //         Serial.print("VENT:");
  //        Serial.println(vent);
                 
          lcd.setCursor(0, 0); 
          lcd.print("VENT: "); 
          lcd.print(vent);   
          lcd.print(" Km/h");
         }
         else {
//          Serial.println ("vent non détecté ");    // debug
          char resultat[chaine.length()+1];
          chaine.toCharArray(resultat, chaine.length()+1);
          sscanf(resultat, "%d EOLIENNE", &eolienne);
  //        Serial.print("EOLIENNE:");
  //        Serial.println(eolienne);

          lcd.setCursor(0, 1); 
          lcd.print("EOLIENNE: "); 
          lcd.print(eolienne);   
          lcd.print(" rpm");
          }
       
          chaine = "";  // vide la String
    }
  }


}
