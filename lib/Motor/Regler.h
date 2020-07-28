/* 
Regler für konst Fahren. ohne am Motor zu pfuschen.
Autor: Andy 
*/
#ifndef REGLER_H
#define REGLER_H

#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
//Braucht den Enoder und den Motor
#include "PID_v1.h"
#include "Defines.h"

#define Regleristda 
/* class Motor;
extern Motor RB_Dfr_444; //Geht beim Motor irgendwie nicht.
extern Encoder derEncoder; */
extern int Encoderstand; //muss halt in der Loop geupdatet werden.


class Regler
{
    public:
    Regler(); // Erstellt Pid und wird am besten von der Motor Klasse in 2. instruktur erstellt.
    double Regeln(int pReglewert); //Kontinuierlicher Aufruf wenn Regler aktiv sein soll. Eingabe ist Geschw in mm/s
    ~Regler();
    double Regelwert=0;

    private:
    int WieSchnellBinIch();
    int oldEncoder=0;
    long oldTime=0;
    int maxReglerwert=10; //In Prozent
    double Eingabe=0;
    double Ausgabe=0;
    PID pMotorregler;
};

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
#endif