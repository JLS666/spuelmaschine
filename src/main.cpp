/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für Spülmaschine                             */
/*  Version:      0.1                                               */                 
/********************************************************************/


#include <Arduino.h>
#include "Defines.h"
#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
#include <EEPROM.h>

void setup() {
  pinMode(startPin, INPUT);       
  pinMode(endePin, OUTPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(endschalter_Vorne, INPUT_PULLUP);
  pinMode(endschalter_Hinten, INPUT_PULLUP);
  pinMode(endschalter_Zylinder, INPUT_PULLUP);
  pinMode(endschalter_Deckel, INPUT_PULLUP);
  pinMode(quitieren, INPUT_PULLUP);
  pinMode(kolben, OUTPUT);
  pinMode(blasen, OUTPUT);
  pinMode(notaus, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); // Interrupt für den Encoder, bei einer steigenden Flanke am Eingang A wird eine ISR ausgelöst
  Serial.begin(9600);
  OnBoardLED.SchnellBlinken();  //Andy: Kleines Beispiel für ne LED
  Serial.println("Setup Abgeschlossen !");

}

void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi

  //******************************************************************************/
  //Transitionen:
  if((Spuelautomat.isInState(Nothalt) && digitalRead(notaus)!=kontakt) || digitalRead(endschalter_Deckel)!=kontakt)     //Wenn Notaus (Öffner) betätigt =>Nothalt // Julian: Häh wenn er im Zustand Notaus ist, soll er in den Notaus springen?
    Spuelautomat.transitionTo(Nothalt); // Julian:              ^ hierfür hab ich doch extra den Define notausEin gemacht????????????????????
  
  //******************************************************************************/
  
  Spuelautomat.update();        //Zustandsautomat
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.

  if(timerModus)  //Loop Geschwindigkeit Andy: kann man das schlanker und hübscher machen? @Julian Julian: bestimmt
  
    {
      LoopTime=millis()-LoopTime;
      Serial.print("Loop bearbeitet in (ms): ");
      Serial.println(LoopTime);
    }
    else
    {
      if(timerIndex < 100)
      {
        LoopTimeArray[timerIndex] = micros() - lastTime;
        lastTime = micros();
        timerIndex++;
      }
      else
      {
        for(int j = 0; j<100; j++)
        {
          Serial.print(LoopTimeArray[j]);Serial.print(" ; ");
        }
        timerIndex= 0;    
      }
    }
  
} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende



void encoderEvent() //ISR
{
    if(encoderB)
      derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
}

bool ABS() //Gibt ein Error zurück wenn die Lore festhängt.
{
  static int Position=0;
  static unsigned long Zeit=0;
  if(RB_Dfr_444.getMotorSpeed()>1 && millis()>Zeit+Ramp/2) 
  {
    Zeit=millis();
    if(Position<=derEncoder.getZaehler()-Tolleranz || Position>=derEncoder.getZaehler()+Tolleranz)
      return Error;
    Position=derEncoder.getZaehler();
    return Ok;       
  }
  else
  {
    return Ok;
  }
}

ISR(TIMER2_COMPA_vect){    //This is the interrupt request
  OnBoardLED.Flashen();
  OnBoardLED.refresh();     //Kann auch in die loop
  RoteLED.Flashen();
  RoteLED.refresh();        
  GrueneLED.Flashen();
  GrueneLED.refresh();      
};

int Zyklenzaeler(bool Increment) //mit True aufrufen um Hochzuzählen.
{
  static int RAM=0;
  int ROM=EEPROM.get(0,ROM);
  if(Increment){
    ROM++;
    RAM++;
  }
  EEPROM.put(0,ROM);
  delay(100); // Zum Speichern. Sollte nix stören. Wird nur 1mal pro Zyklus aufgerufen. Und Encoder läuft über Interrupt. // Julian: Mich stört das hier!!!!
  Serial.print("Bereits "); Serial.print(ROM);Serial.println(" Zyklen insgesammt bearbeitet.");
  Serial.print("Bereits "); Serial.print(RAM);Serial.println(" Zyklen seit letztem Neustart bearbeitet.");
  return RAM;
};