/********************************************************************/
/*  Autor:        Julian Schweizerhof                               */                 
/*  Firma:        Daimler AG                                        */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für automatisiertes Probenahmesystem         */
/*  Version:      0.3                                               */    
/*  To-Do:        evakuieren machen und Umgebungsmessung beschreiben*/                
/********************************************************************/
/*
#include <Arduino.h>      // Standard Arduino Bibliothek
#include <Wire.h>         // Wire Bibliothek von ArduinoTwoWire.h - TWI/I2C library for Arduino & Wiring
                          // Copyright (c) 2006 Nicholas Zambetti.
#include <SSD1306Wire.h>  // Driver for the SSD1306 and SH1106 based 128x64, 128x32, 64x48 pixel OLED display running on ESP8266/ESP32 https://thingpulse.com von Daniel Eichhorn
#include <WiFi.h>         //  WiFi.h - esp32 Wifi support. Based on WiFi.h from Arduino WiFi shield library. Copyright (c) 2011-2014 Arduino.  
                          // All right reserved. Modified by Ivan Grokhotkov, December 2014
#include <ESPAsyncWebServer.h>  // Asynchronous HTTP and WebSocket Server Library for ESP8266 and ESP32 von Hristo Gochkov
#include "FiniteStateMachine.h" // Zustandsautomat von Arduino https://playground.arduino.cc/Code/FiniteStateMachine/
#include <ArduinoJson.h>  // Arduino Json Lib von Benoît Blanchon
#include <SPIFFS.h>       // Lib für Zugriff auf den Speicher des ESPs
#include "Musik.h"        // Lib für den Piezo-Speaker inkl. pitches.h
//#include <ESPmDNS.h>      // Damit statt einer IP-Adresse eine Domain verwendet werden kann, läuft aktuell nicht zuverlässig
#include "Quicksort.h"    // Für den Drucksensor

/////////////////////// Defines //////////////////////////////////////////////////////////
// Pins
#define taster1 4
#define taster2 34
#define taster_reset 15
#define drucksensor 35
#define voltage_check 39
#define magnetventilP1 32
#define magnetventilP2 33
#define magnetventilB1 25
#define magnetventilB2 27       // Beutel 2 und 3 wurden anders herum angeschlossen
#define magnetventilB3 26       // Beutel 2 und 3 wurden anders herum angeschlossen
#define magnetventilB4 14
#define magnetventilK1 12
#define magnetventilK2 13
#define magnetventilK3 19
#define magnetventilK4 18
#define magnetventilK5 5
#define pumpe1l 17
#define pumpe10l 16
#define buzzer 23
// Ende Pins
// Zeiten
#define wartezeit_pumpe 1         // Wartezeit für Einschatlen der Pumpe, bis Magnetventile geschaltet werden: 1 Sekunde
#define evakuieren_testzeit 5     // Wartezeit für die Dichtigkeitsprüfung der Beutel: 5 Sekunden
#define evakuieren_abbruch 600    // Wenn nach Ablauf der Zeit nicht der gewünschte Unterdruck erreicht wird ist der Beutel undicht: 600 Sekunden
#define spuelzeit_kartusche 5    // Spülzeit für die Kartusche: 10 Sekunden 
#define spuelzeit_beutel 5       // Spülzeit für die Beutel: 10 Sekunden
#define fuellzeit_kartusche 480   // Füllzeit für die Kartusche: 480 Sekunden
#define fuellzeit_beutel 300      // Füllzeit für die Beutel: 300 Sekunden
#define lueftungszeit 300         // Lueftungszeit für die offenen Türen: 5 Minuten
#define anreicherungszeit 900    // Zeit für die Anreicherung des Abgases 20 +/- 5 Minuten
// Ende Zeiten
// Zustände
#define endeWerteInit 1           // Letzter Zustand WerteInit 
#define endeKalibirieren 7        // Letzter Zustand Kalibrieren 
#define endeEvakuierenBeutel 3    // Letzter Zustand EvakuierenBeutel 
#define endeVorbereiten 9         // Letzter Zustand Vorbereiten 
// Ende Zustände
// Weitere Definitionen
#define drucksollwert 755          // Ab welcher Druckdifferenz in mbar ist der Beutel leer, default: 95 kPa = 950 hPa, nach Messung 750 hPa
#define druckTestWert 690          // Ab welchem Druck sind die Beutel dicht, nach Messung 690 hPa
#define drucksensorError 100      // Drucksensor Korrekturwert
#define drucksensorAnzahlMesswerte 100  // Anzahl der Messwerte für einen Wert
#define drucksensorMuellMesswerte 10    // Wie viele Werte sollen je oben und unten weggeschnitten werden?
#define versorgungsSpannung 5.2     // Spannungsversorgung
// Ende Weitere Definitionen
/////////////////////// Ende Defines /////////////////////////////////////////////////////

/////////////////////// Bekanntmachung der Funktionen ////////////////////////////////////
// Grundlegendes
void setupVariables();            // Variablen in den Ausgangszustand bringen
// Interrupt Service Routinen
void IRAM_ATTR onTimer();         // Interrupt Service Routine für den 1 Sekunden Timer
void IRAM_ATTR resetInterrupt();  // Interrupt Service Routine für den Reset Taster
// WiFi
void sendToClient();              // Daten an die Webseite schicken
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
  // wird aufgerufen wenn ein WebSocket Event auftritt:
// HMI (Display und Webseite)
void displayFunktion();           // Display aktualisieren
void ausgabe();                   // Webseite aktualisieren
void functionTaster1();           // Ereignis, wenn Taster 1 gedrückt wird, primär auf der Webseite
void functionTaster2();           // Ereignis, wenn Taster 2 gedrückt wird, primär auf der Webseite
void checkButton();               // Überpüfung der Taster auf Ereignisse
void voltageCheck();              // Ist die Versorgungsspannung angeschlossen?
void functionReset();             // Reset nach drei Sekunden ausführen
// Drucksensor
double readDrucksensor();         // Drucksensor lesen, Rückgabewert: Unterdruck in kPa
// Statemachine                   // pro Zustand eine Enter-, Update und Exit-Funktion
void updateZustand();             // Zustandsautomat aktualisieren
void allesAbschalten();           // Alle Ventile und Pumpen werden deaktiviert
  // Zustand: werteInit
void enter_werteInit();
void update_werteInit();
void exit_werteInit();
  // Zustand: evakuierenBeutel
void enter_evakuierenBeutel();
void update_evakuierenBeutel();
void exit_evakuierenBeutel();
  // Zustand: probenahme
void enter_probenahme();
void update_probenahme();
void exit_probenahme();
  // Zustand: fertig
void enter_fertig();
void update_fertig();
void exit_fertig();
  // kalibrieren
void enter_kalibrieren();
void update_kalibrieren();
void exit_kalibrieren();
  // Zustand: vorbereiten
void enter_vorbereiten();
void update_vorbereiten();
void exit_vorbereiten();
/////////////////////// Ende Bekanntmachung der Funktionen ///////////////////////////////

/////////////////////// Deklarationen und wichtige Definitonen ///////////////////////////
// Grundlegendes
bool voltageCheckLastState = false;
// Timer
hw_timer_t * timer = NULL;                  // Erzeugen einen Pointer für den Hardware Timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; // Sperren der Register während der ISR
volatile bool flagTimerInterrupt = false;   // Flag für den Timer 1 Sekunde Interrupt 
// Reset
int longTouchReset = 3000;                  // Wie lange muss der Reset-Button gedrückt werden? Angabe in ms, default: 3000
bool pressedReset = false;                  // Flag für den Reset Interrupt, default: false
// WiFi und Webseite inkl. JSON Datein
const char * ssid     = "Probenahmesystem"; // ssid der Messkiste
const char * password = "Russland";     // Passwort für die WiFi Verbindung
//const char* dnsName = "daimler";
AsyncWebServer server(80);                  // Set web server port number to 80
AsyncWebSocket ws("/ws");                   // AyncWebSocket wird erstellt
AsyncWebSocketClient * globalClient = NULL; // Pointer auf den Client, für Überprüfung der Verbindung
struct EmpfangenData                        // Struktur für die empfangen Daten der JSON Datei
{
  bool flagTaster1;                         // Taster 1 gedrückt?
  bool flagTaster2;                         // Taster 2 gedrückt?
  bool flagKalibrieren;                     // Kalibrieren  gewünscht?
  bool flagDiesel;                          // Dieselfahrzeug?
  bool flagEvakuieren;                      // Beutel evakuieren?
}empfangenData;                             // Instanz mit dem Name "empfangenData" von der Struktur "EmpfangenData" erstellen
struct SendenData                           // Struktur für die gesendeten Daten der JSON Datei
{
  int button1;                              // Wie soll Taster 1 aussehen
  int button2;                              // Wie soll Taster 2 ausssehen
  String beutelName;                        // Welcher ist der aktuelle Beutel?
  String beutelZeit;                        // Wie lange wird der aktuelle Beutel noch gefüllt?
  String kartuschenName;                    // Welche ist der aktuelle Kartusche?
  String kartuschenZeit;                    // Wie lange wird die aktuelle Kartusche noch gefüllt?
  String anweisungTodo;                     // Anweisung an den Testingneuer Zeile 1
  String anweisungZeit;                     // Anweisung an den Testingneuer Zeile 2
  bool zustandSpannung;                      // Sind die 12 V angeschlossen?
} sendenData;                               // Instanz mit dem Name "sendenData" von der Struktur "SendenData" erstellen
IPAddress IP;                               // Variable zum Speichern der IP-Adresse 

// Fehler
enum Fehler {wifiFehler, evakuieren, probenahmeBeutelFehler, probenahmeKartuscheFehler};  // Fehler
bool flag_Fehler[8];

// Display
SSD1306Wire  display(0x3c, SDA, SCL);       // Objekt für Display erstellen

// Statemachine
const byte NUMBER_OF_STATES = 7;            // Anzahl der Zustände des Zustandsautomat

  //initialize states
State werteInit = State(enter_werteInit, update_werteInit, exit_werteInit);
State evakuierenBeutel = State(enter_evakuierenBeutel, update_evakuierenBeutel, exit_evakuierenBeutel);
State probenahme = State(enter_probenahme, update_probenahme, exit_probenahme);
State fertig = State(enter_fertig, update_fertig, exit_fertig);
State kalibrieren = State(enter_kalibrieren, update_kalibrieren, exit_kalibrieren);
State vorbereiten = State(enter_vorbereiten, update_vorbereiten, exit_vorbereiten);

FSM probenahmeSystem = FSM(werteInit);      // Starten des Zustandsautomat "probenahmeSystem"; Starten im Zustand: werteInit

  // Flags für den Zustandautomat
bool flagFertigEvakuieren;
bool flagFertigProbenahme;
bool flagFertigBeutel;
bool flagFertigKartusche;
bool flagAusgabeTimer;

bool flagKartuscheIstFertig;

  // weitere Zustandsautomaten
int stateWerteInit;
int stateEvakuierenBeutel;
int stateKalibrieren;
int stateFertig;
int stateVorbereiten;
enum StateBeutel {beutelSpuelen, beutel1Fuellen, beutel2Fuellen, beutel3Fuellen, beutelParkplatz, beutel4Fuellen, beutelUmstecken, beutel5Fuellen, beutelMessleitung, beutelLeerlauf, beutel6Fuellen, beutel7Fuellen, beutel8Fuellen, beutelFertig};  // Zustände für die Zustandsautomat in probenahme
enum StateKartusche {kartuscheSpuelen, kartusche1Fuellen, kartusche2Fuellen, kartusche3Fuellen, kartuscheParkplatz, kartusche4Fuellen, kartuscheUmstecken, kartusche5Fuellen, kartuscheMessleitung, kartuscheLeerlauf, kartusche6Fuellen, kartusche7Fuellen, kartusche8Fuellen, kartuscheFertig};  // Zustände für die Zustandsautomat in probenahme

  // probenahme
StateBeutel stateBeutel;                    // Instanz mit dem Name "stateBeutel" von der Enumeration "StateBeutel" erstellen
StateKartusche stateKartusche;              // Instanz mit dem Name "stateKartusche" von der Enumeration "StateKartusche" erstellen
unsigned long beutelAnfangsZeit;            // Variable zum Zwischenspeichern der aktuellen Zeit
unsigned long kartuscheAnfangsZeit;         // Variable zum Zwischenspeichern der aktuellen Zeit
// Ende Statemachine

// ausgabe
unsigned long ausgabeTimer;                 // Variable zum Zwischenspeichern der aktuellen Zeit für die Zeitausgabe auf der Webseite
unsigned long ausgabeTimerKartusche;        // Variable zum Zwischenspeichern der aktuellen Zeit für die Zeitausgabe auf der Webseite
/////////////////////// Ende Deklarationen und wichtige Definitonen //////////////////////

/////////////////////// Setup ////////////////////////////////////////////////////////////
void setup() 
{ 
  Serial.begin(115200);                     // Serielle Schnittstelle starten

  // Pinmode definieren
  pinMode(taster_reset, INPUT_PULLDOWN);    // Pulldown, damit der Pin einen festen Zustand hat und nicht "floated"
  pinMode(taster1, INPUT_PULLDOWN);         // Pulldown, damit der Pin einen festen Zustand hat und nicht "floated" 
  pinMode(taster2, INPUT);                  // Taster2 hat keinen internen Pulldown Widerstand, dieser sitzt auf der Platine
  pinMode(voltage_check, INPUT);            // voltage_check hat keinen Pulldown Widerstand
  pinMode(drucksensor, INPUT);              // Pin 35 hat keine Pullup oder Pulldown Widerstände
  pinMode(pumpe10l, OUTPUT);
  pinMode(pumpe1l, OUTPUT);
  pinMode(magnetventilP1, OUTPUT);
  pinMode(magnetventilP2, OUTPUT);
  pinMode(magnetventilB1, OUTPUT);
  pinMode(magnetventilB2, OUTPUT);
  pinMode(magnetventilB3, OUTPUT);
  pinMode(magnetventilB4, OUTPUT);
  pinMode(magnetventilK1, OUTPUT);
  pinMode(magnetventilK2, OUTPUT);
  pinMode(magnetventilK3, OUTPUT);
  pinMode(magnetventilK4, OUTPUT);
  pinMode(magnetventilK5, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(taster_reset), resetInterrupt, HIGH); // externer Interrupt für den Reset-Taster
  // Ende Pinmode definieren

  // Dateien vom Speicher laden
  if(!SPIFFS.begin())                       // Dateien vom Flashspeicher des ESP32 lesen, inkl. der Webseite
  {
      Serial.println("An Error has occurred while mounting SPIFFS");  // Zum Debuggen
      Serial.println("Sie muessen eine Webseite hochladen!"); // Zum Debuggen
      return;                               // Wenn keine Webseite hochgeladen wurde, wird kein WiFi eröffnet
  }
  // Ende Dateien vom Speicher laden

  // WiFi
  Serial.print("Starten AP (Access Point)…");
  WiFi.softAP(ssid, password,1,0,1);        // WiFi.softAP(ssid, password, channel, hidden, max_connection), nur ein Client ist
  IP = WiFi.softAPIP();                     // IP-Adresse auslesen
  // if(MDNS.begin(dnsName))
  // {
  //   Serial.println("DNS gestartet, erreichbar unter: ");
  //   Serial.println("http://" + String(dnsName)+ ".local/");
  // }
  Serial.print("AP IP-Adresse: ");
  Serial.println(IP);                       // IP-Adresse ausgeben
  ws.onEvent(onWsEvent);                    // Funktion bekannt machen, die aufgerufen werden soll bei einem WS Events
  server.addHandler(&ws);                   // Handler hinzufügen
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){  // wenn der Client auf die Adresse: "192.168.4.1/" zugreift
    request->send(SPIFFS, "/index.htm", "text/html"); // wird die Webseite vom Flash Speicher geladen und an den Client geschickt
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){   // wenn der Client auf die Adresse: "192.168.4.1/favicon.ico" zugreift
    request->send(SPIFFS, "/favicon.ico", "image/ico"); // wird das Favicon vom Flash Speicher geladen an an den Client geschickt.
  });
  server.begin();                           // Server wird gestarten
  // Ende WiFi

  // Display
  displayFunktion();                        // Diagnosedisplay aktualisieren
  // Ende Display

  // Timer erstellen der je Sekunde einen Interrupt erzeugt 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  // Ende Timer

  // Buzzer
  musik_setup();
  // Ende Buzzer

  // Spannung messen
  sendenData.zustandSpannung = digitalRead(voltage_check);
  // Ende Spannung messen 
}
/////////////////////// Ende Setup ///////////////////////////////////////////////////////

/////////////////////// loop /////////////////////////////////////////////////////////////
void loop() 
{
  probenahmeSystem.update();                // Hauptzustandsautomat laufen lassen
  voltageCheck();                           // Ist die Versorgungsspannung angeschaltet?
  checkButton();
  if(pressedReset)
  {
     functionReset();                          // Reset nach drei Sekunden ausführen
  }
  musik_loop();                             // Buzzer
  //readDrucksensor();                        // Zum Debuggen
  //displayFunktion();                        // Zum Debuggen
  //delay(1000);
}
/////////////////////// Ende loop ////////////////////////////////////////////////////////

/////////////////////// Funktionen ///////////////////////////////////////////////////////

// Interrupt Service Routine
void IRAM_ATTR onTimer() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  flagTimerInterrupt = true; 
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR resetInterrupt()
{
  Serial.println("Interrupt aufgerufen.");
  pressedReset = true;
}
// Ende Interrupt Service Routine

void setupVariables()                       // Variablen in den Ausgangszustand bringen
{
  flagFertigEvakuieren = false;
  flagFertigProbenahme = false;
  flagFertigBeutel = false;
  flagFertigKartusche = false;
  flagAusgabeTimer = true;
  probenahmeSystem = werteInit;
  flag_Fehler[wifiFehler] = 0;
  stateWerteInit = 0;
  stateEvakuierenBeutel = 0;
  stateKalibrieren = 0;
  stateFertig = 0;
  stateVorbereiten = 0;
  stateBeutel = beutelSpuelen;
  stateKartusche = kartuscheSpuelen;
  beutelAnfangsZeit = 0;
  kartuscheAnfangsZeit = 0;
  ausgabeTimer = 0;
  flagKartuscheIstFertig = 0;

  
}

void voltageCheck()                         // Ist die Versorgungsspannung angeschlossen?
{
  if(digitalRead(voltage_check) != voltageCheckLastState) // Nur bei einer Änderung soll was reingeschrieben werden
  {
    displayFunktion();
    sendenData.zustandSpannung = digitalRead(voltage_check);
    ausgabe();                              // Webseite aktualisieren wenn sich die Spannung ändert
  }
  voltageCheckLastState = digitalRead(voltage_check);
}

void checkButton()                          // Überprüfung der Taster
{
  //Serial.printf("Taster 1 ist: %d\n", digitalRead(taster1));  // Zum Debuggen
  //Serial.printf("Taster 2 ist: %d\n", digitalRead(taster2));  // Zum Debuggen
  if (digitalRead(taster1) == 1 && digitalRead(taster2) == 1) // wenn beide Taster gedrückt sind
  {
    buttonBoth = true;
    Serial.printf("Taster 1 ist: %d", digitalRead(taster1));  // Zum Debuggen
    Serial.printf("Taster 2 ist: %d", digitalRead(taster2));  // Zum Debuggen
  }
  if (digitalRead(taster1) == 0 && digitalRead(taster2) == 0 && buttonBoth == true) // wenn beide Taster gedrückt waren und losgelasen wurden
  {
    russland = !russland;
    buttonBoth = false;
    zustandMusik = 0;
  }
}

void functionReset()                        // Reset nach drei Sekunden ausführen
{
  Serial.println("fucntion Reset aufgerufen.");
  unsigned long aktuelleZeit = millis();
  while(digitalRead(taster_reset) == HIGH)  // Solange der Taster gdrückt wird
  {
    if(millis() - aktuelleZeit > longTouchReset)  // Wurde der Taster drei Sekunden gedrückt?
    {
      Serial.println("ESP wird neugestartet.");
      ESP.restart();
    }
    Serial.print("Wert ist:");
    Serial.println(millis()-aktuelleZeit);
  }
  pressedReset = false;                     // Interrupt Flag zurücksetzten
}
int drucksensorIndex = 0;                    // Laufindex Drucksensor
int drucksensorAnalog[drucksensorAnzahlMesswerte];
float drucksensorAnalogMittelwert = 0;
double differenzDruck = 0;
 
double readDrucksensor()                    // Drucksensor lesen, Rückgabewert: Unterdruck in mbar
{
  
  drucksensorAnalog[drucksensorIndex] = analogRead(drucksensor);
  if(drucksensorIndex < drucksensorAnzahlMesswerte-1)
  {
    drucksensorIndex++;
  }
  else
  {
    double ausgangsSpannung;
    quickSort(drucksensorAnalog, 0, (drucksensorAnzahlMesswerte-1));
    for(int i = drucksensorMuellMesswerte; i < drucksensorAnzahlMesswerte - drucksensorMuellMesswerte; i++)
    {
     // Serial.println(drucksensorAnalog[i]);   // Zum Debuggen
      drucksensorAnalogMittelwert += drucksensorAnalog[i];
    }
    //Serial.print("Die Summe der Werte ist:"); // Zum Debuggen
    // Serial.println(drucksensorAnalogMittelwert); // Zum Debuggen
    drucksensorAnalogMittelwert /= (drucksensorAnzahlMesswerte - ( 2 * drucksensorMuellMesswerte));
    //Serial.print("Das analoge Mittelwert ist: "); // Zum Debuggen
    //Serial.println(drucksensorAnalogMittelwert);  // Zum Debuggen
    ausgangsSpannung = 3.3 * drucksensorAnalogMittelwert/4095;
    differenzDruck = ( (ausgangsSpannung/versorgungsSpannung) - (0.04) ) / 0.0018 * 10 + drucksensorError;
    //Serial.print("Der Differenzdruck ist in mbar:");  // Zum Debuggen
    
    
    //Serial.println(differenzDruck); // Zum Debuggen

    drucksensorAnalogMittelwert = 0;
    drucksensorIndex = 0;
  }
  
  // Serial.println(analogRead(drucksensor));  // Zum Debuggen
  //ausgangsSpannung = 3.3 * analogRead(drucksensor)/4095;
  //Serial.println(ausgangsSpannung);      // Zum Debuggen
  //differenzDruck = ( (ausgangsSpannung/versorgungsSpannung) - (0.04) ) / 0.0018 * 10 + drucksensorError;
  //Serial.println(differenzDruck);        // Zum Debuggen
  return differenzDruck; 
}

// Websocket Funktion
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  // wird aufgerufen wenn ein WebSocket Event auftritt:
  if(type == WS_EVT_CONNECT)                // was soll bei einer Verbindung passieren?
  {
    Serial.println("Websocket Verbindung hergestellt.");  // Zum Debuggen
    globalClient = client;                  // client speichern
    Serial.printf("Die ID des Client ist: %d",globalClient->id()); // Zum Debuggen
    ausgabe();                              // Webseite aktualisieren
  } 
  else if(type == WS_EVT_DISCONNECT)        // was soll bei einem Verbindungsabbruch passieren?
  {
    Serial.println("Client disconnected");
    Serial.println("-----------------------");
    globalClient = NULL;                    // client Pointer leer machen
  } 
  else if(type == WS_EVT_DATA)              // was soll bei empfangenen Daten passieren passieren?
  {
    char nachricht[len];                    // Ein Chararray für das Empfangen des Datenstrings im JSON-Format erstelle, len ist die Größe der empfangen Daten
    Serial.print("Data received: ");        // Zum Debuggen
    for(int i=0; i < len; i++)              // Komplette Nachricht auslesen und in das Char-Array "nachricht" schreiben
    {
          nachricht[i] = ((char) data[i]);  // Datenstring füllen, int-Array data in ein char-Array konvertieren
          Serial.print(((char) data[i]));   // Zum Debuggen
    }
    DynamicJsonDocument empfangenJSON(1024); // JSON Dokument erstellen, um empfangen Daten zu verstehen, erforderliche Größe kann auf "https://arduinojson.org/v6/assistant/" berechnet werden
    
    DeserializationError fehlerJSON = deserializeJson(empfangenJSON, nachricht); // Nachricht aus dem Chararray "nachricht" ist das JSON Dokument "empfangenJSON" übertragen
    Serial.println(fehlerJSON.c_str());     // Fehler bei Lesen der JSON Nachricht 
    empfangenData.flagTaster1 = empfangenJSON["flagButton1"]; // Daten in den Strukt schreiben
    empfangenData.flagTaster2 = empfangenJSON["flagButton2"]; // Daten in den Strukt schreiben
    empfangenData.flagKalibrieren = empfangenJSON["flagKalibrieren"]; // Daten in den Strukt schreiben
    empfangenData.flagDiesel = empfangenJSON["flagDiesel"]; // Daten in den Strukt schreiben
    empfangenData.flagEvakuieren = empfangenJSON["flagEvakuieren"]; // Daten in den Strukt schreiben
  
    Serial.println(empfangenData.flagTaster1);  // Zum Debuggen
    Serial.println(empfangenData.flagTaster2);  // Zum Debuggen
    Serial.println(empfangenData.flagKalibrieren);  // Zum Debuggen
    Serial.println(empfangenData.flagDiesel); // Zum Debuggen
    Serial.println(empfangenData.flagEvakuieren); // Zum Debuggen

    if (empfangenData.flagTaster1 && empfangenData.flagTaster2) // Wenn Reset gedrückt wird: Zurück zum Initalstate
    {
      probenahmeSystem.transitionTo(werteInit);       // Soft Reset
      //ESP.restart();                                // Hard Reset
    }
    else if (empfangenData.flagTaster1)          // Wenn Taster 1 gedrückt wird
    {
      functionTaster1();
      empfangenData.flagTaster1 = 0;
    }
    else if (empfangenData.flagTaster2)     // Wenn Taster 2 gedrückt wird
    {
      functionTaster2();
      empfangenData.flagTaster2 = 0;
    }
    ausgabe();                              // Webseite aktualisieren
  }
}
// Ende Websocket Funktion

// Ausgabe auf der Webseite Funktion
void ausgabe()    // Ausgabe auf die HMI (Webseite)
{
  // Serial.println("Funktion ausgabe aufgerufen"); // Zum Debuggen
  if(globalClient != NULL && globalClient->status() == WS_CONNECTED)    // wenn die Verbindung zur Webseite besteht
  {
    if(probenahmeSystem.isInState(werteInit))
    {
      switch (stateWerteInit)
      {
      case 0:
        sendenData.button1 = 6;
        sendenData.button2 = 1;
        sendenData.beutelName = "Hier wird der aktuelle Beutel dargestellt.";
        sendenData.beutelZeit = "Hier steht die restliche Befüllzeit für den aktuellen Beutel.";
        sendenData.kartuschenName = "Hier wird die aktuelle Kartusche dargestellt.";
        sendenData.kartuschenZeit = "Hier steht die restliche Befüllzeit für die aktuellen Kartusche.";
        sendenData.anweisungTodo = "Hier werden Anweisungen an den Testdurchführer stehen.";
        sendenData.anweisungZeit = "";
        break;
      case 1:
        sendenData.button1 = 3;
        sendenData.button2 = 1;
        sendenData.beutelName = "<b>Gewünschte Einstellungen wählen</b>";
        sendenData.beutelZeit = "";
        sendenData.kartuschenName = "";
        sendenData.kartuschenZeit = "";
        sendenData.anweisungTodo = "Kalibrieren gewünscht? Dieselfahrzeug? Beutel evakuieren gewünscht? <br>Hinweis: Einstellung können später nicht mehr geändert werden!";
        sendenData.anweisungZeit = "";
        break;
      default:
        break;
      }
      // Serial.println("werteInit"); // Zum Debuggen
    }
    else if(probenahmeSystem.isInState(kalibrieren))
    {
      switch (stateKalibrieren)
      {
      case 0:
        sendenData.button1 = 1;
        sendenData.button2 = 1;
        sendenData.beutelName = "<b>Kalibrieren</b>";
        sendenData.beutelZeit = "";
        sendenData.kartuschenName = "";
        sendenData.kartuschenZeit = "";
        sendenData.anweisungTodo = "Eingang des Probenahmesystems mit dem Ausgang des Düsen-Kalibrierprüfstands verbinden.";
        sendenData.anweisungZeit = "";
        break;
      case 1:
        sendenData.anweisungTodo = "Kartusche in Position 1 einsetzten. In die übrigen Positionen Blindstopfen oder Kartuschen einsetzen.";
        break;
      case 2:
        sendenData.kartuschenName = "Messung Kartuschen Weg aktiv";
        sendenData.anweisungTodo = "Wenn die Messung fertig ist, Weiter drücken.";
        break;
      case 3:
        sendenData.kartuschenName = "";
        sendenData.anweisungTodo = "Beutel in Position 1 einstecken.";
        break; 
      case 4:
        sendenData.kartuschenName = "Messung Beutel Weg aktiv";
        sendenData.anweisungTodo = "Wenn die Messung fertig ist, Weiter drücken.";
        break;
      case 5:
        sendenData.kartuschenName = "";
        sendenData.anweisungTodo = "Kartusche in Position 1 einsetzten und Beutel in Position 1 einstecken.";
        break;
      case 6:
        sendenData.kartuschenName = "Messung Beutel Weg und Kartuschen Weg aktiv";
        sendenData.anweisungTodo = "Wenn die Messung fertig ist, Weiter drücken.";
        break;
      case 7:
        sendenData.kartuschenName = "Messung beendet";
        sendenData.anweisungTodo = "Beim Drücken von Weiter gelagen Sie auf die Startseite";
        break;     
      default:
        break;
      }
      // Serial.println("Kalibrieren"); // Zum Debuggen
    }
    else if(probenahmeSystem.isInState(evakuierenBeutel))
    {
      switch (stateEvakuierenBeutel)
      {
      case 0:
        sendenData.beutelName = "";
        sendenData.anweisungTodo = "Alle Beutel werden bei einem Klick auf *Weiter* evakuiert.";
        break;
      case 1:
        sendenData.beutelName = "Der aktuelle Druck ist: " + (String) readDrucksensor() + " mbar <br> Der Sollwert ist: " + (String) (drucksollwert) + " mbar";
         sendenData.anweisungTodo = "Alle Beutel werden gerade evakuiert";
         Serial.println(readDrucksensor());
        break;
      case 2:
        sendenData.beutelName = "Der aktuelle Druck ist: " + (String) readDrucksensor() + " mbar <br> Der Sollwert ist: " + (String) (drucksollwert) + " mbar";
        sendenData.anweisungTodo = "Das System wird auf Dichte überprüft.";
        //Serial.println("evakuierenBeutel CASE 2");
        Serial.println(readDrucksensor());
        break;
      case 3:
        sendenData.beutelName = "";
        if(flagFertigEvakuieren)
        {
          sendenData.anweisungTodo = "Evakuieren war erfolgreich, es geht weiter mit der Messung.";
        }
        else
        {
          sendenData.anweisungTodo = "Evakuieren war nicht erfolgreich, es geht zurück zum Anfang. Bitte Überprüfen Sie das System auf Undichtigkeiten.";
        }
      default:
        break;
      }
    }
    else if (probenahmeSystem.isInState(vorbereiten))
    {
      switch (stateVorbereiten)
      {
      case 0:
        sendenData.button1 = 1;
        sendenData.button2 = 1;
        sendenData.beutelZeit = "";
        sendenData.kartuschenName = "";
        sendenData.kartuschenZeit = "";
        sendenData.beutelName = "<b>Testvorbereitung</b>";
        sendenData.anweisungTodo = "Die Stromversorgung des Messsystems mit der Bordspannungsteckdose verbinden. Messleitung am Fahrersitz befestigen. Die ersten vier Beutel und die ersten vier Kartuschen einsetzen. Bitte darauf achten, dass eine Blindkartusche eingesetzt ist!";
        sendenData.anweisungZeit = "";
        break;
      case 1:
        sendenData.anweisungTodo = "Fahrzeug visuell inspiziert? (Fenster, Fahrzeugzelle, Türen, Abgasanlage, ...)";
        break;
      case 2:
        sendenData.anweisungTodo = "Tankfüllung mind. 90 %? Wurde das Fahrzeug, 6 Stunden vor dem Test, in einem geschlossenen Raum, bei der am Messtag erwartet Temperatur, konditioniert ( +/- 5 °C)?";
        break;
      case 3:
        sendenData.anweisungTodo = "Lufttemperatur, Windgeschwindigkeit und Luftfeuchte messen.";
        break;
      case 4:
        sendenData.anweisungTodo = "Messwerte im folgenden Bereich: <br>Lufttemperatur: -20 °C bis 30 °C <br>Windgeschwindigkeit: < 5 m/s <br>Luftfeuchte: < 90 %? <br>Ansonsten Messung verschieben!";
        break; 
      case 5:
        flagAusgabeTimer = true;               // Damit beim Case 5 (Lüften) die Zeit von vorne beginnt. 
        sendenData.anweisungTodo = "1. Motor starten. <br> 2. Vom Konditionierraum auf die Einfahrbahn fahren <br> 3. Fahrzeug parken <br> 4. Motor aus";
        sendenData.anweisungZeit = "";
        break;
      case 6:
        if(flagAusgabeTimer)
        {
          ausgabeTimer = millis();
          flagAusgabeTimer = false;
        }
        sendenData.anweisungTodo = "5. Für 4 bis 6 Minuten alle Fenster und Türen öffnen";
        sendenData.anweisungZeit = "Noch " + (String) (lueftungszeit - (millis() - ausgabeTimer)/1000) + " Sekunden lüften.";
        if((lueftungszeit - (millis() - ausgabeTimer)/1000) == 0)
        {
          stateVorbereiten++;
          flagAusgabeTimer = true;
        }
        break;
      case 7:
        sendenData.anweisungTodo = "6. Wieder alles schließen: <br>Fenster und Türen zu, <br>Klima aus, <br>Umluft aus, <br>Heizung aus, <br>Lüftung an.";
        sendenData.anweisungZeit = "";
        break;
      case 8: 
        flagAusgabeTimer = true;                // Damit beim Case 8 (weiter fahren) die Zeit von vorne beginnt.
        sendenData.anweisungTodo = "7. Motor anlassen und konstant 50 km/h fahren, beim Erreichen der Geschwindigkeit Weiter drücken";
        sendenData.anweisungZeit = "";
        break;
      case 9:
        if(flagAusgabeTimer)
        {
          ausgabeTimer = millis();
          flagAusgabeTimer = false;
        }
        sendenData.anweisungTodo = "8. Für 15 bis 25 Minuten 50 km/h (+/- 5 km/h) konstant weiter fahren.";
        sendenData.anweisungZeit = "Noch " + (String) (anreicherungszeit - (millis() - ausgabeTimer)/1000) + " Sekunden.";
        if(((int) (anreicherungszeit - (millis() - ausgabeTimer)/1000)) <= 0)
        {
          probenahmeSystem.transitionTo(probenahme);
        }
        break;
      default:
        break;
      }
    }
    else if(probenahmeSystem.isInState(probenahme))
    {
      if(flagAusgabeTimer)
      {
        ausgabeTimer = millis();
        ausgabeTimerKartusche = millis();
        flagAusgabeTimer = false;
      }
      Serial.println(stateBeutel);
      switch (stateBeutel)
      {
        case beutelSpuelen:
          sendenData.button1 = 0;
          sendenData.beutelName = "Beutel-Weg wird gespült.";
          sendenData.beutelZeit = "Noch " + (String) (spuelzeit_beutel - (millis() - ausgabeTimer)/1000) + " Sekunden.";
          sendenData.anweisungTodo = "Konstant 50 km/h fahren <br> Bitte warten bis das Spülen vorbei ist.";
          sendenData.anweisungZeit = "";  
        break;
        case beutel1Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "1. Beutel 1 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden.";
            sendenData.anweisungTodo = "Konstant 50 km/h fahren.";
          sendenData.anweisungZeit = "";
        break;
        case beutel2Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "1. Beutel 2 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden.";
        break;
        case beutel3Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "1. Beutel 3 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden.";
        break;
        case beutelParkplatz:
            Serial.println("Ich bin in Zustand 4: BeutelParkplatz");    // Zum Debuggen
          if(empfangenData.flagDiesel && flagKartuscheIstFertig == false)  // wenn ein Diesel gemessen wird und die Kartusche noch nicht fertig ist
          {
              Serial.println("Diesel und Kartuschen nicht fertig"); // Zum Debuggen
            sendenData.button1 = 0;
            sendenData.beutelName = "Die Beutel (50 km/h, Innenraum) sind gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.anweisungTodo = "Konstant 50 km/h fahren <br> Bitte warten bis die Kartuschen und die Beutel fertig sind.";
          }
          else // kein Diesel
          {
              Serial.println("Kein Diesel"); // Zum Debuggen
            sendenData.beutelName = "Die Beutel (50 km/h, Innenraum) sind gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.button1 = 5;
            sendenData.anweisungTodo = "50 km/h Messung beendet. Bitte einen Parkplatz anfahren, den Motor ausmachen und die Messleitung 5 Metern vom Fahrzeug platzieren. Achten Sie darauf, dass die Zündung an ist. <br>Beim Klick auf Weiter startet die nächste Messung.";
              //sendenData.kartuschenZeit = "drinnenTestTest";  // Zum Debuggen
          }
            //sendenData.kartuschenName = "TestTestTestdraußen";  //// Zum Debuggen
            //Serial.println(sendenData.anweisungTodo); // Zum Debuggen
          //sendToClient();
            //Serial.println("sendToClient aufgerufen"); // Zum Debuggen

        break;
        case beutel4Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "1. Beutel 4 (Hintergrund 1) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden";
          sendenData.anweisungTodo = "Achten Sie darauf, dass die Zündung an ist. Aber den Motor auslassen.";
        break;
        case beutelUmstecken:
          if(empfangenData.flagDiesel && flagKartuscheIstFertig == false)  // wenn ein Diesel gemessen wird und die Kartusche noch nicht fertig ist
          {
            sendenData.button1 = 0;
            sendenData.beutelName = "1. Beutel 4 (Hintergrund 1) ist gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.anweisungTodo = "Zündung weiterhin anlassen <br> Bitte warten bis die Kartuschen und die Beutel fertig sind.";
          }
          else // kein Diesel und Kartuschen fertig
          {
              Serial.println("Kein Diesel"); // Zum Debuggen
            sendenData.button1 = 5;
            sendenData.beutelName = "1. Beutel 4 (Hintergrund 1) ist gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.anweisungTodo = "Die ersten vier Beutel sind gefüllt. Bitte die nächsten vier Beutel einstecken. Den Motor weiterhin auslassen und die Messleitung weiterhin 5 Metern vom Fahrzeug platzieren. Achten Sie darauf, dass die Zündung an ist. <br>Beim Klick auf Weiter startet die nächste Messung.";
          }
        break;
        case beutel5Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "2. Beutel 4 (Hintergrund 2) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden";
          sendenData.anweisungTodo = "Zündung weiterhin anlassen <br> Bitte warten bis die Kartuschen und die Beutel fertig sind.";
        break;
        case beutelMessleitung:
          if(empfangenData.flagDiesel && flagKartuscheIstFertig == false)  // wenn ein Diesel gemessen wird und die Kartusche noch nicht fertig ist
          {
            //  Serial.println("Diesel und Kartuschen nicht fertig"); // Zum Debuggen
            sendenData.button1 = 0;
            sendenData.beutelName = "2. Beutel 4 (Hintergrund 2) ist gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.anweisungTodo = "Zündung weiterhin anlassen <br> Bitte warten bis die Kartuschen und die Beutel fertig sind.";
          }
          else // kein Diesel und Kartuschen fertig
          {
            //  Serial.println("Kein Diesel"); // Zum Debuggen
            sendenData.beutelName = "2. Beutel 4 (Hintergrund 2) ist gefüllt.";
            sendenData.beutelZeit = "";
            sendenData.button1 = 5;
            sendenData.anweisungTodo = "Die Hintergrundmessungen sind beendet. Bitte platzieren sie die Messleitung wieder am Fahrersitz. Parken Sie das Fahrzeug ingegen der Windrichtung. Den Motor anmachen und im Leerlauf lassen.";
            sendenData.anweisungZeit = "";
          }
          flagAusgabeTimer = true;  // Damit die Zeit in beutelLeerlajuf von vorne beginnt
        break;
        case beutelLeerlauf:
          if(flagAusgabeTimer)
          {
            ausgabeTimer = millis();
            flagAusgabeTimer = false;
          }
          sendenData.anweisungTodo = "Für 15 bis 25 Minuten im Leerlauf bleiben.";
          sendenData.anweisungZeit = "Noch " + (String) (anreicherungszeit - (millis() - ausgabeTimer)/1000) + " Sekunden.";
          if(((int) (anreicherungszeit - (millis() - ausgabeTimer)/1000)) <= 0)
          {
            stateBeutel = static_cast<StateBeutel>(stateBeutel + 1);
          }
          sendenData.button1 = 5;
        break;
        case beutel6Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "2. Beutel 1 (Leerlauf) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden";
          sendenData.anweisungTodo = "Motor im Leerlauf lassen.";
          sendenData.anweisungZeit = "";
        break;
        case beutel7Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "2. Beutel 2 (Leerlauf) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden";
        break;
        case beutel8Fuellen:
          sendenData.button1 = 2;
          sendenData.beutelName = "2. Beutel 3 (Leerlauf) wird gefüllt.";
          sendenData.beutelZeit = "Restliche Dauer: " + (String)((fuellzeit_beutel - (millis()-beutelAnfangsZeit)/1000)) + " Sekunden";
        break;
        case beutelFertig:
          sendenData.beutelName = "Alle Beutel sind gefüllt.";
          sendenData.beutelZeit = "";
        break;
        default:
        break;
      }
      if(empfangenData.flagDiesel)    // bei einem Dieselfahrzeug
      {
        switch (stateKartusche)
        {
        case kartuscheSpuelen:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "Kartuschen-Weg wird gespült";
          sendenData.kartuschenZeit = "Noch " + (String) (spuelzeit_kartusche - (millis() - ausgabeTimerKartusche)/1000) + " Sekunden.";
          break;
        case kartusche1Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 1 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden";
          break;
        case kartusche2Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 2 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden";      
          break;
        case kartusche3Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 3 (50 km/h, Innenraum) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden";       
          break;
        case kartuscheParkplatz:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "Die Kartuschen (50 km/h, Innenraum) sind gefüllt.";
          sendenData.kartuschenZeit = "";
          break;
        case kartusche4Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 4 (Hintergrund 1) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden";       
          break;
        case kartuscheUmstecken:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "Kartusche 4 (Hintergrund 1) ist gefüllt.";
          sendenData.kartuschenZeit = "";       
          break;
        case kartusche5Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 5 (Hintergrund 2) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden"; 
          break;
        case kartuscheMessleitung:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "Kartusche 5 (Hintergrund 2) ist gefüllt.";
          sendenData.kartuschenZeit = "";
          break;
        case kartuscheLeerlauf:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "";
          sendenData.kartuschenZeit = "";
          break;
        case kartusche6Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 6 (Leerlauf) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden"; 
          break;
        case kartusche7Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 7 (Leerlauf) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden"; 
          break;
        case kartusche8Fuellen:
          sendenData.button2 = 2;
          sendenData.kartuschenName = "Kartusche 8 (Leerlauf) wird gefüllt.";
          sendenData.kartuschenZeit = "Restliche Dauer: " + (String)((fuellzeit_kartusche - (millis()-kartuscheAnfangsZeit)/1000)) + " Sekunden"; 
          break;
        case kartuscheFertig:
          sendenData.button2 = 0;
          sendenData.kartuschenName = "Alle Kartuschen sind fertig";
          sendenData.kartuschenZeit = "";        
          break;
        default:
          break;
        }
      }
      else
      {
        sendenData.button2 = 0;
        sendenData.kartuschenName = "";
        sendenData.kartuschenZeit = "";
      }
      
      // Serial.println("probenahme");  // Zum Debuggen
      Serial.println(stateBeutel);
      Serial.println(stateKartusche);  // Zum Debuggen
    }
    else if(probenahmeSystem.isInState(fertig))
    {
      sendenData.button1 = 4;
      sendenData.button2 = 1;
      sendenData.beutelName = "Alles gefüllt!!!";
      sendenData.beutelZeit = "";
      sendenData.kartuschenName = "";
      sendenData.kartuschenZeit = "";
      sendenData.anweisungTodo = "Probenahme ist abgeschlossen bitte fahren Sie zurück. <br> Einen schönen Tag noch!";
      sendenData.anweisungZeit = "Bei einen Klick auf Weiter gelangen sie zur Startseite.";
      // Serial.println("fertig"); // Zum Debuggen
    }
    sendToClient();   // Daten an die Webseite schicken
    //Serial.println("Daten wurden an Client geschickt.");
  }
}
// Ende Ausgabe auf der Webseite Funktion

// Taster 1 Funktion
void functionTaster1()
{
  Serial.println("Funktion Taster 1 aufgerufen");
  if(probenahmeSystem.isInState(werteInit))
  {
    Serial.println("werteInit");
    if(stateWerteInit == 0)
    {
      // darf nicht vorkommen
    }
    else
    {
      stateWerteInit--;
    }
    
  }
  else if(probenahmeSystem.isInState(kalibrieren))
  {
    Serial.println("Kalibrieren");
    if(stateKalibrieren == 0)
    {
      probenahmeSystem.transitionTo(werteInit);
    }
    else
    {
      stateKalibrieren--;
    }
  }
  else if(probenahmeSystem.isInState(evakuierenBeutel))
  {
    Serial.println("evakuierenBeutel");
    if(stateEvakuierenBeutel == 0)
    {
      probenahmeSystem.transitionTo(werteInit);
    }
    else
    {
      stateEvakuierenBeutel--;
    }
  }
  else if (probenahmeSystem.isInState(vorbereiten))
  {
    Serial.println("vorbereiten");
    if(stateVorbereiten == 0)
    {
      probenahmeSystem.transitionTo(werteInit);
    }
    else
    {
      stateVorbereiten--;
    }
  }
  
  else if(probenahmeSystem.isInState(probenahme)) 
  {
    Serial.println("probenahme");
    if(stateBeutel == beutelFertig)
    {
      // nichts soll passieren
    }
    // enum StateBeutel {beutelSpuelen, beutel1Fuellen, beutel2Fuellen, beutel3Fuellen, beutelParkplatz, beutel4Fuellen, beutelUmstecken, beutel5Fuellen, beutelMessleitung, beutelLeerlauf, beutel6Fuellen, beutel7Fuellen, beutel8Fuellen, beutelFertig};  // Zustände für die Zustandsautomat in probenahme
    else if(empfangenData.flagDiesel && (stateBeutel == beutelParkplatz || stateBeutel == beutelUmstecken || stateBeutel == beutelMessleitung || stateBeutel == beutelLeerlauf))
    {
      stateBeutel = static_cast<StateBeutel>(stateBeutel + 1);
      stateKartusche = static_cast<StateKartusche>(stateKartusche + 1);
      flagKartuscheIstFertig = false;
      kartuscheAnfangsZeit = millis();
      beutelAnfangsZeit = millis();
    }
    else
    {
      beutelAnfangsZeit = millis();   // Damit die Füllzeiten beim Überspringen stimmen
      stateBeutel = static_cast<StateBeutel>(stateBeutel + 1);
    }    
  }
  else if(probenahmeSystem.isInState(fertig))
  {
    Serial.println("fertig");
    ESP.deepSleep(4e9);                     // ESP für 4000 Sekunden schlafen lassen
  }
}
// Ende Taster 1 Funktion

// Taster 2 Funktion
void functionTaster2()
{
  Serial.println("Funktion Taster 2 aufgerufen");
  if(probenahmeSystem.isInState(werteInit))
  {
    Serial.println("werteInit");
    if(stateWerteInit == endeWerteInit)                     // wenn letzer State von WerteInit erreicht wurde, gehe weiter
    {
      if (empfangenData.flagKalibrieren)                    // wenn Kalibrieren ausgewählt, dann zu kalibrieren springen
      {
        probenahmeSystem.transitionTo(kalibrieren);     
      }
      else if (empfangenData.flagEvakuieren)                // wenn Evakuieren augwhält, dann zu evakuieren springen   
      {
        probenahmeSystem.transitionTo(evakuierenBeutel);
      }
      else                                                  // ansonsten zu vorbereiten springen. 
      {
        probenahmeSystem.transitionTo(vorbereiten);
      }
    }
    else
    {
      stateWerteInit++;
    }
  }
  else if(probenahmeSystem.isInState(kalibrieren))
  {
    Serial.println("Kalibrieren");
    if(stateKalibrieren == endeKalibirieren)
    {
      probenahmeSystem.transitionTo(werteInit);
    }
    else
    {
      stateKalibrieren++;
    }
  }
  else if(probenahmeSystem.isInState(evakuierenBeutel))
  {
    Serial.println("evakuierenBeutel");
    if(stateEvakuierenBeutel == endeEvakuierenBeutel && flagFertigEvakuieren == true)
    {
      probenahmeSystem.transitionTo(vorbereiten);
    }
    else if(stateEvakuierenBeutel == endeEvakuierenBeutel && flagFertigEvakuieren == false)
    {
      probenahmeSystem.transitionTo(werteInit);
    }
    else
    {
      if(stateEvakuierenBeutel == 1)
      {
        beutelAnfangsZeit = millis();
      }
      stateEvakuierenBeutel++;
    }
  }
  else if (probenahmeSystem.isInState(vorbereiten))
  {
    Serial.println("vorbereiten");
    if(stateVorbereiten == endeVorbereiten)
    {
      probenahmeSystem.transitionTo(probenahme);
    }
    else
    {
      stateVorbereiten++;
    }
  }  
  else if(probenahmeSystem.isInState(probenahme))
  {
    Serial.println("probenahme");
    if(stateKartusche == kartuscheFertig)
    {
      //passiert nichts
    }
    else
    {
      kartuscheAnfangsZeit = millis();
      stateKartusche = static_cast<StateKartusche>(stateKartusche + 1);
    } 
  }
  else if(probenahmeSystem.isInState(fertig))
  {
    Serial.println("fertig");
    probenahmeSystem.transitionTo(werteInit);
  }
}
// Taster 2 Funktion

// Funktion zum Senden der JSON Datei an die Webseite
void sendToClient()
{
  DynamicJsonDocument sendenJSON(1024);
  sendenJSON["beutelName"] = sendenData.beutelName;
  sendenJSON["beutelZeit"] = sendenData.beutelZeit;
  sendenJSON["kartuschenName"] = sendenData.kartuschenName;
  sendenJSON["kartuschenZeit"] = sendenData.kartuschenZeit;
  sendenJSON["anweisungTodo"] = sendenData.anweisungTodo;
  sendenJSON["anweisungZeit"] = sendenData.anweisungZeit;
  sendenJSON["button1"] = sendenData.button1;
  sendenJSON["button2"] = sendenData.button2;
  sendenJSON["zustandSpannung"] = sendenData.zustandSpannung;
  String JSONmessage;
   // Serial.print("Gesendete Daten sind: "); // Zum Debuggen
   
  serializeJson(sendenJSON, JSONmessage);
  globalClient->text(JSONmessage);
  // Serial.println(JSONmessage);  // Zum Debuggen
}
// Ende Funktion zum Senden der JSON Datei an die Webseite

// Funktionen für die Statemachine
  // Zustand: werteInit
void enter_werteInit()
{
  Serial.println("Ich bin in werteInit.");  // Zum Debuggen
  setupVariables();                         // Variablen in den Ausgangszustand bringen
  ausgabe();                                // Webseite aktualisieren
}
void update_werteInit()
{
  if(flag_Fehler[wifiFehler])               // herscht ein Wifi-Fehler? Wenn ja --> Reset
  {
    ESP.restart();
  }
}
void exit_werteInit()                       
{
  Serial.println("werteInit wurde verlassen");  // Zum Debuggen
}

  // Zustand: evakuierenBeutel
void enter_evakuierenBeutel()
{
  Serial.println("Ich bin in evakuierenBeutel.");
  stateEvakuierenBeutel = 0;                // Interner Zustand auf 0 setzten
  ausgabe();                                // Webseite aktualisieren
  timerAlarmEnable(timer);                  // Timer starten
}
void update_evakuierenBeutel()
{
  if(flagTimerInterrupt)      // vielleicht noch optimieren, wird nicht immer benötigt
  {
    portENTER_CRITICAL(&timerMux);
    flagTimerInterrupt = false;
    portEXIT_CRITICAL(&timerMux);
    ausgabe();
  }
  switch(stateEvakuierenBeutel)
  {
    case 0:   // Nachricht

      break;
    case 1:   // alle Ventile schalten
      digitalWrite(magnetventilP1, HIGH);
      digitalWrite(magnetventilB1, HIGH);
      digitalWrite(magnetventilB2, HIGH);
      digitalWrite(magnetventilB3, HIGH);
      digitalWrite(magnetventilB4, HIGH);
      digitalWrite(pumpe10l, HIGH);
      if(readDrucksensor() >= (drucksollwert) )  // wenn die Beutel evakuiert sind,
      {
        beutelAnfangsZeit = millis();
        stateEvakuierenBeutel++;
      }
        break;
      case 2: // Dichte testen
        digitalWrite(magnetventilP1, LOW);  // Beutel abgedichtet
        digitalWrite(pumpe10l, LOW);        // Pumpe abschalten
        readDrucksensor();                  // Damit auf der UI der aktueller Wert gelesen werden kann. 
        if(millis() - beutelAnfangsZeit > evakuieren_testzeit*1000)
        {
          if(readDrucksensor() >= (druckTestWert)  )// wenn die Beutel evakuiert sind, mind. 700 mbar
          {
            flagFertigEvakuieren = true;
            stateEvakuierenBeutel++;
          }
          else
          {
            flagFertigEvakuieren = false;
            stateEvakuierenBeutel++;
          }
        }
        break;
      case 3: // Ergebnis anzeigen
        break;
    default:
      break;
  }
  // Beutel entleeren
  // Überprüfen ob Beutel dicht?
}
void exit_evakuierenBeutel()
{
  timerAlarmDisable(timer);               // Timer stoppen
  Serial.println("exit_evakuierenBeutel wurde verlassen");
}

  // Zustand: probenahme
void enter_probenahme()
{
  beutelAnfangsZeit = millis();
  kartuscheAnfangsZeit = millis();
  Serial.println("Ich bin in enter_probenahme.");
  flagAusgabeTimer = true;
  ausgabe();
  timerAlarmEnable(timer);  // Timer starten
}
void update_probenahme()
{
  if(flagTimerInterrupt)
  {
    portENTER_CRITICAL(&timerMux);
    flagTimerInterrupt = false;
    portEXIT_CRITICAL(&timerMux);
    ausgabe();
  }

  // Richtig komplizierte Logik
  if(empfangenData.flagDiesel)    // bei einem Dieselfahrzeug
  {
    if(flagFertigKartusche && flagFertigBeutel )
    {
      probenahmeSystem.transitionTo(fertig);
    }
  }
  else      // wenn kein Diesel
  {
    if(flagFertigBeutel)
    {
      probenahmeSystem.transitionTo(fertig);
    }
  }


  switch(stateBeutel)    // Zustandsautomat Beutelbefüllung
  {
    //enum StateBeutel {beutelSpuelen, beutel1Fuellen, beutel2Fuellen, beutel3Fuellen, beutelParkplatz, beutel4Fuellen, beutelUmstecken, beutel5Fuellen, beutelMessleitung, beutelLeerlauf, beutel6Fuellen, beutel7Fuellen, beutel8Fuellen, beutelFertig};  // Zustände für die Zustandsautomat in probenahme
    case beutelSpuelen:
      if((millis() - beutelAnfangsZeit) <= (spuelzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, LOW);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutel1Fuellen;
        beutelAnfangsZeit = millis();
      }
      break;
    case beutel1Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, HIGH);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutel2Fuellen;
        beutelAnfangsZeit = millis();
      }
      
      
      break;
    case beutel2Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, HIGH);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutel3Fuellen;
        beutelAnfangsZeit = millis();
      }
    break;
    case beutel3Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, HIGH);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutelParkplatz;
        beutelAnfangsZeit = millis();
      }
      break;

    case beutelParkplatz: // kann nur mit Taste übersprungen werden                   
      digitalWrite(magnetventilB1, LOW);
      digitalWrite(magnetventilB2, LOW);
      digitalWrite(magnetventilB3, LOW);
      digitalWrite(magnetventilB4, LOW);
      digitalWrite(magnetventilP1, HIGH); // Damit die Kartuschenweg nicht über den Beutelweg saugt.
      digitalWrite(magnetventilP2, LOW);
      digitalWrite(pumpe10l, LOW);
      break;
    case beutel4Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, HIGH);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutelUmstecken;
      }
      break;
    case beutelUmstecken:
      digitalWrite(magnetventilB1, LOW);
      digitalWrite(magnetventilB2, LOW);
      digitalWrite(magnetventilB3, LOW);
      digitalWrite(magnetventilB4, LOW);
      digitalWrite(magnetventilP1, HIGH); // Damit die Kartuschenweg nicht über den Beutelweg saugt.
      digitalWrite(magnetventilP2, LOW);
      digitalWrite(pumpe10l, LOW);      
      break;
    
    case beutel5Fuellen:  // wird an Position 4 gefüllt, damit die Hintergrundmessung immer Beutel 4 ist
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, HIGH);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutelMessleitung;
      }
      break;

    case beutelMessleitung:
      digitalWrite(magnetventilB1, LOW);
      digitalWrite(magnetventilB2, LOW);
      digitalWrite(magnetventilB3, LOW);
      digitalWrite(magnetventilB4, LOW);
      digitalWrite(magnetventilP1, HIGH); // Damit die Kartuschenweg nicht über den Beutelweg saugt.
      digitalWrite(magnetventilP2, LOW);
      digitalWrite(pumpe10l, LOW);       
      break;


    case beutelLeerlauf:  // Weitersprunglogik bei ausgabe()
      digitalWrite(magnetventilB1, LOW);
      digitalWrite(magnetventilB2, LOW);
      digitalWrite(magnetventilB3, LOW);
      digitalWrite(magnetventilB4, LOW);
      digitalWrite(magnetventilP1, HIGH); // Damit die Kartuschenweg nicht über den Beutelweg saugt.
      digitalWrite(magnetventilP2, LOW);
      digitalWrite(pumpe10l, LOW);  
      break;

    case beutel6Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, HIGH);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutel7Fuellen;
        beutelAnfangsZeit = millis();
      }
      break;

    case beutel7Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, HIGH);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutel8Fuellen;
        beutelAnfangsZeit = millis();
      }
      break;      

      case beutel8Fuellen:
      if ((millis() - beutelAnfangsZeit) <= (fuellzeit_beutel*1000))
      {
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, HIGH);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, LOW);
        digitalWrite(magnetventilP2, HIGH);
        digitalWrite(pumpe10l, HIGH);
      }
      else
      {
        stateBeutel = beutelFertig;
      }
      break;    


    case beutelFertig:
        digitalWrite(pumpe10l, LOW);
        digitalWrite(magnetventilB1, LOW);
        digitalWrite(magnetventilB2, LOW);
        digitalWrite(magnetventilB3, LOW);
        digitalWrite(magnetventilB4, LOW);
        digitalWrite(magnetventilP1, HIGH); // Damit die Kartuschenweg nicht über den Beutelweg saugt.
        digitalWrite(magnetventilP2, LOW);
        flagFertigBeutel = true;
      break;
    default:
        flag_Fehler[probenahmeBeutelFehler] = true;
      break;
  }
  if(empfangenData.flagDiesel)    // bei einem Dieselfahrzeug
  {
    switch(stateKartusche) // Zustandsautomat Kartuschenbefüllung
    {
      // enum StateKartusche {kartuscheSpuelen, kartusche1Fuellen, kartusche2Fuellen, kartusche3Fuellen, kartuscheParkplatz, kartusche4Fuellen, kartuscheUmstecken, kartusche5Fuellen, kartuscheMessleitung, kartuscheLeerlauf, kartusche6Fuellen, kartusche7Fuellen, kartusche8Fuellen, kartuscheFertig};  // Zustände für die Zustandsautomat in probenahme
      case kartuscheSpuelen:
        if((millis() - kartuscheAnfangsZeit) <= (spuelzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, HIGH);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartusche1Fuellen;
          kartuscheAnfangsZeit = millis();
        }
        break;
      case kartusche1Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, HIGH);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartusche2Fuellen;
          kartuscheAnfangsZeit = millis();
        }
        break;
      case kartusche2Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, HIGH);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartusche3Fuellen;
          kartuscheAnfangsZeit = millis();
        }
      break;
      case kartusche3Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, HIGH);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartuscheParkplatz;
          kartuscheAnfangsZeit = millis();
        }
        break;
      case kartuscheParkplatz:
        flagKartuscheIstFertig = true;
        digitalWrite(pumpe1l, LOW);
        digitalWrite(magnetventilK5, LOW);
        digitalWrite(magnetventilK4, LOW);
        digitalWrite(magnetventilK3, LOW);
        digitalWrite(magnetventilK2, LOW);
        digitalWrite(magnetventilK1, LOW);        
        break;
      case kartusche4Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, HIGH);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartuscheUmstecken;
        }
        break;
      case kartuscheUmstecken:
        flagKartuscheIstFertig = true;
        digitalWrite(pumpe1l, LOW);
        digitalWrite(magnetventilK5, LOW);
        digitalWrite(magnetventilK4, LOW);
        digitalWrite(magnetventilK3, LOW);
        digitalWrite(magnetventilK2, LOW);
        digitalWrite(magnetventilK1, LOW);
        break;          
      case kartusche5Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, HIGH);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartuscheMessleitung;
        }
        break;
      case kartuscheMessleitung:
        flagKartuscheIstFertig = true;
        digitalWrite(pumpe1l, LOW);
        digitalWrite(magnetventilK5, LOW);
        digitalWrite(magnetventilK4, LOW);
        digitalWrite(magnetventilK3, LOW);
        digitalWrite(magnetventilK2, LOW);
        digitalWrite(magnetventilK1, LOW);
        break;  
      case kartuscheLeerlauf:
        digitalWrite(pumpe1l, LOW);
        digitalWrite(magnetventilK5, LOW);
        digitalWrite(magnetventilK4, LOW);
        digitalWrite(magnetventilK3, LOW);
        digitalWrite(magnetventilK2, LOW);
        digitalWrite(magnetventilK1, LOW);
        break;  
      case kartusche6Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, HIGH);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartusche7Fuellen;
          kartuscheAnfangsZeit = millis();
        }
        break;
      case kartusche7Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, HIGH);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartusche8Fuellen;
          kartuscheAnfangsZeit = millis();
        }
        break;
      case kartusche8Fuellen:
        if((millis() - kartuscheAnfangsZeit) <= (fuellzeit_kartusche*1000))
        {
          digitalWrite(magnetventilK1, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK3, HIGH);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(pumpe1l, HIGH);
        }
        else
        {
          stateKartusche = kartuscheFertig;
        }
        break;
      case kartuscheFertig:
          digitalWrite(pumpe1l, LOW);
          digitalWrite(magnetventilK5, LOW);
          digitalWrite(magnetventilK4, LOW);
          digitalWrite(magnetventilK3, LOW);
          digitalWrite(magnetventilK2, LOW);
          digitalWrite(magnetventilK1, LOW);
          flagKartuscheIstFertig = true;
          flagFertigKartusche = true; 
        break;
      default:
          flag_Fehler[probenahmeKartuscheFehler] = true;
        break;
    }
  }
  // Serial.println(stateBeutel);           // Zum Debuggen
  // Serial.println(stateKartusche);        // Zum Debuggen
}
void exit_probenahme()
{
  Serial.println("Ich bin in exit_probenahme.");
  digitalWrite(pumpe10l, LOW);              // alles ausschalten
  digitalWrite(pumpe1l, LOW);
  digitalWrite(magnetventilB1, LOW);
  digitalWrite(magnetventilB2, LOW);
  digitalWrite(magnetventilB3, LOW);
  digitalWrite(magnetventilB4, LOW);
  digitalWrite(magnetventilP1, LOW);
  digitalWrite(magnetventilP2, LOW);
  digitalWrite(magnetventilK1, LOW);
  digitalWrite(magnetventilK2, LOW);
  digitalWrite(magnetventilK3, LOW);
  digitalWrite(magnetventilK4, LOW);
  digitalWrite(magnetventilK5, LOW);
  timerAlarmDisable(timer);
}
  // Zustand: fertig
void enter_fertig()
{
  Serial.println("Ich bin in enter_fertig.");
  ausgabe();
}
void update_fertig()
{
  // Serial.println("Ich bin in update_fertig."); // Zum Debuggen
}
void exit_fertig()
{
  Serial.println("Ich bin in exit_fertig.");
}
  // kalibrieren
void enter_kalibrieren()
{
  Serial.println("Ich bin in enter_kalibrieren.");
  ausgabe();
}
void update_kalibrieren()
{
  //Serial.println("Ich bin in update_kalibrieren.");
  switch (stateKalibrieren)
  {
  case 0:
    allesAbschalten();
    break;
  case 1:
    allesAbschalten();
    break;
  case 2:
     digitalWrite(magnetventilK1, HIGH);
     digitalWrite(magnetventilP1, HIGH);  // Damit nicht Luft über den Beutelpfad gesaugt wird
     digitalWrite(pumpe1l, HIGH);
    break;
  case 3:    
    allesAbschalten();
    break; 
  case 4:
    digitalWrite(magnetventilB1, HIGH);
    digitalWrite(magnetventilP2, HIGH);
    digitalWrite(pumpe10l, HIGH);
    break;
  case 5:
    allesAbschalten();
    break;
  case 6:
    digitalWrite(magnetventilB1, HIGH);
    digitalWrite(magnetventilP2, HIGH);
    digitalWrite(pumpe10l, HIGH);
    digitalWrite(magnetventilK1, HIGH);
    digitalWrite(pumpe1l, HIGH);
    break;
  case 7:
    allesAbschalten();
    break;     
  default:
    Serial.println("default im Switch aufgerufen!");
    break;
  }
}
void exit_kalibrieren()
{
  Serial.println("Ich bin in exit_kalibrieren.");
}
  // vorbereiten
void enter_vorbereiten()
{
  Serial.println("Ich bin in enter_vorbereiten.");
  ausgabe();
  timerAlarmEnable(timer);
}
void update_vorbereiten()
{
  if(flagTimerInterrupt)      // vielleicht noch optimieren, wird nicht immer benötigt
  {
    portENTER_CRITICAL(&timerMux);
    flagTimerInterrupt = false;
    portEXIT_CRITICAL(&timerMux);
    ausgabe();
  }
}
void exit_vorbereiten()
{
  Serial.println("Ich bin in exit_vorbereiten");
  timerAlarmDisable(timer);
}
// Ende Funktionen für die Statemachine

void allesAbschalten()
{
    digitalWrite(magnetventilB1, LOW);
    digitalWrite(magnetventilB2, LOW);
    digitalWrite(magnetventilB3, LOW);
    digitalWrite(magnetventilB4, LOW);
    digitalWrite(magnetventilP1, LOW);
    digitalWrite(magnetventilP2, LOW);
    digitalWrite(magnetventilK1, LOW);
    digitalWrite(magnetventilK2, LOW);
    digitalWrite(magnetventilK3, LOW);
    digitalWrite(magnetventilK4, LOW);
    digitalWrite(magnetventilK5, LOW);
    digitalWrite(pumpe10l, LOW);
    digitalWrite(pumpe1l, LOW);
}
// Display Funktion
void displayFunktion()
{
  // Display
  display.init();
  display.flipScreenVertically();
  display.clear();  // clear the display
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  // display.drawStringMaxWidth(64, 0, 128, "Automatisiertes Probenahmesystem");
  // display.drawString(64, 30, "Daimler AG");
  // display.drawString(64, 50, "Julian Schweizerhof");
  // display.display();
  display.clear();  // clear the display
  display.drawStringMaxWidth(64, 0, 128, "Probenahmesystem");
  if(digitalRead(voltage_check) == HIGH)
  {
    display.drawString(64, 30, "12 Volt");
  }
  display.drawString(64, 50, "Adresse: " + IP.toString());
  display.display();
}
// Ende Display Funktion



*/