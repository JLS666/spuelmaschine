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
extern State Nothalt;
extern State LastState;

extern FiniteStateMachine Spuelautomat;

// Alle Funktionen des Zustandsautomat bekannt machen
void en_Init();
void do_Init();
void ex_Init();

//Standby
void en_Standby();
void do_Standby();
void ex_Standby();

//Rakeln
void en_Rakeln();
void do_Rakeln();
void ex_Rakeln();

//Rakelreinigen
void en_Rakelreinigen();
void do_Rakelreinigen();
void ex_Rakelreinigen();

//Abstreifen
void en_Abstreifen();
void do_Abstreifen();
void ex_Abstreifen();

//Lore vorne
void en_Ausgabe();
void do_Ausgabe();
void ex_Ausgabe();

//Error
void en_Error();
void do_Error();
void ex_Error();

//Kalibrierung
void en_Kalibrierung();
void do_Kalibrierung();
void ex_Kalibrierung();
void en_Kalibrierung_Lore_hinten();
void do_Kalibrierung_Lore_hinten();
void ex_Kalibrierung_Lore_hinten();
void en_Kalibrierung_Lore_vorne();
void do_Kalibrierung_Lore_vorne();
void ex_Kalibrierung_Lore_vorne();
void en_Kalibrierung_Kolben_raus();
void do_Kalibrierung_Kolben_raus();
void ex_Kalibrierung_Kolben_raus();
void en_Kalibrierung_Kolben_rein();
void do_Kalibrierung_Kolben_rein();
void ex_Kalibrierung_Kolben_rein();
//Abselute Leere
void Leer();

//Nothalt
void en_Nothalt();
void do_Nothalt();
void ex_Nothalt();







