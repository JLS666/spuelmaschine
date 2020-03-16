/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */ 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        ObjekteUndZustandsfuntkionen.h                    */
/*  Beschreibung: Header für die Objekte und die Zustandsfunktionen */
/*  Version:      0.1                                               */                 
/********************************************************************/
#pragma once

// Alle Headerdateien der Klassen einfügen.
#include "Encoder.h"
#include "Motor.h"
#include "FiniteStateMachine.h"
#include "LED.h"


// alle Objekte mit extern bekannt machen
extern Motor RB_Dfr_444;
extern Encoder derEncoder;
extern LED OnBoardLED, GrueneLED, RoteLED;

extern State Init;
extern State Kalibrierung;
extern State Rakelreinigen;
extern State Standby;
extern State Rakeln;
extern State Abstreifen;
extern State Ausgabe;
extern State ErrorState;

extern FiniteStateMachine Spuelautomat;

// Alle Funktionen des Zustandsautomat bekannt machen
void en_Init();
void do_Init();
void ex_Init();
void en_Kalibrierung();
void do_Kalibrierung();
void ex_Kalibrierung();
void en_Blasen();
void do_Standby();
void Leer();







