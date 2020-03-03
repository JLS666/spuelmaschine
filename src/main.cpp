/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann            */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        main.cpp                                          */
/*  Beschreibung: main für Spülmaschine                             */
/*  Version:      0.1                                               */                 
/********************************************************************/

#include <Arduino.h>
#include <Defines.h>
#include "Motor.h"




void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);             //AB: 2 für was gibts die Defines?
  pinMode(ventil1, INPUT_PULLUP); //AB: @JS wer pullups net bracuht lässt es bleiben!
  digitalWrite(ventil1, HIGH);

  Serial.begin(9600);

  Motor RB_Dfr_444; //Obj erstellen. Bitte Auskommentieren wenn was am Motor nicht geht.
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

