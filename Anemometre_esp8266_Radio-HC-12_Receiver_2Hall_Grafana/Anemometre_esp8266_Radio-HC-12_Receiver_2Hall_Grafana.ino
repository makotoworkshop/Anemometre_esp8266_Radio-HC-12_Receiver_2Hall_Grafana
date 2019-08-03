#include "ESPinfluxdb.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

/****************/
/* DÉCLARATIONS */
/****************/
const char *INFLUXDB_HOST = "ddomain.org";
const uint16_t INFLUXDB_PORT = 8086;

const char *DATABASE = "base";
const char *DB_USER = "user";
const char *DB_PASSWORD = "pass";

SoftwareSerial HC12(D5, D6); // HC-12 TX Pin, HC-12 RX Pin

ESP8266WiFiMulti WiFiMulti;

Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT);  //https://github.com/projetsdiy/grafana-dashboard-influxdb-exp8266-ina219-solar-panel-monitoring/tree/master/solar_panel_monitoring

char acquis_data;
String chaine;
//https://plaisirarduino.fr/moniteur-serie/

int vent;
int eolienne;
//https://forum.arduino.cc/index.php?topic=553743.0

int compteur = 1;

/*********/
/* SETUP */
/*********/ 
void setup() {
  Serial.begin(115200);           // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
        
  WiFiMulti.addAP("ssid", "password");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println("Ready");

  //influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD);
  // Connexion à la base InfluxDB - Connect to InfluxDB database
  while (influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD)!=DB_SUCCESS) {
    Serial.println("Open database failed");
    delay(10000);
  }
}

/*************/
/* FONCTIONS */
/*************/
void ReceiveDataFromRadio() {
// surtout pas de delay dans cette boucle, sinon les data reçues sont erronnées.
  while (HC12.available()) {      // If HC-12 has data
    acquis_data = HC12.read();
    chaine = chaine + acquis_data;
//      Serial.println (chaine);           // debug
/* message reçu de la forme 
114 VENT
937 EOLIENNE
pour chaque ligne on fait :*/
 //   if (acquis_data == 10) {  //détection de fin de ligne : en ascii
    if (chaine.endsWith("\n")) {  //détection de fin de ligne : méthodes String
 //     Serial.println ("fin de ligne");   // debug


        if (chaine.endsWith("V\n")) {  //détection de fin de ligne méthodes String
//          Serial.println ( "vent détecté");    // debug
          char resultat[chaine.length()+1];   // déclare une char "resultat" avec la longueur+1 de la String
          chaine.toCharArray(resultat, chaine.length()+1);  // convertis la String en chaine
          sscanf(resultat, "%d VENT", &vent); // parse le texte de la char, %d pour le texte avant le mot VENT, mis dans la variable vent
 //         Serial.print("VENT:");
 //         Serial.println(vent);            
         }
         else {
//          Serial.println ("vent non détecté ");    // debug
          char resultat[chaine.length()+1];
          chaine.toCharArray(resultat, chaine.length()+1);
          sscanf(resultat, "%d EOLIENNE", &eolienne);
 //         Serial.print("EOLIENNE:");
 //         Serial.println(eolienne);
          }
       
          chaine = "";  // vide la String
    }
  }
}


void SendDataToInfluxdbServer() {
  //Writing data with influxdb HTTP API: https://docs.influxdata.com/influxdb/v1.5/guides/writing_data/
  //Querying Data: https://docs.influxdata.com/influxdb/v1.5/query_language/
 
  // Envoi la vitesse du vent et la vitesse de rotation de l'éolienne
  dbMeasurement rowDATA("Data");
  rowDATA.addField("VitesseVent", vent);
  rowDATA.addField("RotationEolienne", eolienne);
 // Serial.println(influxdb.write(rowDATA) == DB_SUCCESS ? " - rowDATA write success" : " - Writing failed");
  influxdb.write(rowDATA);

  // Vide les données - Empty field object.
  rowDATA.empty();
}

/*************/
/* PROGRAMME */
/*************/
void loop() {
  ReceiveDataFromRadio();

  if (compteur > 500) { // envoie la data toutes les 10 secondes (delay 10 x 500 = 5000 ms, et l'opération prend 5 secondes à transférer/écrire en base)
    SendDataToInfluxdbServer();  
    compteur = 1;
  }
  else {
    compteur++;
  }
  delay(10);  // refresh la data sur l'afficheur toutes les 10 ms
//  Serial.print ( "compteur = " );
//  Serial.println(compteur);
}
