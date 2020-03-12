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
#include "Motor.h"
#include "Encoder.h"
#include "FiniteStateMachine.h"



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
  State Init        = State (en_Init,Leer,Leer); // Andy hier sind Fehler bei mir!
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
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen. Max: ja find ich auch ;)
  Serial.begin(9600);

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





  //******************************************************************************/
  
  Spuelautomat.update();        //Zustandsautomat
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.

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
//Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooploop

//******************************************************************************/
  //Aktionen:
  //Init
  void en_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStopp();         //erstmal soll sich nichts bewegen

    //Zylinder ein
    digitalWrite(led_Gruen, true);
    digitalWrite(led_Rot, true);
    digitalWrite(Kolben, Kolben_rein);

    //Wir warten auf den Start.
    Serial.println("getting Ready...");
    
    
  }
  void do_Init()
  {;}
  void ex_Init()
  {;}

  //Kalibrierung
  void en_Kalibrierung()
  {}
  void do_Kalibrierung()
  {
    unsigned long maxtime = millis();
    //Kalibrierung Hinten
    while(digitalRead(endschalter_Hinten)==0)
    {
      RB_Dfr_444.setMotorStart(Lore_ab);
      digitalRead(endschalter_Hinten);
      if((maxtime-millis())<10000)
        {}
    }
  }
  
  void ex_Kalibrierung()
  {}
  //Standby

  //Rakeln

  //Rakel reinigen
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
