/********************************************************************/
/*  Autoren:      Anselm Maximilian Lehmann                         */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        Defines.h                                         */
/*  Beschreibung: Defines                                           */
/*  Version:      0.1                                               */                 
/********************************************************************/

//Pins
#define startPin 0
#define endePin 1
#define encoderA 3              //Interrupt Pin
#define encoderB 2              //Interrupt Pin
#define motortreiberPWM 9       //PWM Pin 
#define motortreiberDIR_A 8     //Dir Pin H=>CW
#define motortreiberDIR_B 4     //Dir Pin H=>CCW
#define endschalter_Vorne 5
#define endschalter_Hinten 6
#define endschalter_Zylinder 7
#define endschalter_Deckel 10                   
#define quittieren 11            
#define kolben A0
#define blasen A1
#define notaus A2
#define led_Rot A3                 
#define led_Gruen A4

#define kontakt LOW 
#define Lore_ab MotorRechts     //Schlitten nach hinten
#define Lore_auf MotorLinks     //Schlitten nach vorne

// Zeiten und Grenzwerte  
#define SollEncoderWert 700                         //Encoder in Mitte
#define EncoderEndeWert 1400                        //Encoder am Ende
#define ErrTimeSchlitten_Kalib 30000                //Error wenn in dieser Zeit nicht am Endschalter unten. Beim Init Kalibirieren.
#define ErrTimeSchlitten_ab_Abstreifen 13000        //Error wenn nach dieser Zeit Schlitten nicht hinten angekommen ist
#define ErrTimeSchlitten_auf_Return 30000           //Error wenn nach dieser Zeit Schlitten nicht vorne angekommen ist
#define ErrTimeKolben_backagain 10000               //Error wenn nach dieser Zeit Düsen nicht eingefahren sind
#define KolbenFahrzeit 6000                         //Zeit bis kolben Raus. x2=Kolben Rein und Raus.