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
#include "GlobaleObjekte.h"
#include "Zustandsautomat.h"

//#include "Motor.h"
//#include "Encoder.h"
//#include "FiniteStateMachine.h"
//#include "LED.h"
#include <EEPROM.h>



  //************************************   Objekte erezugen ****************************************************/
/*   Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
  Encoder derEncoder; 
  LED OnBoardLED(13), GrueneLED(led_Gruen), RoteLED(led_Rot);
 */
  //************************************   Globale Variablen ***************************************************/
  int MotorStatus;
  unsigned long LoopTime=0;
  unsigned long LoopTimeArray[100] = {0};
  unsigned long lastTime = 0;
  bool timerModus = false;
  int8_t Statecounter = 0;
  int timerIndex= 0;


  //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
 /* State Init            = State (en_Init,do_Init,ex_Init);
  State Kalibrierung    = State (Leer, Leer, Leer);
  State Rakelreinigen   = State (Leer,Leer,Leer);
  State Standby         = State (do_Standby);
  State Rakeln          = State (Leer);
  State Abstreifen      = State (Leer);
  State Ausgabe         = State (Leer); 
  State ErrorState      = State (Leer);
  
  FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt  */
  //******************************************************************************/


void setup() {
  //Input Output Setzen. Andy: Wenn julian ne Schalter/Ventil Klasse macht brauchen wir das nicht.
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen. Max: ja find ich auch ;)
  Serial.begin(9600);
  OnBoardLED.SchnellBlinken();  //Andy: Kleines Beispiel für ne LED
  Serial.println("Setup Abgeschlossen !");


  pinMode(startPin, INPUT_PULLUP);       // HILFE INPUT PULLUP ODER NUR INPUT??????????????????????????????????????????????????????????
  pinMode(endePin, OUTPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  // Motortreiber werden extern gemacht, siehe Motor Klasse, finde ich doof!!!!
  pinMode(endschalter_Vorne, INPUT_PULLUP);
  pinMode(endschalter_Hinten, INPUT_PULLUP);
  pinMode(endschalter_Zylinder, INPUT_PULLUP);
  pinMode(endschalter_Deckel, INPUT_PULLUP);
  pinMode(quitieren, INPUT_PULLUP);
  pinMode(kolben, OUTPUT);
  pinMode(blasen, OUTPUT);
  pinMode(notaus, INPUT_PULLUP);
  pinMode(led_Rot, OUTPUT);
  pinMode(led_Gruen, OUTPUT);
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
  switch (Statecounter)
  {
  case 0:
    Spuelautomat.transitionTo(Init);
    Statecounter++;
    break;
  case 1:
    Spuelautomat.transitionTo(Kalibrierung);
    Statecounter++;
    break;
  case 2:
    Spuelautomat.transitionTo(Standby);
    Statecounter++;
    break;
  case 3:
    Spuelautomat.transitionTo(Rakeln);
    Statecounter++;
    break;
  case 4:
    Spuelautomat.transitionTo(Rakelreinigen);
    Statecounter++;
    break;
  case 5:
    Spuelautomat.transitionTo(Abstreifen);
    Statecounter++;
    break;
  case 6:
    Spuelautomat.transitionTo(Ausgabe);
    Statecounter = 2;
    break;
  
  default:
    break;
  }




  //******************************************************************************/
  
  Spuelautomat.update();        //Zustandsautomat
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.

  //State im Serial anzeigen
  State& getCurrentState(); //Andy: Wie soll das gehen?

  if(timerModus)  //Loop Geschwindigkeit Andy: kann man das schlanker und hübscher machen?

    {
      LoopTime=millis()-LoopTime;
      Serial.print("Loop bearbeitet in (ms): ");
      Serial.println(LoopTime);
    }
    else
    {
      if(timerIndex < 100)
      {
        LoopTimeArray[timerIndex] = micros() - lastTime;
        lastTime = micros();
        timerIndex++;
      }
      else
      {
        for(int j = 0; j<100; j++)
        {
          Serial.println(LoopTimeArray[j]);
        }
        timerIndex= 0;    
      }
    }

}
//Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooploop

//******************************************************************************/
//Aktionen:
  //Init
  /* void en_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStopp();         //erstmal soll sich nichts bewegen

    //Zylinder ein
    digitalWrite(led_Gruen, true);
    digitalWrite(led_Rot, true);
    digitalWrite(kolben, kolbenRein);

    //Wir warten auf den Start.
    Serial.println("getting Ready...");
    
    
  } */
  // /* void do_Init()
  // {;}
  // void ex_Init()
  // {
  //   digitalWrite(led_Gruen, 0);
  //   digitalWrite(led_Rot, 0);
  // }

  // //Kalibrierung
  // void en_Kalibrierung()
  // {}
  // void do_Kalibrierung()
  // {
  //   unsigned long maxtime = millis();
  //   //Kalibrierung Hinten
  //   while(digitalRead(endschalter_Hinten)==0)
  //   {
  //     RB_Dfr_444.setMotorStart(Lore_ab);
  //     if((maxtime-millis())<10000) //Andy: oder so: if(Spuelautomat.timeInCurrentState()>ErrTimeLore_ab_Kalib)
  //       Spuelautomat.immediateTransitionTo(ErrorState);
  //   }
  //   //Encoderzaehler_max = Encoder_now //Max: wenn wir Hinten und Vorne feststellen wollen //Andy: brauchst du net Juli macht das automatisch.

  //   //Kalibrierung Vorne
  //   while(digitalRead(endschalter_Vorne)==0)
  //   {
  //     RB_Dfr_444.setMotorStart(Lore_auf);
  //     if((maxtime-millis())>10000)
  //       Spuelautomat.immediateTransitionTo(ErrorState);
  //   }
  //   //Encoderzaehler_min = Encoder_now //Max: wenn wir Hinten und Vorne feststellen wollen
    
  //   //Kalibrierung Kolben
  //   /*
  //   maxtime = millis();
  //   while(digitalRead(endschalter_Zylinder)== 1)
  //   {
  //     digitalWrite(Kolben, Kolben_ausfahren);
  //     if((maxtime-millis())>20000)
  //       Spuelautomat.immediateTransitionTo(ErrorState);
  //   }

  //   while(digitalRead(endschalter_Zylinder)== 0)
  //   {
  //     digitalWrite(Kolben, Kolben_einfahren)
  //     if((maxtime-millis())>20000)
  //       Spuelautomat.immediateTransitionTo(ErrorState);
  //     digitalRead(endschalter_Hinten);
  //     if((maxtime-millis())<10000)
  //       {}
  //   }
  //   */
  // }
  
  // void ex_Kalibrierung()
  // {}
  // //Standby

  // //Rakeln

  // //Rakel reinigen
  // void en_Blasen() 
  // {
  //   Serial.println("entry Blasen");

  // }
  // void do_Standby() 
  // {
  //   Serial.println("Standby");

  // }
  // void Leer()
  // {
  //   //Nix die ist Leer.
  //   Serial.println("Welcher Trottel ruft Leer auf?");
  // } */
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
