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



  Motor RB_Dfr_444; //Obj erstellen. Bitte Auskommentieren wenn was am Motor nicht geht.
  void do_Init();
  void en_Blasen();
  void do_Standby();
  //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
  State Init        = State (do_Init);
  State BlasenEin   = State (en_Blasen);
  State Standby     = State (do_Standby);
/*State Rakeln      = State ();
  State Abstreifen  = State ();
  State BlasenAus   = State ();
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
  RB_Dfr_444.Run();
  //Not_Aus Test

  //State im Serial anzeigen

}

//******************************************************************************/
  //Aktionen:
  void do_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStart(Lore_Zurueck);
    //Zylinder ein
    //Led an

      //Wir warten auf den Start.
      Serial.println("getting Reay...");
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
  //******************************************************************************/