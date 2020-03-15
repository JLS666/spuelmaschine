#include "Zustaende.h"
#include <Arduino.h>

//State Standby = State(&Zustaende::en_Init, &Zustaende::do_Init, &Zustaende::ex_Init);
//State Zustaende::Spuelautomat(Init);

void Zustaende::Statemachine_Uebergabe(
                        FiniteStateMachine *SSpuelautomat, 
                        State *SInit, 
                        State *SKalibrierung,    
                        State *SRakelreinigen,  
                        State *SStandby,         
                        State *SRakeln,         
                        State *SAbstreifen,   
                        State *SAusgabe,   
                        State *SErrorState,
                        State *SNothalt,
                        Motor *SRB_Dfr_444)
{
  *ZSpuelautomat    = *SSpuelautomat;
  *ZInit            = *SInit;
  *ZKalibrierung    = *SKalibrierung;    
  *ZRakelreinigen   = *SRakelreinigen;  
  *ZStandby         = *SStandby;         
  *ZRakeln          = *SRakeln;         
  *ZAbstreifen      = *SAbstreifen;   
  *ZAusgabe         = *SAusgabe;   
  *ZErrorState      = *SErrorState;
  *ZNothalt         = *SNothalt;
  *ZRB_Dfr_444      = *SRB_Dfr_444;
}

void Zustaende::en_Init()
{
  Serial.println("Initialisierung");
  ZRB_Dfr_444->setMotorStopp();         //erstmal soll sich nichts bewegen

  //Zylinder ein
  digitalWrite(led_Gruen, true);
  digitalWrite(led_Rot, true);
  digitalWrite(kolben, kolbenRaus);
  Serial.println("getting Ready...");
}
void Zustaende::do_Init()
  {
    if(digitalRead(endschalter_Deckel)==kontakt)
      ZSpuelautomat -> transitionTo(*ZKalibrierung);
  }

void Zustaende::ex_Init()
  {
    digitalWrite(led_Gruen, 0);
    digitalWrite(led_Rot, 0);
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }

  //Kalibrierung
void Zustaende::en_Kalibrierung()
  {
    ZState_timer=millis();
  }
  void Zustaende::do_Kalibrierung()
  {
    //Kalibrierung Hinten
    while(digitalRead(endschalter_Hinten)==0)
    {
      ZRB_Dfr_444->setMotorStart(Lore_ab);
      if((ZState_timer-millis())<10000)
        ZSpuelautomat->immediateTransitionTo(*ZErrorState);
    }
    //Encoderzaehler_max = Encoder_now //Max: wenn wir Hinten und Vorne feststellen wollen

    //Kalibrierung Vorne
    while(digitalRead(endschalter_Vorne)==0)
    {
      ZRB_Dfr_444->setMotorStart(Lore_auf);
      if((ZState_timer-millis())>10000)
        ZSpuelautomat->immediateTransitionTo(*ZErrorState);
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
  
  void Zustaende::ex_Kalibrierung()
  {
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }

  //Standby
  void Zustaende::en_Standby()
  {
    ZEncoder->resetZaehler();
    ZRB_Dfr_444->setMotorStopp();
  }
  void Zustaende::do_Standby()
  {
    if(Serial.read()==1)
      ZSpuelautomat->transitionTo(*ZRakeln);
  }
  void Zustaende::ex_Standby()
  {
    *Zlast_State = ZSpuelautomat->getCurrentState();
    }

  //Rakeln
  void Zustaende::en_Rakeln()
  {
    ZRB_Dfr_444->setMotorStart(Lore_ab);  
  }
  void Zustaende::do_Rakeln()
  {  
    if(ZEncoder->getZaehler()==SollEncoderWert)
    {
      ZSpuelautomat->transitionTo(*ZRakelreinigen);
    }
  }
  void Zustaende::ex_Rakeln()
  {
    ZRB_Dfr_444->setMotorStopp();
  }

  //Rakelreinigen
  void Zustaende::en_Rakelreinigen() 
  {
    //digitalWrite(Ventil_Duesen, An);
    ZState_timer = 0;
    //digitalWrite(Ventil_Kolben, Kolben_raus);
  }
  void Zustaende::do_Rakelreinigen() 
  {
    //Zeit definieren wie lange der Kolben ausfährt
    /*
      if((ZState_timer-millis)>Ausfahrzeit)   //Max: Hier muss die Zeit für das ausfahrne des Kolbens noch definiert werden
        digitalWrite(Ventil_Kolben, Kolben_rein);
      if(digitalRead(Endschlater_Kolben)==kontakt)
        Spuelautomat->transitionTo(Abstreifen);
    */
  }
  void Zustaende::ex_Rakelreinigen()
  {
    //digitalWrite(Ventil_Duesen, Aus);
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }

  //Abstreifen
  void Zustaende::en_Abstreifen()
  {
    ZRB_Dfr_444->setMotorStart(Lore_ab);
  }
  void Zustaende::do_Abstreifen()
  {
    if(digitalRead(endschalter_Hinten)== kontakt)
      ZSpuelautomat->transitionTo(*ZAusgabe);
  }
  void Zustaende::ex_Abstreifen()
  {
    ZRB_Dfr_444->setMotorStopp();
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }


  //Lore vorne
  void Zustaende::en_Ausgabe()
  {
    ZRB_Dfr_444->setMotorStart(Lore_auf);
  }
  void Zustaende::do_Ausgabe()
  {
    if(digitalRead(endschalter_Vorne)==kontakt)
      ZSpuelautomat->transitionTo(*ZStandby);
  }
  void Zustaende::ex_Ausgabe()
  {
    ZRB_Dfr_444->setMotorStopp();
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }

  //Error
  void Zustaende::en_Error()
  {
    ZRB_Dfr_444->setMotorStopp();
    ZRB_Dfr_444->Fehlererkennung();
  }
  void Zustaende::do_Error()
  {
    //Fehlerausgabe
    Serial.print("irgendwas ist schiefgelaufen :|");
  
  }
  void Zustaende::ex_Error()
  {
    *Zlast_State = ZSpuelautomat->getCurrentState();
  }

  //Nothalt
  void Zustaende::en_Nothalt()
  {
    ZRB_Dfr_444->setMotorStopp();
    digitalWrite(led_Gruen, 0);
    digitalWrite(led_Rot, 1);
  }
  void Zustaende::do_Nothalt()
  {
    ;
  }
  void Zustaende::ex_Nothalt()
  {
    ZSpuelautomat->transitionTo(*Zlast_State);
    digitalWrite(led_Gruen, 1);
    digitalWrite(led_Rot, 0);
  }

  void Zustaende::Leer()
  {
    //Nix die ist Leer.
    Serial.println("Welcher Trottel ruft Leer auf?");
  }