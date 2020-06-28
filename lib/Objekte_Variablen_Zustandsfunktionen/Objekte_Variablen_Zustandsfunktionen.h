/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */ 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        Objekte_Variablen_Zustandsfunktionen.h            */
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

extern State Init;                      //Nr.:1
extern State Kalibrierung;              //Nr.:2
extern State Kalibrierung_Lore_hinten;  //Nr.:21
extern State Kalibrierung_Lore_vorne;   //Nr.:22
extern State Kalibrierung_Kolben_raus;  //Nr.:23
extern State Kalibrierung_Kolben_rein;  //Nr.:24
extern State Standby;                   //Nr.:3
extern State Rakeln;                    //Nr.:4
extern State Rakelreinigen;             //Nr.:5
extern State Rakelreinigen_Kolben_raus; //Nr.:51
extern State Rakelreinigen_Kolben_rein; //Nr.:52
extern State Abstreifen;                //Nr.:6
extern State Ausgabe;                   //Nr.:7
extern State ErrorState;                //Nr.:8
extern State Nothalt;                   //Nr.:9

extern FiniteStateMachine Spuelautomat;


// alle globalen Variablen bekannt machen
extern int MotorStatus;
extern unsigned long LoopTime;
extern unsigned long LoopTimeArray[100];
extern unsigned long lastTime;
extern bool timerModus;
extern int8_t Statecounter;
extern int timerIndex;


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

//Rakelreinigen Kolben raus
void en_Rakelreinigen_Kolben_raus();
void do_Rakelreinigen_Kolben_raus();
void ex_Rakelreinigen_Kolben_raus();

//Rakelreinigen Kolben rein
void en_Rakelreinigen_Kolben_rein();
void do_Rakelreinigen_Kolben_rein();
void ex_Rakelreinigen_Kolben_rein();

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
//Absolute Leere
void Leer();

//Nothalt
void en_Nothalt();
void do_Nothalt();
void ex_Nothalt();







