
//#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
#include "Regler.h"
#include <Arduino.h>
Regler::Regler():pMotorregler(&Eingabe, &Ausgabe, &Regelwert, 1, 0.1, 0.1, DIRECT) //Ah cool so geht das.
{
    pMotorregler.SetMode(AUTOMATIC);
    pMotorregler.SetOutputLimits(0,MotSpeed);
    
}

double Regler::Regeln(int pReglerwert)
{
    Eingabe=WieSchnellBinIch();
    Serial.print("Gemessene Gesch. = "); Serial.println(Eingabe);
    Regelwert=pReglerwert;
    pMotorregler.Compute(); //Magic
    Serial.print("Debug: Es wird geregelt mit: "); Serial.println(Ausgabe);
    return Ausgabe;
}

int Regler::WieSchnellBinIch()
{
    int Weg=Encoderstand-oldEncoder;
    oldEncoder=Encoderstand;
    long Zeit=millis()-oldTime;
    oldTime=millis();

    if(Zeit>500 || Weg*StreckeProEncoderWert>10)
    {
        Serial.println("Regler neustart");
        return(Ausgabe);
    }
    else
    return((Weg*StreckeProEncoderWert) / (Zeit/1000));
}