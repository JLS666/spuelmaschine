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
#include <FiniteStateMachine.h>



  Motor RB_Dfr_444; //Obj erstellen. Bitte Auskommentieren wenn was am Motor nicht geht.
  void do_Init();
  void en_Blasen();
  void do_Standby();
  //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
  State Init        = State (do_Init);
  State Blasen      = State (en_Blasen);
  State Standby     = State (do_Standby);
/*State Rakeln      = State ();
  State Abstreifen  = State ();
  State Return      = State ();
  State ErrorState  = State (); */
  
  FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt
  //******************************************************************************/


void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);             //AB: 2 für was gibts die Defines?
  pinMode(ventil1, INPUT_PULLUP); //AB: @JS wer pullups net bracuht lässt es bleiben!
  digitalWrite(ventil1, HIGH);

  Serial.begin(9600);


}

void loop() {
  // put your main code here, to run repeatedly:
  

  //******************************************************************************/
  //Transitionen:
  if(digitalRead(endschalter_Hinten)==kontakt && digitalRead(endschalter_Zylinder)==kontakt) //&&State = Init
  Spuelautomat.transitionTo(Standby); //Init to Standbye





  //******************************************************************************/
  
  Spuelautomat.update();
  RB_Dfr_444.Run();
  //Not_Aus Test

  //State im Serial anzeigen

}

//******************************************************************************/
  //Aktionen:
  void do_Init()
  {
    RB_Dfr_444.setMotorStart(Lore_Zurueck);
    //Zylinder ein
    //Led an

      //Wir warten auf den Start.
      Serial.println("getting Reay...");
      delay(500);
    
  }
  void en_Blasen() 
  {

  }
  void do_Standby() 
  {

  }
  //******************************************************************************/