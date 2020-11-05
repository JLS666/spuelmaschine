#include "Encoder.h"
#include "Defines.h"
#include "Arduino.h"

Encoder::Encoder()
{
    zaehler = 0;
    letzterZaehler = 0;
    aktuelleZeit = 0;
    letzteZeit = 0;
    geschwindigkeit = 0;
    altertum = 1;
}

Encoder::~Encoder()
{
}
void Encoder::inkrementZaehler()
{
    zaehler++;
    aktuelleZeit=micros();
}
void Encoder::dekrementZaehler()
{
    zaehler--;
    aktuelleZeit=micros();
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
    
    if(aktuelleZeit != letzteZeit)
    {
        //Serial.println("time ist: " + (String)Time);
        float erg = (float) (zaehler-letzterZaehler) * StreckeProEncoderWert * 1000000 / (1.0 * (aktuelleZeit-letzteZeit)) ;
        //erg=erg/abs(zaehler-letzterZaehler+1);
        letzterZaehler=zaehler;
        
        letzteZeit=aktuelleZeit;

        //Serial.println(" Erg ist: " + (String) erg);
        geschwindigkeit=abs(erg);
        altertum=1;
        return abs(erg);
    }
    
    else
    {
        if (altertum>10)
            return 0;
        return geschwindigkeit*((11-altertum++)/10);
    }
}
