#include "Encoder.h"
#include "Defines.h"
#include "Arduino.h"

Encoder::Encoder()
{
    zaehler = 0;
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
