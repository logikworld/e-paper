//DEBUG ON OFF
#define DEBUGKL true
const int NbDataMemory = 1000;
long interval = 5000; //250000; // READING INTERVAL
//constantes PI
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
//
//BROCHAGES:
#include "pins_arduino.h"
//maths
#include "math.h"
//
//brochage pour le montage:
//Le bus SPI utilise quatre signaux logiques :
//
//    SCLK — Serial Clock, Horloge (généré par le maître)
//    MOSI — Master Output, Slave Input (généré par le maître)
//    MISO — Master Input, Slave Output (généré par l'esclave)
//    SS — Slave Select, Actif à l'état bas (généré par le maître)
//
//Il existe d'autres noms qui sont souvent utilisés :
//
//    SCK, SCL — Horloge (généré par le maître)
//    SDI, DI, SI — Serial Data IN, MISO
//    SDO, SDA, DO, SO — Serial Data OUT, MOSI
//    nCS, CS, nSS, STE, CSN— SS
//E-PAPER
// SPI pin definition
//#define CS_PIN 15
//#define RST_PIN 2
//#define DC_PIN 4
//#define BUSY_PIN 5
//mémoire et horloge temps reel I2C
//D1: SCL
//D2: SDA
// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: D4
// GESTION WIFI:
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "scripts.h"    // JavaScript code
#include "css.h"        // Cascading Style Sheets
#include "html.h"       // HTML page of the tool
#include "epd.h"        // e-Paper driver
//
//dessin sur EPAPER
#include "GUI_Paint.h"
#include "DEV_Config.h"
//
//horloge
#include <time.h> //horloge par ntp wifi
#include <TimeLib.h>
//#include <NtpClientLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
//horloge NTP web:
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 3600, 60000);
String SLHeure = "00:00:00";
String SLHeureLue = "00:00:00";
char oldminute = 0;

const char* ssid = "WIFI";
const char* password = "Password";
//
//Gestion IP:
ESP8266WebServer server(80);
IPAddress myIP;       // IP address in your local wifi net
//
//EPAPER 4.2 NOIR et ROUGE:
//création d'un tampon d'image:
int epaperwidth = 400;
int epaperheight = 300;
UBYTE *ImageEpaper; // black \Red or Yellow
UWORD ImagesizeEpaper = ((epaperwidth % 8 == 0) ? (epaperwidth / 8 ) : (epaperwidth / 8 + 1)) * epaperheight;
//
//          _____                                   _                     __       _  _               _     _  _  _
//         |_   _|___  _ __   _ __  ___  _ _  __ _ | |_  _  _  _ _  ___  / _|___  | || | _  _  _ __  (_) __| |(_)| |_  ___
//           | | / -_)| '  \ | '_ \/ -_)| '_|/ _` ||  _|| || || '_|/ -_) > _|_ _| | __ || || || '  \ | |/ _` || ||  _|/ -_)
//           |_| \___||_|_|_|| .__/\___||_|  \__,_| \__| \_,_||_|  \___| \_____|  |_||_| \_,_||_|_|_||_|\__,_||_| \__|\___|
//                           |_|
#include "DHTesp.h"
//uint32_t delayMS;
//#define DHTPIN D6 // SENSOR PIN
#include <dhtnew.h>
DHTNEW mySensor(D6);
//D0 J5 PIN1 sortie LED ROUGE EPAPER board
//D2 J5 PIN3 GPIO4 utilisée par ecran
//D6 J5 PIN 9
//patte 1 vers le S (de face carte composant:)
// 1  Data
// 2  VCC
// 3  GND
float temperaturemesure;
float humiditymesure;
boolean *garde;
float *temperature;
float *humidity;
//unsigned long
time_t *mesTps;
time_t FirstTps;
char Aff[64];
//uint32_t mesTps[0] = 0;
//uint32_t mesTps[cptmesure] = 0;
uint32_t graphdate = 0;
//float lasttemp = 0;
float tempmin = 100;
float tempmax = -100;
byte humidmin = 100;
byte humidmax = 0;
byte humidmoy = 0;
float tempmoy = 0;
float beforetemp;
byte beforehumi;
float graphtempmin = 100;
float graphtempmax = -100;
float graphhumidmin = 100;
float graphhumidmax = -100;
//float lasthumi = 0;
float SeuilMail = 60; //on allume la diode rouge !
int cptmesure = 0;
boolean Alerte30deg = false;
long previousMillis = 0;
unsigned long currentMillis = 0;

