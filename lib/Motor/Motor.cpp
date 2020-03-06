#include "Motor.h"
#include <Defines.h>
#include <Arduino.h>


    Motor::Motor()
    {
        sollRichtung=Lore_Zurueck;
        istRichtung=sollRichtung;
        maxSpeed=MotSpeed;

        //Hier oder muss das in den Setup?
        pinMode(motortreiberPWM,OUTPUT);
        pinMode(motortreiberDIR,OUTPUT);
    }                  
    int Motor::Run() //analogWrite + Ramp +Dir
    {
    
        unsigned long lastTime;

        if(istRichtung!=sollRichtung)
        {
            Stopp=true;
        }

        if(istSpeed==maxSpeed)
        return Ok; //Volle Fahrt
        if(lastTime+Ramp/100<=millis()) //in 100 Schritten
        {
            lastTime=millis();
            if(Stopp==false)
            {
                istSpeed++;
            }
            else
            {
                istSpeed--;
            }

            if(istSpeed==0)
            {
            analogWrite(motortreiberPWM,istSpeed);
            analogWrite(motortreiberDIR,sollRichtung);
            istRichtung=sollRichtung;
            }
            else if(istSpeed<10/2)
            {
                //nix
                analogWrite(motortreiberPWM,0);
            }
            else if(istSpeed>maxSpeed/2) //Maxwert festlegen % !! nicht über 100!!
            {
                istSpeed=maxSpeed/2; //Maximal 
            }
            else
            {
                //Fahren
                analogWrite(motortreiberPWM,istSpeed);
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
/*     void Motor::Not_Aus()
    {
        analogWrite(motortreiberPWM,0);
        Stopp=true;
    } */
    Motor::~Motor()
    {
        //machen wir nicht.
    }  