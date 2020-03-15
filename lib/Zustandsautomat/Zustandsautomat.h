#pragma once

// Alle Headerdateien der Klassen einfügen.
#include "Encoder.h"
#include "Motor.h"
#include "FiniteStateMachine.h"
#include "LED.h"


void en_Init();
void do_Init();
void ex_Init();
void en_Kalibrierung();
void do_Kalibrierung();
void ex_Kalibrierung();
void en_Blasen();
void do_Standby();
void Leer();




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