//
//           SSSSSSSSSSSSSSS EEEEEEEEEEEEEEEEEEEEEETTTTTTTTTTTTTTTTTTTTTTTUUUUUUUU     UUUUUUUUPPPPPPPPPPPPPPPPP
//         SS:::::::::::::::SE::::::::::::::::::::ET:::::::::::::::::::::TU::::::U     U::::::UP::::::::::::::::P
//        S:::::SSSSSS::::::SE::::::::::::::::::::ET:::::::::::::::::::::TU::::::U     U::::::UP::::::PPPPPP:::::P
//        S:::::S     SSSSSSSEE::::::EEEEEEEEE::::ET:::::TT:::::::TT:::::TUU:::::U     U:::::UUPP:::::P     P:::::P
//        S:::::S              E:::::E       EEEEEETTTTTT  T:::::T  TTTTTT U:::::U     U:::::U   P::::P     P:::::P
//        S:::::S              E:::::E                     T:::::T         U:::::D     D:::::U   P::::P     P:::::P
//         S::::SSSS           E::::::EEEEEEEEEE           T:::::T         U:::::D     D:::::U   P::::PPPPPP:::::P
//          SS::::::SSSSS      E:::::::::::::::E           T:::::T         U:::::D     D:::::U   P:::::::::::::PP
//            SSS::::::::SS    E:::::::::::::::E           T:::::T         U:::::D     D:::::U   P::::PPPPPPPPP
//               SSSSSS::::S   E::::::EEEEEEEEEE           T:::::T         U:::::D     D:::::U   P::::P
//                    S:::::S  E:::::E                     T:::::T         U:::::D     D:::::U   P::::P
//                    S:::::S  E:::::E       EEEEEE        T:::::T         U::::::U   U::::::U   P::::P
//        SSSSSSS     S:::::SEE::::::EEEEEEEE:::::E      TT:::::::TT       U:::::::UUU:::::::U PP::::::PP
//        S::::::SSSSSS:::::SE::::::::::::::::::::E      T:::::::::T        UU:::::::::::::UU  P::::::::P
//        S:::::::::::::::SS E::::::::::::::::::::E      T:::::::::T          UU:::::::::UU    P::::::::P
//         SSSSSSSSSSSSSSS   EEEEEEEEEEEEEEEEEEEEEE      TTTTTTTTTTT            UUUUUUUUU      PPPPPPPPPP
void setup(void)
{
  //Démarrage SERIAL PORT
  Serial.begin(115200);
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(F("EPAPER Logik Temperture et Humidité"));
  //creation du tampon pour l'image:
  Serial.print("Creation Tampon image:");
  if ((ImageEpaper = (UBYTE *)malloc(ImagesizeEpaper)) == NULL)
  {
    Serial.println(F("KO:"));
    Serial.println(ImagesizeEpaper);
    while (1);
  }
  else
  {
    Serial.println(F("OK"));
  }
  Serial.print(F("Creation Tampon Mesures:"));
  Serial.print(F("Garde:"));
  if ((garde = (boolean *)malloc(NbDataMemory)) == NULL)
  {
    Serial.println(F("KO:"));
    Serial.println(NbDataMemory);
    while (1);
  }
  else
  {
    Serial.println(F("OK"));
  }
  Serial.print(F("temp:"));
  if ((temperature = (float *)malloc(NbDataMemory)) == NULL)
  {
    Serial.println(F("KO:"));
    Serial.println(NbDataMemory);
    while (1);
  }
  else
  {
    Serial.println(F("OK"));
  }
  Serial.print(F("humi:"));
  if ((humidity = (float *)malloc(NbDataMemory)) == NULL)
  {
    Serial.println(F("KO:"));
    Serial.println(NbDataMemory);
    while (1);
  }
  else
  {
    Serial.println(F("OK"));
  }
  Serial.print(F("tps:"));
  if ((mesTps = (time_t *)malloc(NbDataMemory)) == NULL)
  {
    Serial.println(F("KO:"));
    Serial.println(NbDataMemory);
    while (1);
  }
  else
  {
    Serial.println(F("OK"));
  }
  Serial.print(F("SPI:"));
    // SPI initialization
  pinMode(CS_PIN  , OUTPUT);
  pinMode(RST_PIN , OUTPUT);
  pinMode(DC_PIN  , OUTPUT);
  pinMode(BUSY_PIN,  INPUT);
  SPI.begin();
  Serial.println(F("OK"));
  EPD_Affiche("Initialisation en cours", "Veuillez patienter, mesures en cours d'acquisition.");
  //Démarrage de la sonde temperature:
  mySensor.read();
#if DEBUGKL
  Serial.println(F("DHT11: Sonde temperature & Humidité"));
  Serial.print(mySensor.humidity);
  Serial.print("\t");
  Serial.println(mySensor.temperature);
#endif
  //Connection NTP WIFI UDP
  WiFi.begin(ssid, password);
  // Connect to WiFi network
  Serial.print("Connexion à ");
  Serial.println(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  //
  // NTP CLIENT BEGIN
  timeClient.begin();
  #if DEBUGKL
    Serial.println(F("Horloge:"));
  #endif
  timeClient.setTimeOffset(3600 * 2);
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  #if DEBUGKL 
    Serial.println(F("Deconnexion:"));
  #endif
  WiFi.disconnect();
  //
  // Wait for disconnection
  while (WiFi.status() == WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  #if DEBUGKL
    Serial.println("");
    Serial.println(F("Reconnexion IP:"));
  #endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Static IP setting---by Lin
  wifi_station_dhcpc_stop();
  struct ip_info info;
  //IP4_ADDR(&info.ip, 192,168,1,251);
  //IP4_ADDR(&info.gw, 192,168,1,254);
  IP4_ADDR(&info.ip, 192, 168, 252, 251);
  IP4_ADDR(&info.gw, 192, 168, 252, 254);
  IP4_ADDR(&info.netmask, 255, 255, 255, 0);
  wifi_set_ip_info(STATION_IF, &info);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("OK."));
  //Serial.println(ssid);

  //
  #ifdef DEBUGKL
    Serial.print("\r\nIP address: ");
    Serial.println(myIP = WiFi.localIP());
  #endif
  //
  if (MDNS.begin("esp8266")) {
    Serial.print("MDNS:");
  }
  //
  server.on("/", handleRoot);
  server.on("/styles.css", sendCSS);
  server.on("/processingA.js", sendJS_A);
  server.on("/processingB.js", sendJS_B);
  server.on("/processingC.js", sendJS_C);
  server.on("/processingD.js", sendJS_D);
  server.on("/LOAD", EPD_Load);
  server.on("/EPD", EPD_Init);
  server.on("/NEXT", EPD_Next);
  server.on("/SHOW", EPD_Show);
  server.on("/TIME", EPD_Time);
  server.on("/FILL", Remplir);
  server.onNotFound(handleNotFound);
  //
  //lecture temperature (au moins 10secondes apres demarrage de la sonde:
  mySensor.read();
  temperaturemesure = mySensor.temperature;
  humiditymesure = mySensor.humidity;
  #ifdef DEBUGKL
    Serial.print(F("T=")); Serial.println(temperaturemesure); Serial.println(F("°C"));
    Serial.print(F("H=")); Serial.println(humiditymesure); Serial.println(F("%"));
  #endif
  cptmesure = 0;
  temperature[cptmesure] = temperaturemesure;
  humidity[cptmesure] = humiditymesure;
  mesTps[cptmesure] = timeClient.getEpochTime();
  FirstTps=mesTps[cptmesure];
  //lasttemp = temperaturemesure;
  //lasthumi = humiditymesure;
  humidmoy = humiditymesure;
  tempmoy = temperaturemesure;
  if (tempmin > temperaturemesure) tempmin = temperaturemesure;
  if (tempmax < temperaturemesure) tempmax = temperaturemesure;
  if (humidmin > humiditymesure) humidmin = humiditymesure;
  if (humidmax < humiditymesure) humidmax = humiditymesure;
  //mesTps[cptmesure] = timeClient.getEpochTime(); //(mesmonth[cptmesure]*24*3600*31)+(mesday[cptmesure]*24*3600)+(meshour[cptmesure]*3600)+(mesminute[cptmesure]*60)+messecond[cptmesure];
  //mesTps[0] = timeClient.getEpochTime(); //mesTps[cptmesure];
  SLHeure = timeClient.getFormattedTime(); //.substring(0,5);
  //EPD_Time();
  server.begin();
  if (DEBUGKL) Serial.println(F("HTTP OK"));
  previousMillis = millis();
}

//              LLLLLLLLLLL                  OOOOOOOOO          OOOOOOOOO     PPPPPPPPPPPPPPPPP
//              L:::::::::L                OO:::::::::OO      OO:::::::::OO   P::::::::::::::::P
//              L:::::::::L              OO:::::::::::::OO  OO:::::::::::::OO P::::::PPPPPP:::::P
//              LL:::::::LL             O:::::::OOO:::::::OO:::::::OOO:::::::OPP:::::P     P:::::P
//                L:::::L               O::::::O   O::::::OO::::::O   O::::::O  P::::P     P:::::P
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P::::P     P:::::P
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P::::PPPPPP:::::P
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P:::::::::::::PP
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P::::PPPPPPPPP
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P::::P
//                L:::::L               O:::::O     O:::::OO:::::O     O:::::O  P::::P
//                L:::::L         LLLLLLO::::::O   O::::::OO::::::O   O::::::O  P::::P
//              LL:::::::LLLLLLLLL:::::LO:::::::OOO:::::::OO:::::::OOO:::::::OPP::::::PP
//              L::::::::::::::::::::::L OO:::::::::::::OO  OO:::::::::::::OO P::::::::P
//              L::::::::::::::::::::::L   OO:::::::::OO      OO:::::::::OO   P::::::::P
//              LLLLLLLLLLLLLLLLLLLLLLLL     OOOOOOOOO          OOOOOOOOO     PPPPPPPPPP

void loop(void)
{
  timeClient.update();
  server.handleClient();
  //lecture temperature
  currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  { // READ ONLY ONCE PER INTERVAL
    mySensor.read();
    previousMillis = currentMillis;
    temperaturemesure = mySensor.temperature; //.getTemperature();
    humiditymesure = mySensor.humidity; //dht.getHumidity();
    //if (dht11.read(pinDHT11, &temperaturemesure, &humiditymesure, dhtdata))
    //{
    //  Serial.println("DHT:KO");
    //}
    //temperaturemesure=event.temperature;
    //humiditymesure=event.relative_humidity;
    if (DEBUGKL)
    {
      Serial.print(F("T=")); Serial.print(temperaturemesure); Serial.println(F("°C"));
    }
    if (DEBUGKL)
    {
      Serial.print(F("H=")); Serial.print(humiditymesure); Serial.println(F("%"));
    }
    //gestion des mesures
    {
      if (cptmesure >= (NbDataMemory - 1)) //pas de dépassement mémoire !
      {
        for (int cptmestemp = 0; cptmestemp < (NbDataMemory - 2); cptmestemp++)
        {
          temperature[cptmestemp] = temperature[cptmestemp + 1];
          humidity[cptmestemp] = temperature[cptmestemp + 1];
          mesTps[cptmestemp] = mesTps[cptmestemp + 1];
        }
        //cherche si reduction possible nb valeurs
        for (int cptmestemp = 0; cptmestemp < (NbDataMemory - 1); cptmestemp++)
        {
          if ((cptmestemp < (NbDataMemory - 2)) && (cptmestemp > 1))
          {
            if (((temperature[cptmestemp - 1] == temperature[cptmestemp]) && (humidity[cptmestemp - 1] == humidity[cptmestemp])) && ((temperature[cptmestemp] == temperature[cptmestemp + 1]) && (humidity[cptmestemp] == humidity[cptmestemp + 1])))
            {
              garde[cptmestemp] = false;
            }
            else
            {
              garde[cptmestemp] = true;
            }
          }
          else
          {
            garde[cptmestemp] = true;
          }
        }
        // on enleve les mesures identiques qui ne déforment pas la courbe
        int cptnext = 0;
        int cptmestempgarde = 0;
#if DEBUGKL
        Serial.println(F("Cherche prochain à garder:"));
        Serial.println(F("|N  |T  |H  |s  | Que l'on garde"));
#endif
        do
        {
          //je cherche premier à garder:
          while (garde[cptnext] == false)
          {
            cptnext = cptnext + 1;
            if (cptnext >= (NbDataMemory - 1)) break;
          }
          //donc on garde cptnext, on le stock en premier
          temperature[cptmestempgarde] = temperature[cptnext];
          humidity[cptmestempgarde] = humidity[cptnext];
          mesTps[cptmestempgarde] = mesTps[cptnext];
#if DEBUGKL
          Serial.print(F("| "));
          Serial.print(cptmestempgarde);
          Serial.print(F("| "));
          Serial.print(temperature[cptmestempgarde]);
          Serial.print(F("| "));
          Serial.print(humidity[cptmestempgarde]);
          Serial.print(F("| "));
          Serial.print( mesTps[cptmestempgarde] /*- mesTps[0]*/);
          Serial.println(F("|"));
#endif
          cptmestempgarde = cptmestempgarde + 1;
          cptnext = cptnext + 1;
        } while (cptnext <= (NbDataMemory - 1)); //on arrete on est à la fin
        cptmesure = max(1, min((NbDataMemory - 1), cptmestempgarde - 1)); //pas de dépassement mémoire !
#if DEBUGKL
        Serial.print(F("Reste Mesures "));
        Serial.println(cptmesure);
#endif
      }
      else
      {
        cptmesure = cptmesure + 1;
        cptmesure = max(1, min(cptmesure, (NbDataMemory - 1))); //pas de dépassement mémoire !
      }
      temperature[cptmesure] = temperaturemesure;
      humidity[cptmesure] = humiditymesure;
      mesTps[cptmesure] = timeClient.getEpochTime();
      humidmoy = (humidmoy + humiditymesure) / 2;
      tempmoy = (tempmoy + temperaturemesure) / 2;
      Serial.println(F("*********************"));
        Serial.print(F("|N  |T  |H  |s  |n°"));
      Serial.println(mesTps[cptmesure]);
      for (int cptmestemp = 0; cptmestemp <= cptmesure; cptmestemp++)
      {
        Serial.print(F("| "));
        Serial.print(cptmestemp);
        Serial.print(F("| "));
        Serial.print(temperature[cptmestemp]);
        Serial.print(F("| "));
        Serial.print(humidity[cptmestemp]);
        Serial.print(F("| "));
        Serial.print( mesTps[cptmestemp] /*- mesTps[0]*/);
        //Serial.println(F("|"));
        if (mesTps[cptmestemp]<FirstTps) //GESTION d'ERRUER MEMOIRE !
        {
          Serial.print(F("<ERREUR MEMOIRE:"));
          if (cptmestemp <= cptmesure)
          {
            mesTps[cptmestemp]=max(FirstTps,mesTps[cptmestemp+1]);
          }
          else
          {
            mesTps[cptmestemp]=max(FirstTps,mesTps[cptmestemp-1]);  
          }
          Serial.print( mesTps[cptmestemp]);
        }
        Serial.println(F("|"));
      }
      if (tempmin > temperaturemesure) tempmin = temperaturemesure;
      if (tempmax < temperaturemesure) tempmax = temperaturemesure;
      if (humidmin > humiditymesure) humidmin = humiditymesure;
      if (humidmax < humiditymesure) humidmax = humiditymesure;
    }
  }
  SLHeureLue = timeClient.getFormattedTime();
  if (SLHeure != SLHeureLue)
  {
    SLHeure = SLHeureLue;
    if ( (timeClient.getSeconds() % 5 ) == 0)
    {
      Serial.println(timeClient.getFormattedTime());
    }
    //clignotte pin en sortie:
    //pinMode(D0, OUTPUT);
    //digitalWrite(D0, timeClient.getSeconds() % 1);
    if ( (timeClient.getSeconds() % 60 ) == 0)
    {
      if (cptmesure > 2)
      EPD_Time();
    }
  }
}

//                  _____                               _   _
//                 |  __ \                             | | (_)
//                 | |__) |   ___   _ __ ___    _ __   | |  _   _ __
//                 |  _  /   / _ \ | '_ ` _ \  | '_ \  | | | | | '__|
//                 | | \ \  |  __/ | | | | | | | |_) | | | | | | |
//                 |_|  \_\  \___| |_| |_| |_| | .__/  |_| |_| |_|
//                                             | |
//                                             |_|


void Remplir()
{
  unsigned int tpsrandom = random(60, 2678400 * 12); //1min à 12mois
  if (DEBUGKL)
  {
    Serial.print(F("Aleatoire sur:"));
    Serial.print(tpsrandom);
    Serial.println(F("secondes"));
  }
  for (cptmesure = 0; cptmesure <= (100 % NbDataMemory ); cptmesure++)
  {
    temperaturemesure = cptmesure % 100;
    humiditymesure =  cptmesure %100;
    //lasttemp = temperaturemesure;
    //lasthumi = humiditymesure;
    temperature[cptmesure] = temperaturemesure;
    humidity[cptmesure] = humiditymesure;
    mesTps[cptmesure] = timeClient.getEpochTime() - (unsigned int)((unsigned int)tpsrandom / (unsigned int)(NbDataMemory - 3) * (unsigned int)((unsigned int)((100 % NbDataMemory ) - cptmesure)));
    if (tempmin > temperaturemesure) tempmin = temperaturemesure;
    if (tempmax < temperaturemesure) tempmax = temperaturemesure;
    if (humidmin > humiditymesure) humidmin = humiditymesure;
    if (humidmax < humiditymesure) humidmax = humiditymesure;
    Serial.print(cptmesure);
    Serial.print(":");
    Serial.print(temperature[cptmesure]);
    Serial.print(":");
    Serial.print(humidity[cptmesure]);
    Serial.print(":");
    Serial.println(mesTps[cptmesure]);
  }
  FirstTps=mesTps[0];
  cptmesure = (100 % NbDataMemory );
  //mesTps[cptmesure] = mesTps[cptmesure]; //(mesmonth[cptmesure]*24*3600*31)+(mesday[cptmesure]*24*3600)+(meshour[cptmesure]*3600)+(mesminute[cptmesure]*60)+messecond[cptmesure];
  //mesTps[0] = mesTps[0]; //mesTps[cptmesure];
    Serial.print(F("tempmax:"));
    Serial.println(tempmax);
    Serial.print(F("temp:"));
    Serial.println(temperature[cptmesure]);
    Serial.print(F("tempmin:"));
    Serial.println(tempmin);
    Serial.print(F("mesTps[cptmesure]:"));
    Serial.println(mesTps[cptmesure]);
    Serial.print(F("mesTps[0]:"));
    Serial.println(mesTps[0]);
  server.send(200, "text/plain", "<head><title>Mesure temperature</title><meta http-equiv=\"refresh\" content=\"10; url=/\" ><meta charset = \"utf-8\" /></head><body><h1>ES2COM - Web Server ESP12E</h1><p>Remplir<BR>");
  //delay(10000);
  //EPD_Time();
  //delay(10000);
}


//              _____           _   _
//             |_   _|         (_) | |
//               | |    _ __    _  | |_
//               | |   | '_ \  | | | __|
//              _| |_  | | | | | | | |_
//             |_____| |_| |_| |_|  \__|
//

void EPD_Init()
{
  EPD_dispIndex = ((int)server.arg(0)[0] - 'a') +  (((int)server.arg(0)[1] - 'a') << 4);
  // Print log message: initialization of e-Paper (e-Paper's type)
  if (DEBUGKL) Serial.printf("EPD %s\r\n", EPD_dispMass[EPD_dispIndex].title);

  // Initialization
  EPD_dispInit();
  server.send(200, "text/plain", "Init ok\r\n");
}

//            _                            _
//           | |                          | |
//           | |        ___     __ _    __| |
//           | |       / _ \   / _` |  / _` |
//           | |____  | (_) | | (_| | | (_| |
//           |______|  \___/   \__,_|  \__,_|
//

void EPD_Load()
{
  //server.arg(0) = data+data.length+'LOAD'
  String p = server.arg(0);
  if (p.endsWith("LOAD"))
  {
    int index = p.length() - 8;
    int L = ((int)p[index] - 'a') + (((int)p[index + 1] - 'a') << 4) + (((int)p[index + 2] - 'a') << 8) + (((int)p[index + 3] - 'a') << 12);
    if (L == (p.length() - 8))
    {
      if (DEBUGKL) Serial.println("LOAD");
      // if there is loading function for current channel (black or red)
      // Load data into the e-Paper
      if (EPD_dispLoad != 0) EPD_dispLoad();
    }
  }
  server.send(200, "text/plain", "Load ok\r\n");
}

//              _   _                 _
//             | \ | |               | |
//             |  \| |   ___  __  __ | |_
//             | . ` |  / _ \ \ \/ / | __|
//             | |\  | |  __/  >  <  | |_
//             |_| \_|  \___| /_/\_\  \__|
//
//
void EPD_Next()
{
  if (DEBUGKL) Serial.println(F("NEXT"));

  // Instruction code for for writting data into
  // e-Paper's memory
  int code = EPD_dispMass[EPD_dispIndex].next;

  // If the instruction code isn't '-1', then...
  if (code != -1)
  {
    // Do the selection of the next data channel
    EPD_SendCommand(code);
    delay(2);
  }
  // Setup the function for loading choosen channel's data
  EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

  server.send(200, "text/plain", "Next ok\r\n");
}


//               _____   _
//              / ____| | |
//             | (___   | |__     ___   __      __
//              \___ \  | '_ \   / _ \  \ \ /\ / /
//              ____) | | | | | | (_) |  \ V  V /
//             |_____/  |_| |_|  \___/    \_/\_/
//

void EPD_Show()
{
  if (DEBUGKL) Serial.println(F("SHOW"));
  // Show results and Sleep
  EPD_dispMass[EPD_dispIndex].show();
  server.send(200, "text/plain", "Show ok\r\n");
}
//                 .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.
//                | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |
//                | |      __      | || |  _________   | || |  _________   | || |     _____    | || |     ______   | || |  ____  ____  | || |  _________   | |
//                | |     /  \     | || | |_   ___  |  | || | |_   ___  |  | || |    |_   _|   | || |   .' ___  |  | || | |_   ||   _| | || | |_   ___  |  | |
//                | |    / /\ \    | || |   | |_  \_|  | || |   | |_  \_|  | || |      | |     | || |  / .'   \_|  | || |   | |__| |   | || |   | |_  \_|  | |
//                | |   / ____ \   | || |   |  _|      | || |   |  _|      | || |      | |     | || |  | |         | || |   |  __  |   | || |   |  _|  _   | |
//                | | _/ /    \ \_ | || |  _| |_       | || |  _| |_       | || |     _| |_    | || |  \ `.___.'\  | || |  _| |  | |_  | || |  _| |___/ |  | |
//                | ||____|  |____|| || | |_____|      | || | |_____|      | || |    |_____|   | || |   `._____.'  | || | |____||____| | || | |_________|  | |
//                | |              | || |              | || |              | || |              | || |              | || |              | || |              | |
//                | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |
//                 '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'
//
void EPD_Affiche(String Titre, String Texte)
{
  if (DEBUGKL) Serial.println(F("Affiche:"));
  server.send(200, "text/plain", "Affiche:\r\n" + Titre+Texte);
  Paint_NewImage(ImageEpaper, epaperwidth, epaperheight, 0, WHITE);
  Paint_SelectImage(ImageEpaper);
  //Noir:
  Paint_Clear(WHITE);
  int cpt=0;
  int maxlen=400/8;
  int lentext=Texte.length();
  while((maxlen*cpt)<lentext)
  {
    Paint_DrawString_EN(1,cpt*12+20,Texte.substring((maxlen*cpt),maxlen).c_str(), &Font12, WHITE, BLACK);
    cpt=cpt+1;
  }
  //EPD_showA();
  EPD_Init_4in2b();
  EPD_SendCommand(0x10); // SEND BLACK
  EPD_loadImg(ImageEpaper, ImagesizeEpaper);
  //Rouge:
  Paint_Clear(WHITE);
  lentext=Titre.length();
  maxlen=400/10;
  cpt=0;
  while((maxlen*cpt)<lentext)
  {
    Paint_DrawString_EN(1,cpt*20,Titre.substring((maxlen*cpt), maxlen).c_str(), &Font20, WHITE, BLACK);
    cpt=cpt+1;
  }
  EPD_SendCommand(0x13); // SEND RED
  EPD_loadImg(ImageEpaper, ImagesizeEpaper);
  if (DEBUGKL) Serial.print(F(":SENT"));
  //Turn On display:
  EPD_SendCommand(0x12); // DISPLAY_TURN ON
  delay(100);
  if (DEBUGKL) Serial.println(F(":DRAWNED"));
  //(ImageEpaper, ImageEpaper);
}
//
//            ████████╗██╗███╗   ███╗ ███████╗
//            ╚══██╔══╝██║████╗ ████║██╔════╝
//                ██║   ██║██╔████╔██║  █████╗
//                ██║   ██║██║╚██╔╝██║  ██╔══╝
//                ██║   ██║██║ ╚═╝  ██║  ███████╗
//                ╚═╝   ╚═╝╚═╝       ╚═╝  ╚══════╝
//
void EPD_Time()
{
  if (DEBUGKL) Serial.println(F("Time:"));
  // Show results and Sleep
  //EPD_dispMass[EPD_dispIndex].show();
  server.send(200, "text/plain", "Show time:\r\n");
  if (DEBUGKL) Serial.println(F("CLOCK:"));
  if (DEBUGKL) Serial.println(timeClient.getFormattedTime());
  //int epaperwidth=400;
  //int epaperheight=300;
  //UBYTE *ImageEpaper; // black \Red or Yellow
  Paint_NewImage(ImageEpaper, epaperwidth, epaperheight, 0, WHITE);
  //Select Image
  // *******************************************************************************************************
  // *******************************************************************************************************
  // *******************************************************************************************************
  //                         888888ba  dP                   dP
  //                         88    `8b 88                   88
  //                        a88aaaa8P' 88 .d8888b. .d8888b. 88  .dP
  //                         88   `8b. 88 88'  `88 88'  `"" 88888"
  //                         88    .88 88 88.  .88 88.  ... 88  `8b.
  //                         88888888P dP `88888P8 `88888P' dP   `YP
  // *******************************************************************************************************
  // *******************************************************************************************************
  // *******************************************************************************************************
  Paint_SelectImage(ImageEpaper);
  Paint_Clear(WHITE);
  //Cadre:
  if (DEBUGKL) Serial.println(F("BLACK"));
  int cadreposx = 0;
  int cadreposy = 0;
  int cadrelenx = 400;
  int cadreleny = 300;
  int cadrelarg = 5;
  int souscadrehaut = 32;
  int barregraphtpshgaut = 10;
  //void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
  //cadre du tour:
  if (DEBUGKL)  Serial.println(F("tour"));
  Paint_DrawRectangle(cadreposx, cadreposy, cadrelenx, cadreposy + cadrelarg, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //HAUT
  Paint_DrawRectangle(cadreposx, cadreleny - cadrelarg, cadrelenx, cadreleny, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //BAS
  Paint_DrawRectangle(cadreposx, cadreposy, cadreposx + cadrelarg, cadreleny, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //GAUCHE
  Paint_DrawRectangle(cadrelenx - cadrelarg, cadreposy, cadrelenx, cadreleny, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); //DROIT
  //bordures du bas:
  if (DEBUGKL) Serial.println(F("bordure"));
  Paint_DrawRectangle(cadreposx, cadreleny - cadrelarg - souscadrehaut, cadrelenx, cadreleny - souscadrehaut, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  //barregraph:
  if (DEBUGKL) Serial.println(F("bargraph"));
  Paint_DrawRectangle(cadreposx, cadreleny - cadrelarg - souscadrehaut - barregraphtpshgaut, cadrelenx, cadreleny - souscadrehaut - barregraphtpshgaut, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  //heure
  if (DEBUGKL) Serial.println(F("heure"));
  Paint_DrawString_EN(12,  cadreleny - cadrelarg - souscadrehaut + 16, timeClient.getFormattedTime().substring(0, 5).c_str(), &Font20, WHITE, BLACK);
  if (DEBUGKL) Serial.println(F("ip"));
  Paint_DrawString_EN(12 + (16 * 5) + 10, cadreleny - cadrelarg - souscadrehaut + 8, WiFi.localIP().toString().c_str(), &Font8, WHITE, BLACK);
  if (DEBUGKL) Serial.println(F("temp"));
  sprintf(Aff, "T=%.2fC H=%02d%%", temperaturemesure, humiditymesure);
  Paint_DrawString_EN(12 + (16 * 5) + (6 * 16) - 8, cadreleny - cadrelarg - souscadrehaut + 16, Aff, &Font20, WHITE, BLACK);
  Serial.println(F("bas"));
  Paint_DrawRectangle(cadreposx + ((16 * 5) + 10), cadreleny - cadrelarg, cadreposx + ((16 * 5) + 10) + cadrelarg, cadreleny, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawRectangle(cadreposx + ((16 * 5) + 10 + (8 * 16) + 10), cadreleny - cadrelarg, cadreposx + ((16 * 5) + 10) + cadrelarg, cadreleny, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  //draw clock
  //Dimentions et caractèristiques:
  int clklarg = 68;
  int clktailletickCINQ = clklarg / 13;
  int clktailletickQUART = clklarg / 10;
  int clktailleHEUR = clklarg * 3 / 5;
  int clktailleMINUTE = clklarg * 4 / 5;
  int clkposX = (cadrelarg * 2) + clklarg; //centre horloge
  int clkposY = (cadrelarg * 2) + clklarg;
  int fontsize = 8;
  //logo
  if (DEBUGKL) Serial.println(F("logo"));
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2), "ES com", &Font16, WHITE, BLACK);
  //
  Paint_DrawCircle(clkposX, clkposY, clklarg, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
  Paint_DrawCircle(clkposX, clkposY, clktailletickQUART, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  int cpt = 0;
  int tick = 12;
  boolean hourdrawned = false;
  if (DEBUGKL) Serial.println(F("clock"));
  for (cpt = 60; cpt >= 0; cpt = cpt - 1)
  {
    float x1 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART));
    float y1 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART));
    float x2 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clklarg);
    float y2 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clklarg);
    Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    if ((cpt % 5) == 0)
    {
      Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
      float x3 = 0;
      float y3 = 0;
      if (tick < 10)
      {
        x3 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART - (clktailletickCINQ))) - (float)(fontsize / 2);
        y3 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART - (clktailletickCINQ))) - (float)(fontsize / 2);
      }
      else
      {
        x3 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART - (clktailletickCINQ))) - (float)(fontsize / 2);
        y3 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART - (clktailletickCINQ))) - (float)(fontsize / 2);
      }
      Paint_DrawNum(x3, y3, tick, &Font8, BLACK, WHITE);
      if (tick == 0) tick = 13;
      tick = tick - 1;
    }
    if ((cpt % 15) == 0)
    {
      Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    }
    x1 = clkposX;
    y1 = clkposY;
    x2 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clktailleMINUTE);
    y2 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clktailleMINUTE);
    if (cpt == timeClient.getMinutes())
    {
      Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    }
    /*
      x2 = (float)clkposX + (float)(cos((float)TWO_PI / (float)12 * (float)(cpt+((float)1/(float)60*(float)(timeClient.getMinutes()+1))) - (float)HALF_PI) * (float)clktailleHEUR);
      y2 = (float)clkposY + (float)(sin((float)TWO_PI / (float)12 * (float)(cpt+((float)1/(float)60*(float)(timeClient.getMinutes()+1))) - (float)HALF_PI) * (float)clktailleHEUR);
      if (cpt == (timeClient.getHours() % 12))
      {
      if (!hourdrawned)
      {
        Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
        hourdrawned=true;
      }
      }
    */
  }
  if (DEBUGKL) Serial.println(F("remplissage graph"));
  DessinGraph(150, 0, 250, 254 / 2, 0, 0); //TEMP
  DessinGraph(150, 254 / 2, 250, 254 / 2, 1, 0); //HUMDID
  //tableau de min max moy
  if (DEBUGKL) Serial.println(F("min max moy"));
  sprintf(Aff, "min %.2f %02d ", tempmin, humidmin);
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 16, Aff, &Font12, WHITE, BLACK);
  sprintf(Aff, "moy %.2f %02d", tempmoy, humidmoy);
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 16 + 12, Aff, &Font12, WHITE, BLACK);
  sprintf(Aff, "    %.2f %02d", temperaturemesure, humiditymesure);
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 16 + 12 + 12, Aff, &Font12, WHITE, BLACK);
  sprintf(Aff, "max %.2f %02d", tempmax, humidmax);
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 16 + 12 + 12 + 12, Aff, &Font12, WHITE, BLACK);
  sprintf(Aff, "%04d %s", cptmesure % 9999, timeClient.getFormattedTime().c_str());
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 82, Aff, &Font12, WHITE, BLACK);
  // Paint_DrawTime(20, 270, timeClient,  &Font16, BLACK, WHITE);
  //EPD_showA();
  EPD_Init_4in2b();
  EPD_SendCommand(0x10); // SEND BLACK
  EPD_loadImg(ImageEpaper, ImagesizeEpaper);
  if (DEBUGKL) Serial.print(F(":SENT"));
  // *******************************************************************************************************
  // *******************************************************************************************************
  // *******************************************************************************************************
  //                                         ____     ___  ___
  //                                        |    \   /  _]|   \  
  //                                        |  D  ) /  [_ |    \ 
  //                                        |    / |    _]|  D  |
  //                                        |    \ |   [_ |     |
  //                                        |  .  \|     ||     |
  //                                        |__|\_||_____||_____|
  //
  // *******************************************************************************************************
  // *******************************************************************************************************
  // *******************************************************************************************************
  Paint_Clear(WHITE);
  if (DEBUGKL) Serial.print(F("/WHITE"));
  //Paint_DrawRectangle(cadreposx, cadreleny-cadrelarg-souscadrehaut-barregraphtpshgaut,cadrelenx, cadreleny-souscadrehaut-barregraphtpshgaut, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  //logo
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2), "  2", &Font16, WHITE, BLACK);
  Paint_DrawString_EN(12,  cadreleny - cadrelarg - souscadrehaut + 16, "  :", &Font20, WHITE, BLACK);
  Paint_DrawString_EN(12 + (16 * 5) + 10, cadreleny - cadrelarg - souscadrehaut + 16, ssid, &Font8, WHITE, BLACK);
  int upline = 255;
  int baseline = 258;
  //barregraph:
  Paint_DrawRectangle(cadreposx, cadreleny - souscadrehaut - barregraphtpshgaut, cadreposx + (int)((long)cadrelenx / (long)60 * (long)timeClient.getMinutes()), cadreleny - souscadrehaut - barregraphtpshgaut + cadrelarg, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  hourdrawned = false;
  for (cpt = 59; cpt >= 0; cpt = cpt - 1)
  {
    float x1 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART));
    float y1 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)(clklarg - clktailletickQUART));
    float x2 = (float)clkposX + (float)(cos((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clklarg);
    float y2 = (float)clkposY + (float)(sin((float)TWO_PI / (float)60 * (float)cpt - (float)HALF_PI) * (float)clklarg);
    //Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    if ((cpt % 5) == 0)
    {
      Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    }

    if (cpt == (timeClient.getHours() % 12))
    {
      x1 = (float)clkposX + (float)(cos((float)TWO_PI / (float)12 * (float)(cpt + ((float)1 / (float)60 * (float)timeClient.getMinutes())) - (float)HALF_PI) * (float)(clktailletickQUART));
      y1 = (float)clkposY + (float)(sin((float)TWO_PI / (float)12 * (float)(cpt + ((float)1 / (float)60 * (float)timeClient.getMinutes())) - (float)HALF_PI) * (float)(clktailletickQUART));
      x2 = (float)clkposX + (float)(cos((float)TWO_PI / (float)12 * (float)(cpt + ((float)1 / (float)60 * (float)timeClient.getMinutes())) - (float)HALF_PI) * (float)clktailleHEUR);
      y2 = (float)clkposY + (float)(sin((float)TWO_PI / (float)12 * (float)(cpt + ((float)1 / (float)60 * (float)timeClient.getMinutes())) - (float)HALF_PI) * (float)clktailleHEUR);
      if (!hourdrawned)
      {
        Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
        hourdrawned = true;
      }
    }
  }
  DessinGraph(150, 0, 250, 254 / 2, 0, 1); //TEMP
  DessinGraph(150, 254 / 2, 250, 254 / 2, 1, 1); //HUMDID
  // Paint_DrawTime(20, 270, timeClient,  &Font16, BLACK, WHITE);
  sprintf(Aff, "mes %.2f %02d", temperaturemesure, humiditymesure);
  if (DEBUGKL) Serial.println(Aff);
  Paint_DrawString_EN(cadreposx + cadrelarg + 2, cadreposy + cadrelarg + 2 + 16 + (clklarg * 2) + 16 + 12 + 12, Aff, &Font12, WHITE, BLACK);
  EPD_SendCommand(0x13); // SEND RED
  EPD_loadImg(ImageEpaper, ImagesizeEpaper);
  if (DEBUGKL) Serial.print(F(":SENT"));
  //Turn On display:
  EPD_SendCommand(0x12); // DISPLAY_TURN ON
  delay(100);
  if (DEBUGKL) Serial.println(F(":DRAWNED"));
  //(ImageEpaper, ImageEpaper);
}
//              _                           _   _          _   _           _     ______                               _
//             | |                         | | | |        | \ | |         | |   |  ____|                             | |
//             | |__     __ _   _ __     __| | | |   ___  |  \| |   ___   | |_  | |__      ___    _   _   _ __     __| |
//             | '_ \   / _` | | '_ \   / _` | | |  / _ \ | . ` |  / _ \  | __| |  __|    / _ \  | | | | | '_ \   / _` |
//             | | | | | (_| | | | | | | (_| | | | |  __/ | |\  | | (_) | | |_  | |      | (_) | | |_| | | | | | | (_| |
//             |_| |_|  \__,_| |_| |_|  \__,_| |_|  \___| |_| \_|  \___/   \__| |_|       \___/   \__,_| |_| |_|  \__,_|
//
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(200, "text/plain", message);
  if (DEBUGKL) Serial.print("Unknown URI: ");
  if (DEBUGKL) Serial.println(server.uri());
}
// **********************************************************************************************************
// **********************************************************************************************************
// **********************************************************************************************************
// **********************************************************************************************************
//         ______   _______  _______  _______  ___   __    _  _______  ______    _______  _______  __   __
//        |      | |       ||       ||       ||   | |  |  | ||       ||    _ |  |   _   ||       ||  | |  |
//        |  _    ||    ___||  _____||  _____||   | |   |_| ||    ___||   | ||  |  |_|  ||    _  ||  |_|  |
//        | | |   ||   |___ | |_____ | |_____ |   | |       ||   | __ |   |_||_ |       ||   |_| ||       |
//        | |_|   ||    ___||_____  ||_____  ||   | |  _    ||   ||  ||    __  ||       ||    ___||       |
//        |       ||   |___  _____| | _____| ||   | | | |   ||   |_| ||   |  | ||   _   ||   |    |   _   |
//        |______| |_______||_______||_______||___| |_|  |__||_______||___|  |_||__| |__||___|    |__| |__|
//
// **********************************************************************************************************
// **********************************************************************************************************
// **********************************************************************************************************
// **********************************************************************************************************
void DessinGraph(int posx, int posy, int taillex, int tailley, int typemesure, byte modecolor)
{
  //int traceur = 0;
  double x, y, x2, y2; //, x3, y3;
  //int cptlue = 0;
  //int cptx, cpty;
  unsigned long echellex;
  unsigned long sautgrilletps = 0;
  //cadre graph
  //grille de temps:
  echellex = (double)min((double)mesTps[cptmesure], (double)max((double)0, (double)mesTps[cptmesure] - (double)mesTps[0] + (double)2));
  sautgrilletps = 15;
  if (DEBUGKL) Serial.println(F("CADRE"));
  if (modecolor == 0)
  {
    Paint_DrawRectangle(posx , posy, posx + taillex, posy + tailley, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
  }
  //aff mesures
  if (typemesure == 0) //temperature
  {
    ValeursAff(tempmin, temperature[cptmesure], tempmax, "T(C)", posx, posy, taillex, tailley, modecolor);
  }
  else if (typemesure == 1) //humidité
  {
    ValeursAff(humidmin, humidity[cptmesure], humidmax, "H(%)", posx, posy, taillex, tailley, modecolor);
  }
  //on enleve 16 en x pour affichage des valeurs
  if (DEBUGKL) Serial.println(F("Grilletps"));
  if (modecolor == 0)
  {
    if (echellex >= 2678400) //>1mois
    {
      sautgrilletps = 2678400;
      Paint_DrawString_EN(posx + taillex - 12, posy + tailley - 4, "A", &Font12, BLACK, WHITE);
      if (DEBUGKL) Serial.println(F("A"));
    }
    else if (echellex >= 604800) //>1semaine
    {
      sautgrilletps = 604800;
      Paint_DrawString_EN(posx + taillex - 12, posy + tailley - 4, "M", &Font12, BLACK, WHITE);
      if (DEBUGKL) Serial.println(F("M"));
    }
    else if (echellex >= 86400) //>1jour
    {
      sautgrilletps = 86400;
      Paint_DrawString_EN(posx + taillex - 12, posy + tailley - 4, "J", &Font12, BLACK, WHITE);
      if (DEBUGKL) Serial.println(F("J"));
    }
    else if (echellex >= 3600) //>1h
    {
      sautgrilletps = 3600;
      Paint_DrawString_EN(posx + taillex - 12, posy + tailley - 4, "h", &Font12, BLACK, WHITE);
      if (DEBUGKL) Serial.println(F("h"));
    }
    else
    {
      sautgrilletps = 60;
      Paint_DrawString_EN(posx + taillex - 12, posy + tailley - 4, "m", &Font12, BLACK, WHITE);
      if (DEBUGKL) Serial.println(F("m"));
    }
  }
  //
  //   _____ _____  _____ _      _      ______  __   __
  //  / ____|  __ \|_   _| |    | |    |  ____| \ \ / /
  // | |  __| |__) | | | | |    | |    | |__     \ V /
  // | | |_ |  _  /  | | | |    | |    |  __|     > <
  // | |__| | | \ \ _| |_| |____| |____| |____   / . \ 
  //  \_____|_|  \_\_____|______|______|______| /_/ \_\
  //
  if (modecolor == 0)
  {
    if (DEBUGKL) Serial.println(F("grillex"));
    if (DEBUGKL) Serial.print("echellex:");
    if (DEBUGKL) Serial.println(echellex);
    for ( unsigned long grille = 0; grille <= (echellex + 1); grille = grille + 1)
    {
      x = (double)posx + (double)((double)taillex * (double)grille / (double)max((double)1, (double)echellex));
      x = (double)max((double)min((double)x, (double)posx + (double)taillex), (double)posx);
      y = (double)posy + (double)9;
      y = (double)max((double)min((double)y, (double)posy + (double)tailley), (double)posy);
      y2 = (double)posy + (double)tailley - (double)9;
      y2 = (double)max((double)min((double)y2, (double)posy + (double) tailley), (double)posy);
      if (((mesTps[0] + grille) % sautgrilletps) == 0) //on trace
      {
        if (DEBUGKL) Serial.print("grille:");
        if (DEBUGKL) Serial.println(grille);
        if (DEBUGKL) Serial.print("x:");
        if (DEBUGKL) Serial.println((int)x);
        if (DEBUGKL) Serial.print("y:");
        if (DEBUGKL) Serial.println((int)y);
        if (DEBUGKL) Serial.print("y2:");
        if (DEBUGKL) Serial.println((int)y2);
        if (echellex >= 2678400) //>1mois
        {
          Paint_DrawLine((int)x, (int)y, (int)x, (int)y2, BLACK, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
          Paint_DrawNum((int)x, (int)y2, readYear((unsigned int)((unsigned int)mesTps[0] + (unsigned int)grille)), &Font12, BLACK, WHITE);
        }
        else if (echellex >= 604800) //>1semaine
        {
          Paint_DrawLine((int)x, (int)y, (int)x, (int)y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
          Paint_DrawNum((int)x, (int)y2, readMonth((unsigned int)((unsigned int)mesTps[0] + (unsigned int)grille)), &Font12, BLACK, WHITE);
        }
        else if (echellex >= 86400) //>1jour
        {
          Paint_DrawLine((int)x, (int)y, (int)x, (int)y2, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
          Paint_DrawNum((int)x, (int)y2, readDay((unsigned int)((unsigned int)mesTps[0] + (unsigned int)grille)), &Font12, BLACK, WHITE);
        }
        else if (echellex >= 3600) //>1h
        {
          Paint_DrawLine((int)x, (int)y,(int) x, (int)y2, BLACK, DOT_PIXEL_2X2, LINE_STYLE_DOTTED);
          Paint_DrawNum((int)x, (int)y2, readHours((unsigned int)((unsigned int)mesTps[0] + (unsigned int)grille)), &Font12, BLACK, WHITE);
        }
        else
        {
          Paint_DrawLine((int)x, (int)y, (int)x, (int)y2, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
          if ((((mesTps[0] + grille) % sautgrilletps) % 15) == 0) //on trace
          {
            Paint_DrawNum((int)x, (int)y2, readMinutes((unsigned int)((unsigned int)mesTps[0] + (unsigned int)grille)), &Font12, BLACK, WHITE);
          }
        }
      }
    }
  }
  //
  //
  //   _____ _____  _____ _      _      ______  __     __
  //  / ____|  __ \|_   _| |    | |    |  ____| \ \   / /
  // | |  __| |__) | | | | |    | |    | |__     \ \_/ /
  // | | |_ |  _  /  | | | |    | |    |  __|     \   /
  // | |__| | | \ \ _| |_| |____| |____| |____     | |
  //  \_____|_|  \_\_____|______|______|______|    |_|
  //
  //
  if (DEBUGKL) Serial.println(F("grilley"));
  if (modecolor == 0)
  {
    if (typemesure == 0) //temperature
    {
      GrilleAff(tempmin,  tempmax, posx, posy, taillex, tailley, modecolor);
    }
    else
    {
      GrilleAff(humidmin,  humidmax, posx, posy, taillex, tailley, modecolor);
    }
  }
  //tracer:
  if (DEBUGKL) Serial.println("Tracer:");
  //Serial.print("cptmesure:");
  //Serial.println(cptmesure);
  if (typemesure == 0) //temperature
  {
    TraceAff(tempmin, temperature, tempmax, 30, "AlmTmp", posx, posy, taillex, tailley, modecolor);
  }
  else
  {
    TraceAff(humidmin, humidity, humidmax, 90, "AlmHum", posx, posy, taillex, tailley, modecolor);
  }
}

/***
                _____                _____                    _____
               /\    \              /\    \                  /\    \
              /::\____\            /::\    \                /::\    \
             /::::|   |            \:::\    \              /::::\    \
            /:::::|   |             \:::\    \            /::::::\    \
           /::::::|   |              \:::\    \          /:::/\:::\    \
          /:::/|::|   |               \:::\    \        /:::/__\:::\    \
         /:::/ |::|   |               /::::\    \      /::::\   \:::\    \
        /:::/  |::|   | _____        /::::::\    \    /::::::\   \:::\    \
       /:::/   |::|   |/\    \      /:::/\:::\    \  /:::/\:::\   \:::\____\
      /:: /    |::|   /::\____\    /:::/  \:::\____\/:::/  \:::\   \:::|    |
      \::/    /|::|  /:::/    /   /:::/    \::/    /\::/    \:::\  /:::|____|
       \/____/ |::| /:::/    /   /:::/    / \/____/  \/_____/\:::\/:::/    /
               |::|/:::/    /   /:::/    /                    \::::::/    /
               |::::::/    /   /:::/    /                      \::::/    /
               |:::::/    /    \::/    /                        \::/____/
               |::::/    /      \/____/                          ~~
               /:::/    /
              /:::/    /
              \::/    /
               \/____/

*/
int getYear()
{
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int year = ti->tm_year + 1900;

  return year;
}

int getMonth()
{
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int month = (ti->tm_mon + 1) < 10 ? 0 + (ti->tm_mon + 1) : (ti->tm_mon + 1);

  return month;
}

int getDay()
{
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  int day = (ti->tm_mday) < 10 ? 0 + (ti->tm_mday) : (ti->tm_mday);

  return day;
}
///  NTP ADDON
int readYear(unsigned int timeread)
{
  time_t rawtime = timeread;
  struct tm * ti;
  ti = localtime (&rawtime);
  int year = ti->tm_year + 1900;

  return year;
}

int readMonth(unsigned int timeread)
{
  time_t rawtime = timeread;
  struct tm * ti;
  ti = localtime (&rawtime);
  int month = (ti->tm_mon + 1) < 10 ? 0 + (ti->tm_mon + 1) : (ti->tm_mon + 1);

  return month;
}

int readDay(unsigned int timeread)
{
  time_t rawtime = timeread;
  struct tm * ti;
  ti = localtime (&rawtime);
  int day = (ti->tm_mday) < 10 ? 0 + (ti->tm_mday) : (ti->tm_mday);

  return day;
}
int readDayOfWeek(unsigned int timeread)
{
  return (((timeread  / 86400L) + 4 ) % 7); //0 is Sunday
}
int readHours(unsigned int timeread)
{
  return ((timeread  % 86400L) / 3600);
}
int readMinutes(unsigned int timeread)
{
  return ((timeread % 3600) / 60);
}
int readSeconds(unsigned int timeread)
{
  return (timeread % 60);
}
String getTimeStampString()
{
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);

  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);

  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);

  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  uint8_t hours = ti->tm_hour;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  uint8_t minutes = ti->tm_min;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  uint8_t seconds = ti->tm_sec;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return yearStr + "-" + monthStr + "-" + dayStr + " " +
         hoursStr + ":" + minuteStr + ":" + secondStr;
}
String readTimeStampString(unsigned int timeread)
{
  time_t rawtime = timeread;
  struct tm * ti;
  ti = localtime (&rawtime);

  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);

  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);

  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  uint8_t hours = ti->tm_hour;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  uint8_t minutes = ti->tm_min;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  uint8_t seconds = ti->tm_sec;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return yearStr + "-" + monthStr + "-" + dayStr + " " + hoursStr + ":" + minuteStr + ":" + secondStr;
}
// FONCTIONS DE DESSIN:
void ValeursAff(double valmin, double valmes, double valmax, String DispAff, int posx, int posy, int taillex, int tailley, byte modecolor)
{
  //Serial.println("Temp:");
  double echellex = (double)min((double)mesTps[cptmesure], max((double)0, ((double)mesTps[cptmesure] - (double)mesTps[0] + (double)2)));
  double echelley = max((double)2, (double)valmax - (double)valmin);
  //
  //max
  if (modecolor == 0)
  {
    Paint_DrawString_EN(posx + 2, posy + 3, DispAff.c_str(), &Font16, WHITE, BLACK);
    //max
    double y2 = (double)posy + (double)tailley - (double)((double)tailley * (double)valmax / (double)echelley) - (double)4;
    double y = (double)max((double)min((double)y2 - (double)10, (double)posy + (double)tailley), (double)posy);
    Paint_DrawNum(posx + taillex - 16 - 3, y, valmin, &Font12, BLACK, WHITE);
    //min
    y2 = (double)posy + (double)tailley - (double)((double)tailley * (double)valmin / (double)echelley) - (double)4;
    y = (double)max((double)min((double)y2 - (double)10, (double)posy + (double)tailley), (double)posy);
    Paint_DrawNum(posx + taillex - 16 - 3, y, valmin, &Font12, BLACK, WHITE);
  }
  //val
  if (modecolor == 1)
  {
    //val
    double y2 = (double)posy + (double)tailley - (double)((double)tailley * (double)valmes / (int)echelley) - (double)4;
    double y = (double)max((double)min((double)y2 - (double)10, (double)posy + (double)tailley), (double)posy);
    Paint_DrawNum(posx + taillex - 16 - 3, y, valmes, &Font12, BLACK, WHITE);
  }
}
void GrilleAff(double valmin, double valmax, int posx, int posy, int taillex, int tailley, byte modecolor)
{
  //on enleve 16 en x pour affichage des valeurs
  taillex = taillex - 20; //pour les valeurs à droite et en bas
  tailley = tailley - 10;
  double echelley = max((double)2, (double)valmax - (double)valmin);
  for (int grille = valmin; grille <= valmax; grille = grille + 1)
  {
    double y2 = (double)posy + (double)tailley - (double)((double)tailley * (double)(grille) / (double)echelley);
    double y = (double)max((double)min((double)y, (double)posy + (double)tailley), (double)posy);
    if (((int)grille % (int)10) == (int)0) //on trace
    {
      Paint_DrawLine(posx, y, posx + taillex, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }
    else if (((int)grille % (int)5) == (int)0) //on trace
    {
      if (echelley <= 25)
      {
        Paint_DrawLine(posx, y, posx + taillex, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
      }
      else if (((int)grille % (int)25) == (int)0) //on trace
      {
        Paint_DrawLine(posx, y, posx + taillex, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
      }
    }
  }
}
void TraceAff(double valmin, float *valmes, double valmax, double Seuil, String DispAff, int posx, int posy, int taillex, int tailley, byte modecolor)
{
  //on enleve 16 en x pour affichage des valeurs
  taillex = taillex - 20; //pour les valeurs à droite et en bas
  tailley = tailley - 10;
  double echellex = (double)min((double)mesTps[cptmesure], max((double)0, ((double)mesTps[cptmesure] - (double)mesTps[0] + (double)2)));
  double echelley = max((double)2, (double)valmax - (double)valmin);
  double valmesOLD = 0;
  for (int cptlue = cptmesure; cptlue >= 0; cptlue--)
  {
    double graphdate = (double)min((double)mesTps[cptmesure] - (double)mesTps[0], (double)max((double)0, mesTps[cptlue] - (double)mesTps[0]));
    double x2 = (double)posx + (double)((double)taillex * (double)graphdate / (double)echellex);
    double y2 = (double)posy + (double)tailley - (double)((double)tailley * (double)((double)valmes[cptlue] - (double)valmin) / (double)echelley);
    double x = (double)max((double)min((double)x2, (double)posx + (double)taillex), (double)posx);
    double y = (double)max((double)min((double)y2, (double)posy + (double)tailley), (double)posy);
    if (modecolor == 1) //en rouge
    {
      Paint_DrawPoint(x, y, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    }
    if (DEBUGKL) Serial.print("cptlue:");
    if (DEBUGKL) Serial.println(cptlue);
    if (modecolor == 0)
    {
      if (cptlue > 0)
      {
        graphdate = (double)min((double)mesTps[cptmesure] - (double)mesTps[0], (double)max((double)0, mesTps[cptlue - 1] - (double)mesTps[0]));
        if (DEBUGKL) Serial.print("Graphdate-1:");
        if (DEBUGKL) Serial.println(graphdate);
        double x3 = (double)posx + (double)((double)taillex * (double)graphdate / (double)echellex);
        double y3 = (double)posy + (double)tailley - (double)((double)tailley * (double)(valmes[cptlue - 1] - valmin) / (unsigned long)echelley);
        double x4 = (double)max((double)min((double)x3, (double)posx + (double)taillex), (double)posx);
        double y4 = (double)max((double)min((double)y3, (double)posy + (double)tailley), (double)posy);
        Paint_DrawLine(x, y, x4, y4, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      }
    }
    else if (modecolor == 1)
    {
      if (valmes[cptlue] >= Seuil)
      {
        graphdate = (double)min((double)mesTps[cptmesure] - (double)mesTps[0], (double)max((double)0, mesTps[cptlue - 1] - (double)mesTps[0]));
        double x3 = (double)posx + (double)((double)taillex * (double)graphdate / (double)echellex);
        double y3 = (double)posy + (double)tailley - (double)((double)tailley * (double)(valmes[cptlue - 1] - valmin) / (unsigned long)echelley);
        double x4 = (double)max((double)min((double)x3, (double)posx + (double)taillex), (double)posx);
        double y4 = (double)max((double)min((double)y3, (double)posy + (double)tailley), (double)posy);
        if (DEBUGKL) Serial.println("Alarme");
        Paint_DrawLine(x, y, x4, y4, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        //Paint_DrawString_EN(x, y6, DispAff.c_str(), &Font12, BLACK, WHITE);
      }
    }
  }
}
