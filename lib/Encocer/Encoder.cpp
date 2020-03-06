#include "Encoder.h"
#include "Defines.h"
#include "Arduino.h"

Encoder::Encoder()
{
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