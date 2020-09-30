/********************************************************************/
/*  Autoren:      Julian Schweizerhof                               */
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        Encoder.h                                         */
/*  Beschreibung: Header der Datei der Klasse Encoder               */
/*  Version:      0.1                                               */                 
/********************************************************************/
#pragma once

class Encoder
{
private:
    int zaehler;                // Zähler des Encoders
    int letzterZaehler;
    unsigned long letzteZeit;
    unsigned long Time=0, altTime=0;
    float geschwindigkeit;
    int altertum=1;
public:
    Encoder();
    ~Encoder();
    void resetZaehler();        // Zähler zurücksetzten, bei Endschalter vorne
    int getZaehler();           // Zähler als GZ zurückgeben
    void inkrementZaehler();    // Zähler um eins erhöhen 
    void dekrementZaehler();    // Zähler um eins erniedrigen
    float getGeschwindigkeit(); // Gibt die Geschwindigkeit in ... zurück
    float getGeschwindigkeitMicros();   // Gibt die Geschwindigkeit in mm / s der Lore zurück
    float getGeschwindigkeitMicrosSuperduper();
};
