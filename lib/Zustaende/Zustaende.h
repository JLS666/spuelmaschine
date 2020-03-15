/********************************************************************
    Autoren:        Anselm Maximilian Lehmann                       
                                                                                    
    Firma:          Hochschule Karlsruhe - Technik u. Wirtschaft    
    Datei:          Defines.h                                       
    Beschreibung:   Auflistung aller Zustände damit diese nicht die 
                    "main" vollmüllen :)                          
                    Alle Zustände sollten diese drei Aktionen     
                    besitzen auch wenn diese leer sind:
                    en_#
                    do_#
                    ex_#

    Version:      0.1
******************************************************************************
Zustandsautomat erstellen. Nach Plan in der Drive
States:
State Init            = State (en_Init,do_Init,ex_Init);
State Kalibrierung    = State (Leer, Leer, Leer);
State Rakelreinigen   = State (Leer,Leer,Leer);
State Standby         = State (do_Standby);
State Rakeln          = State (Leer);
State Abstreifen      = State (Leer);
State Ausgabe         = State (Leer);
State ErrorState      = State (Leer);
State Nothalt         = State (Leer)

FiniteStateMachine Spuelautomat = FiniteStateMachine(Init); //Eingangsschritt
                                                           
********************************************************************/
#pragma once


#include "Defines.h"
#include "FiniteStateMachine.h"
#include "Encoder.h"
#include "Motor.h"

class Zustaende
{
    public:
        Zustaende();
        ~Zustaende();
        void Statemachine_Uebergabe(
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
                        Motor *SRB_Dfr_444);


         static void en_Init();
         static void do_Init();
         static void ex_Init();

        //Standby
          static void en_Standby();
          static void do_Standby();
          static void ex_Standby();

        //Rakeln
          static void en_Rakeln();
          static void do_Rakeln();
          static void ex_Rakeln();

        //Rakelreinigen
          static void en_Rakelreinigen();
          static void do_Rakelreinigen();
          static void ex_Rakelreinigen();

        //Abstreifen
          static void en_Abstreifen();
           static void do_Abstreifen();
          static void ex_Abstreifen();

        //Lore vorne
          static void en_Ausgabe();
          static void do_Ausgabe();
          static void ex_Ausgabe();

        //Error
          static void en_Error();
          static void do_Error();
          static void ex_Error();

        //Kalibrierung
          static void en_Kalibrierung();
          static void do_Kalibrierung();
          static void ex_Kalibrierung();
        
        //Abselute Leere
        static void Leer();

        //Nothalt
          static void en_Nothalt();
          static void do_Nothalt();
          static void ex_Nothalt();
private:
        static long ZState_timer;
        static State *Zlast_State;
        static State *ZInit;            
        static State *ZKalibrierung;    
        static State *ZRakelreinigen;  
        static State *ZStandby;         
        static State *ZRakeln;         
        static State *ZAbstreifen;   
        static State *ZAusgabe;   
        static State *ZErrorState;
        static State *ZNothalt;


        static FiniteStateMachine *ZSpuelautomat;
        static Encoder *ZEncoder;
        static Motor *ZRB_Dfr_444;
 };




