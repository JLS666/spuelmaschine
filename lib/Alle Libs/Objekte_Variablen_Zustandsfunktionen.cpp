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
#include "Encoder.h"
#include "Motor.h"
#include "FiniteStateMachine.h"
#include "LED.h"
// Damit ist regler weg. #include "Regler.h"


//************************************   Objekte ereugen ****************************************************
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);
  Regler meinRegler; 
//************************************   Globale Variablen ***************************************************
  int MotorStatus;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int8_t Statecounter = 0;
  int timerIndex = 0;
  int8_t LastState = 0;
  int Encoderstand = 0;
//Zustandsautomat erstellen. Nach Plan in der Drive
//States:
State Init                        = State (en_Init, do_Init, ex_Init);
State Kalibrierung                = State (en_Kalibrierung, do_Kalibrierung, ex_Kalibrierung);
State Kalibrierung_Lore_hinten    = State (en_Kalibrierung_Lore_hinten, do_Kalibrierung_Lore_hinten, ex_Kalibrierung_Lore_hinten);
State Kalibrierung_Lore_vorne     = State (en_Kalibrierung_Lore_vorne, do_Kalibrierung_Lore_vorne, ex_Kalibrierung_Lore_vorne);
State Kalibrierung_Kolben_raus    = State (en_Kalibrierung_Kolben_raus, do_Kalibrierung_Kolben_raus, ex_Kalibrierung_Kolben_raus);
State Kalibrierung_Kolben_rein    = State (en_Kalibrierung_Kolben_rein, do_Kalibrierung_Kolben_rein, ex_Kalibrierung_Kolben_rein);
State Rakelreinigen               = State (en_Rakelreinigen,do_Rakelreinigen,ex_Rakelreinigen);
State Rakelreinigen_Kolben_raus   = State (en_Rakelreinigen_Kolben_raus,do_Rakelreinigen_Kolben_raus,ex_Rakelreinigen_Kolben_raus);
State Rakelreinigen_Kolben_rein   = State (en_Rakelreinigen_Kolben_rein,do_Rakelreinigen_Kolben_rein,ex_Rakelreinigen_Kolben_rein);
State Standby                     = State (en_Standby, do_Standby, ex_Standby);
State Rakeln                      = State (en_Rakeln, do_Rakeln, ex_Rakeln);
State Abstreifen                  = State (en_Abstreifen, do_Abstreifen, ex_Abstreifen);
State Ausgabe                     = State (en_Ausgabe, do_Ausgabe, ex_Ausgabe);
State ErrorState                  = State (en_Error, do_Error, ex_Error);
State Nothalt                     = State (en_Nothalt, do_Nothalt, ex_Nothalt);


FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt


//************************************   Funktionen für den Zustansautomat ***********************************
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
  if(digitalRead(endschalter_Deckel)==kontakt) //Andy: Wird doch eh in der main abgeragt. Max: hier muss Startpin abfrage noch rein!!
  {
    if( digitalRead(startPin)== startPinEin)
    Spuelautomat.transitionTo(Kalibrierung); //Zu beginn Start Drücken!
  }
  else if(Serial.read()=='s') //Wenn Deckel offen und s gedrückt.
  {
    Serial.println("Skip the Intro");
    Spuelautomat.transitionTo(Standby); 
  }
}

void ex_Init()
  {
    LastState = 1;
  }

  //Kalibrierung
