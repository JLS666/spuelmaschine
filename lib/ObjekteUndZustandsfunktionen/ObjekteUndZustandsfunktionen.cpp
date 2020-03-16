/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweierhof                               */ 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        ObjekteUndustandsfuntkionen.cpp                  */
/*  Beschreibung: Header für die Objekte und die ustandsfunktionen */
/*  Version:      0.1                                               */                 
/********************************************************************/


#include <Arduino.h>
#include "Defines.h"                      // werden benötigt
#include "ObjekteUndZustandsfunktionen.h" // hier sind auch alle Klassen H Dateien mit drin


//************************************   Objekte ereugen ****************************************************
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);

//************************************   Objekte für den ustansautomat ereugen *****************************
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
State Init                        = State (en_Init, do_Init, ex_Init);
State Kalibrierung                = State (en_Kalibrierung, do_Kalibrierung, ex_Kalibrierung);
State Kalibrierung_Lore_hinten    = State (en_Kalibrierung_Lore_hinten, do_Kalibrierung_Lore_hinten, ex_Kalibrierung_Lore_hinten);
State Kalibrierung_Lore_vorne     = State (en_Kalibrierung_Lore_vorne, do_Kalibrierung_Lore_vorne, ex_Kalibrierung_Lore_vorne);
State Kalibrierung_Kolben_raus    = State (en_Kalibrierung_Kolben_raus, do_Kalibrierung_Kolben_raus, ex_Kalibrierung_Kolben_raus);
State Kalibrierung_Kolben_rein    = State (en_Kalibrierung_Kolben_rein, do_Kalibrierung_Kolben_rein, ex_Kalibrierung_Kolben_rein);
State Rakelreinigen               = State (en_Rakelreinigen,do_Rakelreinigen,ex_Rakelreinigen);
State Standby                     = State (en_Standby, do_Standby, ex_Standby);
State Rakeln                      = State (en_Rakeln, do_Rakeln, ex_Rakeln);
State Abstreifen                  = State (en_Abstreifen, do_Abstreifen, ex_Abstreifen);
State Ausgabe                     = State (en_Ausgabe, do_Ausgabe, ex_Ausgabe);
State ErrorState                  = State (en_Error, do_Error, ex_Error);
State Nothalt                     = State (en_Nothalt, do_Nothalt, ex_Nothalt);
State LastState                   = State (Leer);

FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt


//************************************   Funktionen für den ustansautomat ***********************************
void en_Init()
{
  Serial.println("Initialisierung");
  RB_Dfr_444.setMotorStopp();         //erstmal soll sich nichts bewegen

  //ylinder ein
  digitalWrite(led_Gruen, true);
  digitalWrite(led_Rot, true);
  digitalWrite(kolben, kolbenRaus);
  Serial.println("getting Ready...");
}
void do_Init()
  {
    if(digitalRead(endschalter_Deckel)==kontakt)
      Spuelautomat . transitionTo(Kalibrierung);
  }

void ex_Init()
  {
    digitalWrite(led_Gruen, 0);
    digitalWrite(led_Rot, 0);
    LastState = Spuelautomat.getCurrentState();
  }

  //Kalibrierung
void en_Kalibrierung()
  {
  }
  void do_Kalibrierung()
  {
    Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
    /*
    if(LastState == Kalibrierung_Kolben_rein)
      Spuelautomat.transitionTo(Standby);
    else 
      Spuelautomat.transitionTo(Kalibrierung_Lore_hinten);
    */
  }
  
  void ex_Kalibrierung()
  {
    LastState = Spuelautomat.getCurrentState();
  }

//Kalibrieren Lore Hinten
  void en_Kalibrierung_Lore_hinten()
  {
    RB_Dfr_444.setMotorStart(Lore_ab);
  };
  void do_Kalibrierung_Lore_hinten()
  {
    if(digitalRead(endschalter_Hinten)==kontakt)
      Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
    
    if(Spuelautomat.timeInCurrentState()-millis() > ErrTimeLore_Kalib);
    {
      Spuelautomat.transitionTo(ErrorState);
      LastState = Spuelautomat.getCurrentState();
    }
  };

  void ex_Kalibrierung_Lore_hinten()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = Spuelautomat.getCurrentState();
  };

//Kalibrieren Lore Vorne
  void en_Kalibrierung_Lore_vorne()
  {
    RB_Dfr_444.setMotorStart(Lore_auf);
  };
  void do_Kalibrierung_Lore_vorne()
  {
    if(digitalRead(endschalter_Vorne)==kontakt)
      Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
    
    if(Spuelautomat.timeInCurrentState()-millis() > ErrTimeLore_Kalib);
    {
      Spuelautomat.transitionTo(ErrorState);
      LastState = Spuelautomat.getCurrentState();
    }
  };
  void ex_Kalibrierung_Lore_vorne()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = Spuelautomat.getCurrentState();
  };

