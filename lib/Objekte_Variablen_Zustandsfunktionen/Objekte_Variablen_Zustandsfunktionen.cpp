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
#include "Objekte_Variablen_Zustandsfunktionen.h" // hier sind auch alle Klassen H Dateien mit drin


//************************************   Objekte ereugen ****************************************************
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);
//************************************   Globale Variablen ***************************************************
  int MotorStatus;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int8_t Statecounter = 0;
  int timerIndex= 0;
  int8_t LastState = 0;
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


FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt


//************************************   Funktionen für den ustansautomat ***********************************
void en_Init()
{
  Serial.println("Initialisierung");
  RB_Dfr_444.setMotorStopp();         //erstmal soll sich nichts bewegen
  GrueneLED.An();
  RoteLED.An();
  Serial.println("getting Ready...");
}
void do_Init()
  {
    if(digitalRead(endschalter_Deckel)==kontakt)
      Spuelautomat . transitionTo(Kalibrierung);
  }

void ex_Init()
  {
    GrueneLED.Aus();
    RoteLED.Aus();
    LastState = 1;
  }

  //Kalibrierung
void en_Kalibrierung()
  {
    GrueneLED.Blinken();
  }
  void do_Kalibrierung()
  {
    if(LastState == 24)
      Spuelautomat.transitionTo(Standby);
    else 
      Spuelautomat.transitionTo(Kalibrierung_Lore_hinten);
  }
  
  void ex_Kalibrierung()
  {
    LastState = 2;
  }

//Kalibrieren Lore Hinten
  void en_Kalibrierung_Lore_hinten()
  {
    RB_Dfr_444.setMotorStart(Lore_ab);
  };
  void do_Kalibrierung_Lore_hinten()
  {
    if(digitalRead(endschalter_Hinten)==kontakt)
    {
      Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
    }

    if(Spuelautomat.timeInCurrentState() > ErrTimeLore_Kalib);
    {
      Spuelautomat.transitionTo(ErrorState);
    }
  };

  void ex_Kalibrierung_Lore_hinten()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 21;
  };

//Kalibrieren Lore Vorne
  void en_Kalibrierung_Lore_vorne()
  {
    RB_Dfr_444.setMotorStart(Lore_auf);
  };
  void do_Kalibrierung_Lore_vorne()
  {
    if(digitalRead(endschalter_Vorne)==kontakt)
    {
      Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
      derEncoder.resetZaehler();
    }
    if(Spuelautomat.timeInCurrentState() > ErrTimeLore_Kalib);
    {
      Spuelautomat.transitionTo(ErrorState);
    }
  };
  void ex_Kalibrierung_Lore_vorne()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 22;
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
    LastState = 23;
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
      Spuelautomat.transitionTo(Kalibrierung);
  };
  void ex_Kalibrierung_Kolben_rein()
  {
    LastState =24;
  };
  //Standby
  void en_Standby()
  {
    derEncoder.resetZaehler();
    RB_Dfr_444.setMotorStopp();
    GrueneLED.An();
    RoteLED.Aus();
  }
  void do_Standby()
  {
    if(Serial.read()==1)
      Spuelautomat.transitionTo(Rakeln);
  }
  void ex_Standby()
  {
    LastState = 3;
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
    else if(digitalRead(endschalter_Hinten)==kontakt||ABS())
      Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Rakeln()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 4;
  }

  //Rakelreinigen
  void en_Rakelreinigen() 
  {
    digitalWrite(blasen, blasenEin);
  }
  void do_Rakelreinigen() 
  {
    if(LastState == 52)
      Spuelautomat.transitionTo(Abstreifen);
    else 
      Spuelautomat.transitionTo(Rakelreinigen_Kolben_raus);
  }

  void ex_Rakelreinigen()
  {
    digitalWrite(blasen, blasenAus);
    LastState = 5;
  }