void en_Kalibrierung()
  {
    GrueneLED.Blinken();
    RoteLED.Aus();
    Serial.println("Kalibierung");
    Serial.println(digitalRead(startPin));
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
    Serial.println("Kalibrieren Lore Hinten");
    RB_Dfr_444.setMotorStart(Lore_ab);
  }     // hier stand mal ; das macht man doch so nicht?
  void do_Kalibrierung_Lore_hinten()
  {
    //meinRegler.Notiz();
    if(digitalRead(endschalter_Hinten)==kontakt)
    {
      Spuelautomat.transitionTo(Kalibrierung_Lore_vorne);
    }
    
    if( (Spuelautomat.timeInCurrentState() > ErrTimeLore_Kalib && digitalRead(endschalter_Hinten)!=kontakt) || ABS() )
    {
      Spuelautomat.transitionTo(ErrorState);
    }
  }

  void ex_Kalibrierung_Lore_hinten()
  {
    Serial.println("Exit Kalibrieren Lore Hinten");
    RB_Dfr_444.setMotorStopp();
    LastState = 21;
  }

//Kalibrieren Lore Vorne
  void en_Kalibrierung_Lore_vorne()
  { 
    Serial.println("Kalibrieren Lore Vorne");
      //RB_Dfr_444.changeSpeed(MotSpeed); //Zeile muss raus wenn ein Regler implementiert wurde!!!! + 3
   }
  void do_Kalibrierung_Lore_vorne()
  {
    if(RB_Dfr_444.getMotorInBewegung()==0)
      RB_Dfr_444.setMotorStart(Lore_auf);
      Serial.println("Kalibrieren Lore Vorne Motor Start");
    }
      
    if(digitalRead(endschalter_Vorne)==kontakt)
    {
      Spuelautomat.transitionTo(Kalibrierung_Kolben_raus);
    }
    if( (Spuelautomat.timeInCurrentState() > ErrTimeLore_Kalib && digitalRead(endschalter_Vorne)!=kontakt) ||ABS() )
    {
      Spuelautomat.transitionTo(ErrorState);
    }
  }
  void ex_Kalibrierung_Lore_vorne()
  {
    RB_Dfr_444.setMotorStopp();
    //RB_Dfr_444.changeSpeed(MotSpeed); //Zeile muss raus wenn ein Regler implementiert wurde!!!!
    LastState = 22;
  }

