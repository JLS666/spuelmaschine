/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für Spülmaschine                             */
/*  Version:      0.1                                               */                 
/********************************************************************/

#include <Arduino.h>
#include <Defines.h>
#include "Motor.h"
#include "Encoder.h"
#include <FiniteStateMachine.h>


  //************************************   Objekte erezugen ****************************************************/
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder;

  //************************************   Globale Variablen ***************************************************/
  int MotorStatus;
  int ZyklenZaehler;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int i = 0;

  //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
  State Init        = State (en_Init,Leer,Leer);
  State BlasenEin   = State (en_Blasen,Leer,Leer);
  State Standby     = State (do_Standby);
  State Rakeln      = State (Leer);
  State Abstreifen  = State (Leer);
  State BlasenAus   = State (Leer);
  State Return      = State (Leer);
  State ErrorState  = State (Leer); 
  
  FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt
  //******************************************************************************/


void setup() {
  //Input Output Setzen.
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen.
  Serial.begin(9600);

  Serial.println("Setup Abgeschlossen !");
}

void loop() {
    

  //******************************************************************************/
  //Transitionen:
  if(digitalRead( Spuelautomat.isInState(Init) && endschalter_Hinten)==kontakt && digitalRead(endschalter_Zylinder)==kontakt) //State = Init, Lore=Hinten, Zylinder=drinn.
  Spuelautomat.transitionTo(Standby); //Init to Standbye
  else if(Spuelautomat.isInState(Standby) && digitalRead(startPin)==kontakt)
  Spuelautomat.transitionTo(Rakeln);




  //******************************************************************************/
  
  Spuelautomat.update();
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.
  //Not_Aus Test

  //State im Serial anzeigen

  if(timerModus)  //Loop Geschwindigkeit

    {
      LoopTime=millis()-LoopTime;
      Serial.print("Loop bearbeitet in (ms): ");
      Serial.println(LoopTime);
    }
    else
    {
      if(i < 100)
      {
        LoopTimeArray[i] = micros() - lastTime;
        lastTime = micros();
        i++;
      }
      else
      {
        for(int j = 0; j<100; j++)
        {
          Serial.println(LoopTimeArray[j]);
        }
        i = 0;    
      }
    }
  



}

//******************************************************************************/
  //Aktionen:
  void en_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStart(Lore_Zurueck);
    //Zylinder ein
    //Led an

      //Wir warten auf den Start.
      Serial.println("getting Ready...");
      delay(500);
    
  }
  void en_Blasen() 
  {
    Serial.println("entry Blasen");

  }
  void do_Standby() 
  {
    Serial.println("Standby");

  }
  void Leer()
  {
    //Nix die ist Leer.
    Serial.println("Welcher Trottel ruft Leer auf?");
  }
  //******************************************************************************/


  void encoderEvent() //ISR
  {
    if(encoderB)
      derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
  }