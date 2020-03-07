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


  //************************************   Objekte erezugen ***************************************************/
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR);
  Encoder derEncoder;

  //************************************   Globale Variablen ***************************************************/
  int MotorStatus;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int i = 0;

   //***********************************   Funtkionen bekannt machen*******************************************/
  void en_Init();
  void en_Blasen();
  void do_Standby();
  void Leer();

  void encoderEvent();
  //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
  State Init        = State (en_Init,Leer,Leer);
  State BlasenEin   = State (en_Blasen,Leer,Leer);
  State Standby     = State (do_Standby);
/*State Rakeln      = State ();
  State Abstreifen  = State ();
  State BlasenAus   = State ();
  State Return      = State ();
  State ErrorState  = State (); */
  
  FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt
  //******************************************************************************/


void setup() {

  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen.
  Serial.begin(9600);

  Serial.println("Setup Abgeschlossen !");
}

void loop() {
    

  //******************************************************************************/
  //Transitionen:
  if(digitalRead( Spuelautomat.isInState(Init) && endschalter_Hinten)==kontakt && digitalRead(endschalter_Zylinder)==kontakt) //State = Init, Lore=Hinten, Zylinder=drinn.
  Spuelautomat.transitionTo(Standby); //Init to Standbye
  //else if()
  //Spuelautomat.transitionTo(???);




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
  }
  //******************************************************************************/


  void encoderEvent()
  {
    if(encoderB)
      derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
  }