//Kalibrieren Kolben raus
  void en_Kalibrierung_Kolben_raus()
  {
    Serial.println("Kalibrieren Kolben raus");
    digitalWrite(kolben, kolbenRaus);
  }

  void do_Kalibrierung_Kolben_raus()
  {
      if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!=kontakt)
        Spuelautomat.transitionTo(Kalibrierung_Kolben_rein);
      else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit || (digitalRead(endschalter_Zylinder)==kontakt && Spuelautomat.timeInCurrentState()>KolbenFahrzeit))
        Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Kalibrierung_Kolben_raus()
  {
    LastState = 23;  
  }

  //Kalibrieren Kolben rein
  void en_Kalibrierung_Kolben_rein()
  {
    Serial.println("Kalibrieren Kolben rein");
    digitalWrite(kolben, kolbenRein);
    Serial.println("Druecken");
  }
  void do_Kalibrierung_Kolben_rein()
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!=kontakt)
      Spuelautomat.transitionTo(ErrorState);
    else if(digitalRead(endschalter_Zylinder)==kontakt)
      Spuelautomat.transitionTo(Kalibrierung);
  }
  void ex_Kalibrierung_Kolben_rein()
  {
    LastState =24;
  }
  //Standby
  void en_Standby()
  {
    Serial.println("Standby");
    derEncoder.resetZaehler();
    RB_Dfr_444.setMotorStopp();
    GrueneLED.An();
    RoteLED.Aus();
  }
  void do_Standby()
  {
    if(digitalRead(startPin)==startPinEin) // Gogogo
      Spuelautomat.transitionTo(Rakeln);
  }
  void ex_Standby()
  {
    LastState = 3;
    digitalWrite(endePin, endePinAus); // Denn wir fangen jetzt an.
  }

  //Rakeln
  void en_Rakeln()
  { 
  Serial.println("Rakeln");
   }

  void do_Rakeln()
  {  
    if(RB_Dfr_444.getMotorInBewegung()==0)
      RB_Dfr_444.setMotorStart(Lore_ab); //Jetzt wird geputzt
    else if(derEncoder.getZaehler()>=SollEncoderWert)
    {
      Spuelautomat.transitionTo(Rakelreinigen); // aka Blasen
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
    Serial.println("Haupt-Rakelreinigung");
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
    LastState = 5;                    // Würde den State wie oben weglassen und dann Rekelreinigen 1 und 2 einführen.
  }
//Rakelreinigen Kolben raus
 void en_Rakelreinigen_Kolben_raus() 
  {
    Serial.println("blasen ein");
    digitalWrite(kolben, kolbenRaus);
    digitalWrite(blasen, blasenEin);
  }
  void do_Rakelreinigen_Kolben_raus() 
  {
    if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
      Spuelautomat.transitionTo(Rakelreinigen_Kolben_rein);
    
    else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)== kontakt)
        Spuelautomat.transitionTo(ErrorState); //Diagonse: Kein Druck auf dem Schlauch.
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
    if(digitalRead(endschalter_Zylinder)== kontakt)
      Spuelautomat.transitionTo(Rakelreinigen);
    
    else if(Spuelautomat.timeInCurrentState()>KolbenFahrzeit && digitalRead(endschalter_Zylinder)!= kontakt)
        Spuelautomat.transitionTo(ErrorState);
  }
  void ex_Rakelreinigen_Kolben_rein() 
  {
    digitalWrite(blasen,blasenAus);
    Serial.println("blasen aus");
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
    {
      Serial.println("Springen zur Ausgabe");
      Spuelautomat.transitionTo(Ausgabe); 
    }
      
    else if( (Spuelautomat.timeInCurrentState()>ErrTimeLore_ab_Abstreifen) )//|| ABS() )
    {
      Serial.println("Fehler beim Absteifen!");
      Spuelautomat.transitionTo(ErrorState);
    }
  }
  void ex_Abstreifen()
  {
 
    LastState = 6;
  }

  //Ausgabe
  void en_Ausgabe()
  {
    RB_Dfr_444.setMotorStopp();
  }
  void do_Ausgabe()
  {
    if(RB_Dfr_444.getMotorInBewegung()==0)
      RB_Dfr_444.setMotorStart(Lore_auf);
    else if(digitalRead(endschalter_Vorne)==kontakt)
    {
      Spuelautomat.transitionTo(Standby); //Von Vorne
      digitalWrite(endePin, endePinEin); //Singnal Fertig
    }
    else if( (Spuelautomat.timeInCurrentState()>ErrTimeLore_auf_Return) ) // || ABS() )
    {
      Serial.println("Fehler beim Ausgabe!");
      Spuelautomat.transitionTo(ErrorState);
    }
  }
  void ex_Ausgabe()
  {
    RB_Dfr_444.setMotorStopp();
    LastState = 7;
    Zyklenzaehler(true); //EEPROM mit zählen
    digitalWrite(endePin, endePinEin);
  }

  //Error
  void en_Error()
  {
    RoteLED.Blinken();
    GrueneLED.Aus();
    RB_Dfr_444.Not_Aus();
    digitalWrite(blasen, blasenAus); 
    Serial.println("blasen aus");
    Serial.print("irgendwas ist schiefgelaufen :| "); 
    Serial.println(LastState);
  }
  void do_Error()
  {  }
  void ex_Error()
  {
    LastState = 8;
  }

  //Nothalt
  void en_Nothalt()
  {
    RB_Dfr_444.Not_Aus();
    digitalWrite(blasen, blasenAus);
    Serial.println("blasen aus");
    GrueneLED.Aus();
    RoteLED.An();
    Serial.println("Not Halt");
  }
  void do_Nothalt()
  {
    if(digitalRead(quittieren)==kontakt || Serial.read()=='q') //Andy ändert != zu == habe aber kein Plan. Das darf nicht durchlaufen nur weil kein Schalter angeschlossen ist!
    {  
      Serial.println("Quittierung...");
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


