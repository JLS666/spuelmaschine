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
    Time=micros();
}
void Encoder::dekrementZaehler()
{
    zaehler--;
    Time=micros();
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
#define totzeit 10000
float Encoder::getGeschwindigkeitMicros()
{
    if(micros() >= letzteZeit + totzeit)        // Sicherheitsfunktion um eine durch 0 Teilung zu verhindern
    {
        //geschwindigkeit = ( (zaehler - letzterZaehler) / (1.0 * (micros() - letzteZeit)) ) * 1000;
        geschwindigkeit = ( (zaehler - letzterZaehler) / (1.0 * (micros() - letzteZeit)) ) * 1000 * 1000 * StreckeProEncoderWert;
        letzterZaehler = zaehler;
        letzteZeit = micros();
    }
    return abs(geschwindigkeit);
}
float Encoder::getGeschwindigkeitMicrosSuperduper()
{
    Serial.println("time ist: " + (String)Time);
    if(Time != altTime)
    {
        float erg = (zaehler-letzterZaehler) * StreckeProEncoderWert * 10000000 / (1.0 * (Time-altTime));
        //erg=erg/abs(zaehler-letzterZaehler+1);
        letzterZaehler=zaehler;
        
        altTime=Time;
        return erg;
    }
    else
    {
        return 0;
    }
    
}
