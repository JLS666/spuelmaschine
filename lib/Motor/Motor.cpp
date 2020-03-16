
#include "Motor.h"
#include "Defines.h"
#include <Arduino.h>


    Motor::Motor(int PWM_Pin, int DIR_Pin_A, int DIR_Pin_B)
    {
        sollRichtung=Lore_auf;
        istRichtung=sollRichtung;
        maxSpeed=MotSpeed;
        pinMode(PWM_Pin,OUTPUT);
        pinMode(DIR_Pin_A,OUTPUT);
        pinMode(DIR_Pin_B,OUTPUT);
        setFrequenz(1000); //PWM auf 1kHz zu Beginn
    }                  
    int Motor::Run() //Ramp + Dir
    {
        unsigned long lastTime=0;

        if(istRichtung!=sollRichtung && istSpeed!=0)
        {
            Stopp=true;
        }

        if(istSpeed==maxSpeed)
        return Ok; //Volle Fahrt

        //Serial.println(istSpeed);
        if(istSpeed==0) //Entscheidung Geschw. Bereich
            {
            AusgangsPower(motortreiberPWM,istSpeed);
            AusgangsPower(-1,sollRichtung);
            istRichtung=sollRichtung;
            }
            else if(istSpeed<10/2)
            {
                //nix
                AusgangsPower(motortreiberPWM,0);
            }
            else if(istSpeed>maxSpeed/2) //Maxwert festlegen % !! nicht über 100!!
            {
                istSpeed=maxSpeed/2; //Maximal 
            }
            else
            {
                //Fahren
                AusgangsPower(motortreiberPWM,istSpeed);
            }
        
        if(lastTime+Ramp/100<=millis()) //in 100 Schritten
        {
            lastTime=millis();
            if(Stopp==false)
            {
                istSpeed++;
            }
            else
            {
                if(istSpeed!=0)
                istSpeed--;
            }
        }          
        if(istRichtung!=sollRichtung)
        {
            Serial.println("Fehlerhafter Richtingswechsel, Motor zuerst Stoppen bis zum Halt.");
            return Error;
        }
        return 1; //Hochlaufen
    }               
    void Motor::setMotorStart(bool mRichtung)
    {
        Stopp=false;
        sollRichtung=mRichtung;
        Serial.println("Motor wurde gestartet.");

    } 
    void Motor::setMotorStopp()
    {
        Stopp=true;
        Serial.println("Motor wurde gesoppt.");
    }
    int Motor::getMotorSpeed()  
    {
        return istSpeed;
    }
    bool Motor::getMotoristRichtung()
    {
        return istRichtung;
    }
    void Motor::Not_Aus()
    {
        AusgangsPower(motortreiberPWM,0);
        Stopp=true;
        Serial.println("Motor Not_Aus");
    } 
    void Motor::setFrequenz(int f)
    {
        if (f>10000||f<800)
        {
             Serial.print("Frequenz außerhalb zulässigem Bereich");
             return;
        }
        Frequenz=(16000000/8)/f;
        //Timer 1 => Pin 9+10 in Fast PWM
        TCCR1A = _BV(COM1A1) | _BV(WGM11);              //Kanal A nicht-invertierend, Mode 14  =0b11000010;     // Pin OC1A frei schalten
        TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS11);   //Mode 14, Prescaler = 8    =0b00011001;     // Prescaler auf 01 setzen um ~31.000 Takte zu haben
        ICR1 = Frequenz;                                // TOP auf F fest legen, als besteht jeder Periode aus F Schritten
        //OCR1A = 0;                                    // Dutycycle auf x von F Takte legen !! Trozdem Impulse
        Serial.print("Frequenz ist jezt: "); Serial.println(Frequenz);
    }           
    void Motor::changeSpeed(int speed)
    {
        Serial.println("Motor Speed wurde angepasst.");
        maxSpeed=speed;
    }
    void Motor::AusgangsPower(int Pin, int Power)
    {
        if(Pin==-1)//Power==1 => CW  =Motor_links @BB-VNH3SP30
        {
            digitalWrite(motortreiberDIR_A,Power);
            Power=!Power;
            digitalWrite(motortreiberDIR_B,Power);
        }
        else
        {
            //Power=map(Power,0,100,0,Frequenz); //Power in Prozent to Duty
            Power=(Frequenz/100)*Power;
            //Serial.println(Power);
            analogWrite(Pin,Power);
        }     
    }
    int Motor::Fehlererkennung(){return Ok;}      //Nicht Implementiert.
    void Motor::Bremsen(){}                       //Nicht Implementiert.
    Motor::~Motor()
    {
        //machen wir nicht.
    }  
