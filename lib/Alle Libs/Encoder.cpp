#include "Encoder.h"
#include "Defines.h"
#include "Arduino.h"

Encoder::Encoder()
{
    zaehler = 0;
    letzterZaehler = 0;
    letzteZeit = 0;
    geschwindigkeit = 0;
}

Encoder::~Encoder()
{
}
void Encoder::inkrementZaehler()
{
    zaehler++;
}
void Encoder::dekrementZaehler()
{
    zaehler--;
}
void Encoder::resetZaehler()
{
    zaehler = 0;
}
int Encoder::getZaehler()
{
    return zaehler;
}
float Encoder::getGeschwindigkeit()
{
    if(millis() != letzteZeit)        // Sicherheitsfunktion um eine durch 0 Teilung zu verhindern
    {
        geschwindigkeit = ( (zaehler - letzterZaehler) / (1.0 * (millis() - letzteZeit)) );
        letzterZaehler = zaehler;
        letzteZeit = millis();
    }
    return geschwindigkeit;
}
