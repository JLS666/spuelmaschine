/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für Spülmaschine                             */
/*  Version:      0.1                                               */                 
/********************************************************************/


#include "Defines.h"

//#include "Encoder.h"
//#include "FiniteStateMachine.h"
#include "Zustaende.h"
//#include "Motor.h"

  //************************************   Objekte erezugen ****************************************************/
Motor RB_Dfr_444(motortreiberPWM,motortreiberDIR_A,motortreiberDIR_B);
Encoder derEncoder; 
Zustaende dieZustaende;
//************************************   Globale Variablen ***************************************************/
//int MotorStatus;                  //Max: Können die nicht auch in die Define.h?
int ZyklenZaehler;
unsigned long LoopTime=0;
unsigned long LoopTimeArray[100] = {0};
unsigned long lastTime = 0;
bool timerModus = false;

int8_t i = 0;
int timerIndex= 0;

void encoderEvent();
bool ABS();
//******************************************************************************/
//Zustandsautomat erstellen. Nach Plan in der Drive
//States:
State Init            = State (dieZustaende.en_Init, dieZustaende.do_Init, dieZustaende.ex_Init);
State Kalibrierung    = State (dieZustaende.en_Kalibrierung, dieZustaende.do_Kalibrierung, dieZustaende.ex_Kalibrierung);
State Rakelreinigen   = State (dieZustaende.en_Rakelreinigen,dieZustaende.do_Rakelreinigen,dieZustaende.ex_Rakelreinigen);
State Standby         = State (dieZustaende.en_Standby, dieZustaende.do_Standby, dieZustaende.ex_Standby);
State Rakeln          = State (dieZustaende.en_Rakeln, dieZustaende.do_Rakeln, dieZustaende.ex_Rakeln);
State Abstreifen      = State (dieZustaende.en_Abstreifen, dieZustaende.do_Abstreifen, dieZustaende.ex_Abstreifen);
State Ausgabe         = State (dieZustaende.en_Ausgabe, dieZustaende.do_Ausgabe, dieZustaende.ex_Ausgabe);
State ErrorState      = State (dieZustaende.en_Error, dieZustaende.do_Error, dieZustaende.ex_Error);
State Nothalt         = State (dieZustaende.en_Nothalt, dieZustaende.do_Nothalt, dieZustaende.ex_Nothalt);

FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt
//******************************************************************************/

void setup() {
  //Input Output Setzen.
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); //Andy: Hier könnte ruhig ein Kommentar stehen. Max: ja find ich auch ;)
  Serial.begin(9600);

  Serial.println("Setup Abgeschlossen !");
  dieZustaende.Statemachine_Uebergabe(
                                      &Spuelautomat,        //Das erste ist die Statemachine, danach kommen die Funktionen
                                      &Init,
                                      &Kalibrierung, 
                                      &Rakelreinigen,
                                      &Standby,
                                      &Rakeln,
                                      &Abstreifen,
                                      &Ausgabe,
                                      &ErrorState,
                                      &Nothalt,
                                      &RB_Dfr_444           //Und zum Schluss noch Andys Motorklasse
                                      );                    //Damit in den Funktionen die Zustände gewchselt werden können müssen alle States hier einmal übergeben

}

void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi

  Spuelautomat.update();
  if(timerModus)  //Loop Geschwindigkeit

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