//Rakelreinigen Kolben raus
 void en_Rakelreinigen_Kolben_raus() 
  {
    digitalWrite(kolben, kolbenRaus);
  }
  void do_Rakelreinigen_Kolben_raus() 
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
      Spuelautomat.transitionTo(Rakelreinigen_Kolben_rein);
    
    else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)== kontakt)
        Spuelautomat.transitionTo(ErrorState);
  }

  void ex_Rakelreinigen_Kolben_raus() 
  {
    LastState = 51;
  }
  //Rakelreinigen Kolben rein
   void en_Rakelreinigen_Kolben_rein() 
  {
    digitalWrite(kolben, kolbenRein);
  }
  void do_Rakelreinigen_Kolben_rein() 
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)== kontakt)
      Spuelautomat.transitionTo(Rakelreinigen);
    
    else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
        Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Rakelreinigen_Kolben_rein() 
  {
    digitalWrite(blasen, blasenAus);
    LastState = 52;
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
      // if(Spuelautomat.timeInCurrentState()>ErrTimeLore_ab_Abstreifen || ABS())  //Andy: So stelle ich mir das vor. ErrTime und ABS Abfrage.
      // Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Abstreifen()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 6;
  }

  //Ausgabe
  void en_Ausgabe()
  {
    RB_Dfr_444.setMotorStart(Lore_auf);  //Andy: Der Motor mag das nicht. Er will bissel chillen.
  }
  void do_Ausgabe()
  {
    if(RB_Dfr_444.getMotorSpeed()==0) //Z.B. so. Da kann er kurz verschnaufen.
      RB_Dfr_444.setMotorStart(Lore_auf);
    if(digitalRead(endschalter_Vorne)==kontakt)
      Spuelautomat.transitionTo(Standby);
  }
  void ex_Ausgabe()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 7;
  }

  //Error
  void en_Error()
  {
    RoteLED.Blinken();
    GrueneLED.Aus();
    RB_Dfr_444.Not_Aus();
    digitalWrite(blasen, blasenAus); 
    Serial.print("irgendwas ist schiefgelaufen :| "); Serial.println(LastState); //Andy: Dann will ich auch gleich wissen was ;D
  }
  void do_Error()
  {
    //Fehlerausgabe
    // if(quitierenEin)

  }
  void ex_Error()
  {
    LastState = 8;
  }

  //Nothalt
  void en_Nothalt()
  {
    RB_Dfr_444.Not_Aus();
    digitalWrite(blasen, blasenAus);
    GrueneLED.Aus();
    RoteLED.An();
    Serial.println("Not Halt");
  }
  void do_Nothalt()
  {
    if(digitalRead(notaus)!=kontakt)
    {  
      switch (LastState)
      {
      case 1:
        Spuelautomat.transitionTo(Init);
        break;
      case 2:
        Spuelautomat.transitionTo(Kalibrierung);
        break;
      case 21:
        Spuelautomat.transitionTo(Kalibrierung_Lore_hinten);
        break;
      case 22:
        Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
        break;
      case 23:
        Spuelautomat.transitionTo(Kalibrierung_Kolben_raus);
        break;
      case 24:
        Spuelautomat.transitionTo(Kalibrierung_Kolben_rein);
        break;
      case 3:
        Spuelautomat.transitionTo(Standby);
        break;
      case 4:
        Spuelautomat.transitionTo(Rakeln);
        break;
      case 5:
        Spuelautomat.transitionTo(Rakelreinigen);
        break;
      case 51:
        Spuelautomat.transitionTo(Rakelreinigen_Kolben_raus);
        break;
      case 52:
        Spuelautomat.transitionTo(Rakelreinigen_Kolben_rein);
        break;
      case 6:
        Spuelautomat.transitionTo(Abstreifen);
        break;
      case 7:
        Spuelautomat.transitionTo(Ausgabe);
        break;
      case 8:
        Spuelautomat.transitionTo(ErrorState);
        break;
        //Kein case 9: da sonst eine endlosschleife entstehen kann
      default:
        break;
      }
    }
  }
  void ex_Nothalt()
  {
    GrueneLED.An();
    RoteLED.Aus();
  }

  void Leer()
  {
    //Nix die ist Leer.
    Serial.println("Welcher Trottel ruft Leer auf?");
  }
//************************************ Ende ******************************************************************


