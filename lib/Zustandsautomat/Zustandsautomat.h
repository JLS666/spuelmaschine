#pragma once
// Alle Headerdateien der Klassen einfügen.
#include "Encoder.h"
#include "Motor.h"
#include "FiniteStateMachine.h"
#include "LED.h"


void en_Init();
void ex_Init();
void do_Init();
void en_Kalibrierung();
void do_Kalibrierung();
void ex_Kalibrierung();
void en_Blasen();
void do_Standby();
void Leer();
void encoderEvent();
bool ABS();
ISR(TIMER2_COMPA_vect);
int Zyklenzaeler(bool Increment);



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


