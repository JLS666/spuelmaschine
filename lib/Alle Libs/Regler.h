/* 
Regler für konst Fahren. ohne am Motor zu pfuschen.
Autor: Andy 
*/
#ifndef REGLER_H
#define REGLER_H


//Braucht den Enoder und den Motor
#include "PID_v1.h"
#include "Defines.h"

// #define Regleristda
/* class Motor;
extern Motor RB_Dfr_444; //Geht beim Motor irgendwie nicht.
extern Encoder derEncoder; */
extern int Encoderstand; //muss halt in der Loop geupdatet werden.


class Regler
{
    public:
        Regler();                       // Erstellt Pid und wird am besten von der Motor Klasse in 2. instruktur erstellt.
        double Regeln(int pReglewert);  //Kontinuierlicher Aufruf wenn Regler aktiv sein soll. Eingabe ist Geschw in mm/s
        //~Regler();
        double Regelwert=0;
        double Notiz(); //Gibt nachrichten aus.

    private: //Max: soviel Speicherplatz wie du mit deinen double belegst ist der Arduinospeicher bald voll, ohne das uns die Kommazahlen helfen. // Meine Geschwindkigkeit rechnet nur in float, also die Double sind echt unnötig
        double WieSchnellBinIch();
        int oldEncoder=0;
        long oldTime=0;
        int maxReglerwert=10; //In Prozent
        double Eingabe=0;
        double Ausgabe=0;
        PID pMotorregler;
        double Glatten(double IN);
        static const int Lange=7;
        double Arr[Lange+1]={0};
};

#endif