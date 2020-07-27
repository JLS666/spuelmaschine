/* 
Regler für konst Fahren. ohne am Motor zu pfuschen.
Autor: Andy 
*/

#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen
//Braucht den Enoder und den Motor
#include "PID_v1.h"
#include "Defines.h"

class Regler
{
    public:
    Regler(); // Erstellt Pid und wird am besten von der Motor Klasse in 2. instruktur erstellt.
    bool Regeln(int pReglewert); //Kontinuierlicher Aufruf wenn Regler aktiv sein soll. Eingabe ist Geschw in mm/s
    ~Regler();
    double Regelwert=0;

    private:
    int WieSchnellBinIch();
    int oldEncoder=0;
    long oldTime=0;
    int maxReglerwert=10; //In Prozent
    double Eingabe=0;
    double Ausgabe=0;
    PID Motorregler;
};

Regler::Regler():Motorregler(&Eingabe, &Ausgabe, &Regelwert, 1, 0.1, 0.1, DIRECT) //Ah cool so geht das.
{
    Motorregler.SetMode(AUTOMATIC);
}

bool Regler::Regeln(int pReglerwert)
{
    Eingabe=WieSchnellBinIch();
    Regelwert=pReglerwert;
    Motorregler.Compute();
    unsigned int Power=int(Ausgabe/pReglerwert)*RB_Dfr_444.getMotorSpeed();
    RB_Dfr_444.changeSpeed(Power);
    Serial.print("Debug: Es wird geregelt mit: "); Serial.println(Power);
    return Ok;
}

int Regler::WieSchnellBinIch()
{
    int Weg=derEncoder.getZaehler()-oldEncoder;
    oldEncoder=derEncoder.getZaehler();
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