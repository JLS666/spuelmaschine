
#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
#include "Regler.h"
#include <Arduino.h>
Regler::Regler():pMotorregler(&Eingabe, &Ausgabe, &Regelwert, 0.007, 0.001, 0.001, DIRECT) //Ah cool so geht das.
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
    unsigned long Zeit=millis()-oldTime;
    if(Zeit<1)     //50
        return Eingabe;
    oldTime=millis();
    double Zeit2=Zeit;
    //Serial.println("Debug");
    //Serial.println(Zeit);
    //Serial.println(Zeit2);
    int Encoder=derEncoder.getZaehler();
    long Weg=abs(Encoder-oldEncoder);
    oldEncoder=Encoder;
    //Serial.println(Weg);
    //Serial.println((Weg*StreckeProEncoderWert) / (Zeit2/1000));
    if(Zeit>500 || Weg*StreckeProEncoderWert>500)
    {
        Serial.println("Regler neustart");
        return(Ausgabe);
    }
    else
    {
        //return 5;
        return(Glatten((Weg*StreckeProEncoderWert) / (Zeit2/1000)));
    }
}
void Regler::Notiz()
{
    //Serial.print("Soll Gesch. = "); Serial.println(Regelwert);
    //Serial.print("Gemessene Gesch. = "); Serial.println(Eingabe);
    //Serial.print("Debug: Es wird geregelt mit: "); Serial.println(Ausgabe);
    //CSV
    Serial.print(Eingabe);Serial.print(";");Serial.println(Ausgabe);
}

double Regler::Glatten(double IN)
{
    /* static bool Flag=0;
    if(Flag)
    Arr[]=... */
    //int Lange=sizeof(Arr);
    double Sum=0;
    //Serial.println(Lange);
    for(int i=0; i<Lange;i++)
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