/* 
LED Blinken.
Autor: Andy 

Anleitung:
-LED Objetk erstellen
-Timerfuktion erstellen sie ganz unten.!!!
-Aufruf mit Obj.An();   ({Aus,An,Blinken,SchnellBlinken})
*/
#pragma once
//#ifndef <Arduino.h>
#include <Arduino.h>

class LED
{
    public:
    LED(int Pin)
    {
        State=sAus;
        LEDPin=Pin;
        pinMode(LEDPin,OUTPUT);
        TimerSetup();
    }
    LED(int Pin, int pPower) //Leistung von 0-255.
    {
        State=sAus;
        LEDPin=Pin;
        pinMode(LEDPin,OUTPUT);
        TimerSetup();
        Power=pPower;
    }
    void An() //Macht die LED An.
    {
        State=sAn;
    };
    void Aus() //Selbsterklärend
    {
        State=sAus;
    };
    void Blinken() //Blinkt in Faktor "Schnellfaktor" langsamer als Schnell. 50% Duty.
    {
        State=sBlinken;
    };
    void SchnellBlinken() //Selbsterklärend
    {
        State=sSchnellBlinken;
    };
    char getState() //Gibt Status zurück.
    {
        return State;
    };
    void refresh() //Updatet den Status. In Loop oder ISR aufrufen!
    {
        switch (State)
        {
        case sAus:
            analogWrite(LEDPin,0);
            break;
        case sAn:
            analogWrite(LEDPin,Power);
            break;
        case sBlinken:
            if((Flash>Blinkfrequenz*SchnellFaktor/2)) //Geilon
            {
                analogWrite(LEDPin,Power);
            }
            else 
            {
                analogWrite(LEDPin,0);
            }
            break;
        case sSchnellBlinken:
            if(Flash/(Blinkfrequenz/2)%2)
            {
                analogWrite(LEDPin,Power);
            }
            else
            {
                analogWrite(LEDPin,0);
            }
            break;
        
        default:
            break;
        }
        
    };
    void Flashen() //1ms Periode. Aufruf in ISR.
    {
        Flash++;
        if(Flash>Blinkfrequenz*SchnellFaktor)
        Flash=1;
    };



    private:
    enum {sAus,sAn,sBlinken,sSchnellBlinken};
    int LEDPin;
    int Blinkfrequenz=100; //in mHz (Schnelles Blinken)
    int SchnellFaktor=3; //x mal so langsam
    int State=0;
    int Flash=1;
    int Power=255;
    void TimerSetup() // UNO Spezifisch 
    {   
        //https://www.simsso.de/?type=arduino/timer-interrupts
        //verdammt gute Seite Junge         
        cli();           // disable all interrupts
        TCCR2A = 0; // set TCCR2A register to 0
        TCCR2B = 0; // set TCCR2B register to 0
        TCNT2  = 0; // set counter value to 0
        
        OCR2A = 156; // set compare match register    =1ms Bei UNO 
        TCCR2B |= (1 << CS22) | (1 << CS20);         // 1:1024 prescaling for timer 2
        TCCR2A |= (1 << WGM21); // turn on CTC mode
        TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
        sei();             // enable all interrupts
    };
};


//ISR((TIMER2_COMPA_vect));

// ISR(TIMER2_COMPA_vect){    //This is the interrupt request
//   GrueneLED.Flashen();
//   GrueneLED.refresh();     //Kann auch in die loop
// };
