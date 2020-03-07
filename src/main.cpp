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
  Motor RB_Dfr_444; //Obj erstellen. Bitte Auskommentieren wenn was am Motor nicht geht. //@Andy, wie naiv bist du eigentlich?
  Encoder derEncoder;

   //***********************************   Funtkionen bekannt machen*******************************************/
  void do_Init();
  void en_Blasen();
  void do_Standby();

  void encoderEvent();
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

  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING);
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


  void encoderEvent()
  {
    if(encoderB)
      derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
  }