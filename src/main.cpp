/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für Spülmaschine                             */
/*  Version:      1.1                                              */                 
/********************************************************************/


#include <Arduino.h>
#include "Defines.h"
#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
#include <EEPROM.h>

/* Fehler Liste:
Der ist gerade 4 mal gefahren.
ABS geht immer an. Er hällt aber trotzdem nicht an!
Er fährt hemmungslos gegen die _Wand 
Platte sprint raus, Schreibe zu hoch? */


void setup() {
  //pinMode(startPin, INPUT);   // INPUT Pull-Down hardwarebasiert auf Lochraster    
  pinMode(endePin, OUTPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(endschalter_Vorne, INPUT_PULLUP);
  pinMode(endschalter_Hinten, INPUT_PULLUP);
  pinMode(endschalter_Zylinder, INPUT_PULLUP);
  pinMode(endschalter_Deckel, INPUT_PULLUP);
  pinMode(quittieren, INPUT_PULLUP);
  pinMode(kolben, OUTPUT);
  pinMode(blasen, OUTPUT);
  pinMode(notaus, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); // Interrupt für den Encoder, bei einer steigenden Flanke am Eingang A wird eine ISR ausgelöst
  Serial.begin(115200); // haben ja keine Zeit zum trödeln!
  OnBoardLED.SchnellBlinken();  //Andy: Kleines Beispiel für ne LED
  Serial.println("Setup Abgeschlossen !");

}
//Testequitpment:
 int zaehlerAlt = 0;
 unsigned long alteZeit = 0;
 float GeschArr[100];
 int i=0;
 bool ausgabefertig = false;
 bool zustandAlt=0;
 #define anzahlWerte 100

void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi
  //******************************************************************************/
  //Transitionen:
  if(Spuelautomat.isInState(Nothalt)!=true && (digitalRead(notaus)==kontakt || digitalRead(endschalter_Deckel)!=kontakt))   //Wenn Notaus (Öffner) betätigt =>Nothalt 
    {
      Spuelautomat.transitionTo(Nothalt);
      Serial.println("Global Error Deckel/Not-Aus");
    }
    
  //******************************************************************************/
  //Funktionsaufrufe:
  Spuelautomat.update();        //Zustandsautomat
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.
  //ABS();                        //Hallo Ibims der ABS Zyklusaufruf
  //******************************************************************************/
  //Sonstiges:
  static bool zustandAlt=0;
  bool zustand=analogRead(startPin)<startPinEin; //Test mit 200?!
    if(zustand!=zustandAlt){
      zustandAlt=analogRead(startPin)<startPinEin;
      Serial.print(analogRead(startPin));
      Serial.print(" Start =");
      Serial.println(zustand);
    }
} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende



void encoderEvent() //ISR
{
    if(digitalRead(encoderB))
        derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
}

bool ABS() //Gibt ein Error zurück wenn die Lore festhängt. NEU Geschwindichkeitsabhängig.
{
  static unsigned long Zeit=0;
  static double altGesch=minSpeedABS+1;
  static int ABS_Zaeler=0;
  if(RB_Dfr_444.getMotorSpeed()>1 && millis()>(Zeit+Ramp*2)) //durch Ramp*2 Anschaltschwelle begrenzen.
  {
    Zeit=millis();
    if(meinRegler.getEingabe()<minSpeedABS && altGesch<minSpeedABS) //Doppelt falls Verzögerung beim Anfahren.
    {
      altGesch=meinRegler.getEingabe(); //Direkt vom Reglereingang abgreifen.
      Serial.println(" ABS Warnung ");
      ABS_Zaeler++;
      if(ABS_Zaeler>=4)
      {
        Serial.println(" ABS Eingriff !");
        GrueneLED.SchnellBlinken();
        RoteLED.SchnellBlinken(); //Damit man weiß was los ist
        return Error; //Ok= Inaktiv Error=Aktiv
      }
    }
    else
      ABS_Zaeler=0;     
  }
  else if(RB_Dfr_444.getMotorSpeed()<1)
    altGesch=minSpeedABS+1;
  return Ok;
}

ISR(TIMER2_COMPA_vect){    //This is the interrupt request
  OnBoardLED.Flashen();
  OnBoardLED.refresh();     //Kann auch in die loop
  RoteLED.Flashen();
  RoteLED.refresh();        
  GrueneLED.Flashen();
  GrueneLED.refresh();      
};

int Zyklenzaehler(bool Increment) //mit True aufrufen um Hochzuzählen.
{
  static int RAM=0;
  int ROM=EEPROM.get(0,ROM);
  if(Increment){
    ROM++;
    RAM++;
  }
  EEPROM.put(0,ROM);
  delay(100); // Zum Speichern. Sollte nix stören. Wird nur 1mal pro Zyklus aufgerufen. Und Encoder läuft über Interrupt. 
  Serial.print("Bereits "); Serial.print(ROM);Serial.println(" Zyklen insgesammt bearbeitet.");
  Serial.print("Bereits "); Serial.print(RAM);Serial.println(" Zyklen seit letztem Neustart bearbeitet.");
  return RAM;
};