//Kalibrieren Kolben raus
  void en_Kalibrierung_Kolben_raus()
  {
    digitalWrite(kolben, kolbenRaus);
  };

  void do_Kalibrierung_Kolben_raus()
  {
      if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!=kontakt)
        Spuelautomat.transitionTo(Kalibrierung_Kolben_rein);
      else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit &&digitalRead(endschalter_Zylinder)==kontakt)
        Spuelautomat.transitionTo(ErrorState);
  };
  void ex_Kalibrierung_Kolben_raus()
  {
    LastState = Spuelautomat.getCurrentState();
  };
  //Kalibrieren Kolben rein
  void en_Kalibrierung_Kolben_rein()
  {
    digitalWrite(kolben, kolbenRein);
  };
  void do_Kalibrierung_Kolben_rein()
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!=kontakt)
      Spuelautomat.transitionTo(ErrorState);
    else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)==kontakt)
      //Spuelautomat.transitionTo(Kalibrierung);
      Spuelautomat.transitionTo(Standby);
  };
  void ex_Kalibrierung_Kolben_rein()
  {
    LastState = Spuelautomat.getCurrentState();
  };
  //Standby
  void en_Standby()
  {
    derEncoder.resetZaehler();
    RB_Dfr_444.setMotorStopp();
  }
  void do_Standby()
  {
    if(Serial.read()==1)
      Spuelautomat.transitionTo(Rakeln);
  }
  void ex_Standby()
  {
    LastState = Spuelautomat.getCurrentState();
    }

  //Rakeln
  void en_Rakeln()
  {
    RB_Dfr_444.setMotorStart(Lore_ab);  
  }
  void do_Rakeln()
  {  
    if(derEncoder.getZaehler()==SollEncoderWert)
    {
      Spuelautomat.transitionTo(Rakelreinigen);
    }
  }
  void ex_Rakeln()
  {
    RB_Dfr_444.setMotorStopp();
  }

  //Rakelreinigen
  void en_Rakelreinigen() 
  {
    digitalWrite(blasen, blasenEin);
    digitalWrite(kolben, kolbenRaus);
  }
  void do_Rakelreinigen() 
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
      digitalWrite(kolben, kolbenRein);
    else if(digitalRead(endschalter_Zylinder)==kontakt)
      Spuelautomat.transitionTo(Abstreifen);
    else if(Spuelautomat.timeInCurrentState()>2*KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
      Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Rakelreinigen()
  {
    digitalWrite(blasen, blasenAus);
    LastState = Spuelautomat.getCurrentState();
  }

  //Abstreifen
  void en_Abstreifen()
  {
    RB_Dfr_444.setMotorStart(Lore_ab);
  }
  void do_Abstreifen()
  {
    if(digitalRead(endschalter_Hinten)== kontakt)
      Spuelautomat.transitionTo(Ausgabe);
  }
  void ex_Abstreifen()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = Spuelautomat.getCurrentState();
  }

  //Lore vorne
  void en_Ausgabe()
  {
    RB_Dfr_444.setMotorStart(Lore_auf);
  }
  void do_Ausgabe()
  {
    if(digitalRead(endschalter_Vorne)==kontakt)
      Spuelautomat.transitionTo(Standby);
  }
  void ex_Ausgabe()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = Spuelautomat.getCurrentState();
  }

  //Error
  void en_Error()
  {
    RB_Dfr_444.setMotorStopp();
    RB_Dfr_444.Fehlererkennung();
  }
  void do_Error()
  {
    //Fehlerausgabe
    Serial.print("irgendwas ist schiefgelaufen :|");
  
  }
  void ex_Error()
  {
    LastState = Spuelautomat.getCurrentState();
  }

  //Nothalt
  void en_Nothalt()
  {
    RB_Dfr_444.setMotorStopp();
    digitalWrite(led_Gruen, 0);
    digitalWrite(led_Rot, 1);
  }
  void do_Nothalt()
  {
    ;
  }
  void ex_Nothalt()
  {
    Spuelautomat.transitionTo(LastState);
    digitalWrite(led_Gruen, 1);
    digitalWrite(led_Rot, 0);
  }

  void Leer()
  {
    //Nix die ist Leer.
    Serial.println("Welcher Trottel ruft Leer auf?");
  }
//************************************ Ende ******************************************************************


