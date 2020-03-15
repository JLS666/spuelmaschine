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
#include "Zustandsautomat.h"

//#include "Motor.h"
//#include "Encoder.h"
//#include "FiniteStateMachine.h"
//#include "LED.h"


  //************************************   Globale Variablen ***************************************************/
  int MotorStatus;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int8_t Statecounter = 0;
  int timerIndex= 0;


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
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen. Max: ja find ich auch ;)
  Serial.begin(9600);
  OnBoardLED.Blinken();  //Andy: Kleines Beispiel für ne LED
  Serial.println("Setup Abgeschlossen !");

}

void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi

  //******************************************************************************/
  //Transitionen:
  if(Spuelautomat.isInState(Init) && digitalRead(endschalter_Hinten)==kontakt && digitalRead(endschalter_Zylinder)==kontakt) //State = Init, Lore=Hinten, Zylinder=drinn.
    Spuelautomat.transitionTo(Standby); //Init to Standbye
  else if(Spuelautomat.isInState(Standby) && digitalRead(startPin)==kontakt)
    Spuelautomat.transitionTo(Rakeln);
  else if(digitalRead(notaus))       //Not_Aus Prüfen und Ausführen.    Andy: Hier ist besser in der Loop der müsste sonnst bei jedem Aktion stehen!
    Spuelautomat.transitionTo(ErrorState);
  //Hier darf Max sich austoben...
  switch (Statecounter)
  {
  case 0:
    Spuelautomat.transitionTo(Init);
    //Statecounter++;
    break;
  case 1:
    Spuelautomat.transitionTo(Kalibrierung);
    Statecounter++;
    break;
  case 2:
    Spuelautomat.transitionTo(Standby);
    Statecounter++;
    break;
  case 3:
    Spuelautomat.transitionTo(Rakeln);
    Statecounter++;
    break;
  case 4:
    Spuelautomat.transitionTo(Rakelreinigen);
    Statecounter++;
    break;
  case 5:
    Spuelautomat.transitionTo(Abstreifen);
    Statecounter++;
    break;
  case 6:
    Spuelautomat.transitionTo(Ausgabe);
    Statecounter = 2;
    break;
  
  default:
    break;
  }




  //******************************************************************************/
  
  Spuelautomat.update();        //Zustandsautomat
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.

  //State im Serial anzeigen
  State& getCurrentState(); //Andy: Wie soll das gehen?

  if(timerModus)  //Loop Geschwindigkeit Andy: kann man das schlanker und hübscher machen?
  
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
  
} // Loop Ende
