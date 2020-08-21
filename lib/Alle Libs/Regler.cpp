
#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
#include "Regler.h"
#include <Arduino.h>
//Regler::Regler():pMotorregler(&Eingabe, &Ausgabe, &Regelwert, 0.06, 0.005, 0.001, DIRECT)
Regler::Regler():pMotorregler(&Eingabe, &Ausgabe, &Regelwert,  0.15, 0.04, 0.07, DIRECT) 
{
    pMotorregler.SetMode(AUTOMATIC);
    pMotorregler.SetOutputLimits(0,MotSpeed);
}

double Regler::Regeln(int pReglerwert)
{
    Eingabe=WieSchnellBinIch();
    Regelwert=pReglerwert;
    pMotorregler.Compute(); //Magic
    return Ausgabe;
}

double Regler::WieSchnellBinIch()
{
    //return derEncoder.getGeschwindigkeitMicros();
    //return derEncoder.getGeschwindigkeitMicrosSuperduper();
    //return Glatten(derEncoder.getGeschwindigkeitMicros());
    return Glatten(derEncoder.getGeschwindigkeitMicrosSuperduper() );

}
double Regler::Notiz()
{
    //Serial.print("Soll Gesch. = "); Serial.println(Regelwert);
    //Serial.print("Gemessene Gesch. = "); Serial.println(Eingabe);
    //Serial.print("Debug: Es wird geregelt mit: "); Serial.println(Ausgabe);

    //CSV
    if(Serial.availableForWrite()<20)
        return Eingabe;
    //Zeit, Soll, Ist, Regler; Sprungantwort plotten.
    Serial.print(millis());Serial.print(";");
    Serial.print(Regelwert);Serial.print(";");Serial.print(Eingabe);Serial.print(";");Serial.println(Ausgabe);
    //Serial.println(Eingabe);
    return Eingabe;
}

double Regler::Glatten(double IN)
{
    /* static bool Flag=0;
    if(Flag)
    Arr[]=... */
    //int Lange=sizeof(Arr);
    double Sum=0;
    //Serial.println(Lange);
    for(int i=0; i<(Lange);i++)
    {
        Arr[i]=Arr[i+1];
        //Serial.println(Arr[i]);
        Sum+=Arr[i];
    }
    Arr[Lange]=IN;
    Sum+=IN;
    //Serial.println(Sum);
    return(Sum/Lange);

}