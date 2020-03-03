#include "Motor.h"
#include "Defines.h"
#include <Arduino.h>


    Motor::Motor()
    {
        sollRichtung=Lore_Zurueck;
        istRichtung=sollRichtung;
    }                  
    bool Motor::Run() //analogWrite + Ramp +Dir
    {
    
        long lastTime;

        if(istSpeed==MotSpeed)
        return Ok;
        if(lastTime+Ramp/100<=millis()) //in 100Schritten
        {
            lastTime=millis();
            if(Stopp==false)
            {
                istSpeed=istSpeed++;
            }
            else
            {
                istSpeed=istSpeed--;
            }

            if(istSpeed==0)
            {
            analogWrite(motortreiberDIR,sollRichtung);
            istRichtung=sollRichtung;
            }
            else if(istSpeed<10)
            {
                //nix
            }
            else if(istSpeed>=MotSpeed/2) //Maxwert festlegen % !! nicht über 100!!
            {
                istSpeed=MotSpeed/2; //Maximal 
            }
            else
            {
                //Fehler
            }
            
            

        }
        
        analogWrite(motortreiberPWM,istSpeed);
        return false;
    }               
    void Motor::setMotorStart(bool mRichtung)
    {
        Stopp=false;
        sollRichtung=mRichtung;

    } 
    void Motor::setMotorStopp()
    {
        Stopp=true;
    }
    void Motor::getMotorSpeed()  
    {

    }
    void Motor::getMotoristRichtung()
    {

    }
    Motor::~Motor()
    {

    }  