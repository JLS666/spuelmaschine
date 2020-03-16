/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */ 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        ObjekteUndZustandsfuntkionen.cpp                  */
/*  Beschreibung: Header für die Objekte und die Zustandsfunktionen */
/*  Version:      0.1                                               */                 
/********************************************************************/


#include <Arduino.h>
#include "Defines.h"                      // werden benötigt
#include "ObjekteUndZustandsfunktionen.h" // hier sind auch alle Klassen H Dateien mit drin


//************************************   Objekte erezugen ****************************************************
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);


//************************************   Objekte für den Zustansautomat erzeugen *****************************
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
  State Init            = State (en_Init,do_Init,ex_Init);
  State Kalibrierung    = State (Leer, Leer, Leer);
  State Rakelreinigen   = State (Leer,Leer,Leer);
  State Standby         = State (do_Standby);
  State Rakeln          = State (Leer);
  State Abstreifen      = State (Leer);
  State Ausgabe         = State (Leer); 
  State ErrorState      = State (Leer);
  
  FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt


//************************************   Funktionen für den Zustansautomat ***********************************
void en_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStopp();         //erstmal soll sich nichts bewegen
    Serial.println("Test");
    //Zylinder ein
    digitalWrite(led_Gruen, true);
    digitalWrite(led_Rot, true);
    digitalWrite(kolben, kolbenRein);

    //Wir warten auf den Start.
    Serial.println("getting Ready...");
    
    
  }

  void do_Init()
  {;}
  void ex_Init()
  {
    digitalWrite(led_Gruen, 0);
    digitalWrite(led_Rot, 0);
  }

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
      if((maxtime-millis())<10000) //Andy: oder so: if(Spuelautomat.timeInCurrentState()>ErrTimeLore_ab_Kalib)
        Spuelautomat.immediateTransitionTo(ErrorState);
    }
    //Encoderzaehler_max = Encoder_now //Max: wenn wir Hinten und Vorne feststellen wollen //Andy: brauchst du net Juli macht das automatisch.

    //Kalibrierung Vorne
    while(digitalRead(endschalter_Vorne)==0)
    {
      RB_Dfr_444.setMotorStart(Lore_auf);
      if((maxtime-millis())>10000)
        Spuelautomat.immediateTransitionTo(ErrorState);
    }
    //Encoderzaehler_min = Encoder_now //Max: wenn wir Hinten und Vorne feststellen wollen
    
    //Kalibrierung Kolben
    /*
    maxtime = millis();
    while(digitalRead(endschalter_Zylinder)== 1)
    {
      digitalWrite(Kolben, Kolben_ausfahren);
      if((maxtime-millis())>20000)
        Spuelautomat.immediateTransitionTo(ErrorState);
    }

    while(digitalRead(endschalter_Zylinder)== 0)
    {
      digitalWrite(Kolben, Kolben_einfahren)
      if((maxtime-millis())>20000)
        Spuelautomat.immediateTransitionTo(ErrorState);
      digitalRead(endschalter_Hinten);
      if((maxtime-millis())<10000)
        {}
    }
    */
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
//************************************ Ende ******************************************************************


