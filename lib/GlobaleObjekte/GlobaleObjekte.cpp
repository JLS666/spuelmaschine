#include "GlobaleObjekte.h"
#include "Defines.h"

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