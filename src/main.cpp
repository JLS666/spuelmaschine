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




void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(ventil1, INPUT_PULLUP);
  digitalWrite(ventil1, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
}