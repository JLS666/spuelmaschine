#include <Arduino.h>
#include "Defines.h"
#include "Zustandsautomat.h"
#include <EEPROM.h>

//************************************   Objekte erezugen ****************************************************/
  Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);


    //******************************************************************************/
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
  //******************************************************************************/


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
//******************************************************************************/



void encoderEvent() //ISR
{
   // noInterrupts();           // disable all interrupts  Andy:Braucht man das? wenn ja mach es rein!
    if(encoderB)
      derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
    //  interrupts();
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

ISR(TIMER2_COMPA_vect){    //This is the interrupt request
  OnBoardLED.Flashen();
  OnBoardLED.refresh();     //Kann auch in die loop
  RoteLED.Flashen();
  RoteLED.refresh();        
  GrueneLED.Flashen();
  GrueneLED.refresh();      
};

int Zyklenzaeler(bool Increment) //mit True aufrufen um Hochzuzählen.
{
  static int RAM=0;
  int ROM=EEPROM.get(0,ROM);
  if(Increment){
    ROM++;
    RAM++;
  }
  EEPROM.put(0,ROM);
  delay(100); // Zum Speichern. Sollte nix stören. Wird nur 1mal pro Zyklus aufgerufen. Und Encoder läuft über Interrupt.
  Serial.print("Bereits "); Serial.print(ROM);Serial.println(" Zyklen insgesammt bearbeitet.");
  Serial.print("Bereits "); Serial.print(RAM);Serial.println(" Zyklen seit letztem Neustart bearbeitet.");
  return RAM;
};