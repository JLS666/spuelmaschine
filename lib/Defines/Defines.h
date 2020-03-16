/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        Defines.h                                         */
/*  Beschreibung: Defines für Spülmaschine                          */
/*  Version:      0.1                                               */                 
/********************************************************************/
/*
  //Seeed Shield für Arduino:
- UART: RX(0), DX(1) --> Start und Ende vom Stefan
- D2: D2, D3 --> Encoder
- D3: D3, D4 --> -
- D4: D4, D5 --> Motortreiber
- D5: D5, D6 --> Endschalter Vorne
- D6: D6, D7 --> Endschalter Hinten 
- D7: D7, D8 --> Endschalter Zylinder
- D8: D8, D9 --> Motortreiber
- A0: A0, A1 --> Ventiltreiber
- A1: A1, A2 --> Notaus
- A2: A2, A3 --> -
- A3: A3, A4 --> LEDs
- 4 x I2C: SDA, SCL --> 
- noch Offen: 1 x Stromversorgung, 1 x Endschalter Deckel, 1 x Taster Start, 1 x Taster-Reset
*/
#pragma once
//Pins
#define startPin 0
#define endePin 1
#define encoderA 2              //Interrupt Pin
#define encoderB 3              //Interrupt Pin
#define motortreiberPWM 9       //PWM Pin 
#define motortreiberDIR_A 8     //Dir Pin H=>CW
#define motortreiberDIR_B 4     //Dir Pin H=>CCW
#define endschalter_Vorne 5
#define endschalter_Hinten 6
#define endschalter_Zylinder 7
#define endschalter_Deckel 10   // Beispiel Pin NICHT FINAL!!!!!!!!
#define quitieren 11            // Beispiel Pin NICHT FINAL!!!!!!!!
#define kolben A0
#define blasen A1
#define notaus A2
#define led_Rot A3                 
#define led_Gruen A4
#define schalter_Nothalt 666    //Max: da brauch ich noch einen Pin @Julian

//#define Blasen_ein //Andy: was willst du? ne Funktion wie Blasen_ein(); oder machst du den digitalWrite selber?
#define MotorRechts false       //Andy: false ist durch den Motortreiber und die Klasse Motor vorgegeben.
#define MotorLinks !MotorRechts 
#define Lore_ab MotorRechts    //ab in den Berg. Rein ins dunkle Loch.
#define Lore_auf MotorLinks      
#define Ramp 300                //in ms bis MotSpeed Rampenzeit 
#define MotSpeed 30             //in Prozent       Niemals über 100% gehen! sonst wird Andy böse

#define Error -1
#define Ok 0
#define kontakt true //Was macht der Endschalter bei Kontakt. //Julian: Ich würde false sagen!!, wegen Pull-Up Wiederständen Julian: Soll weg, ein alter Zopf

// Zeiten und Grenzwerte    Andy:Ramp ist auch eine Zeit!
#define SollEncoderWert 444 //Encoder in Mitte
#define EncoderEndeWert 888 //Encoder am Ende
#define Tolleranz 66 //Tolleranz des Encoders +-
#define ErrTimeLore_Kalib 3000 //Error wenn in dieser Zeit nicht am Endschalter unten. Beim Init Kalibirieren.
#define ErrTimeLore_ab_Abstreifen 1300
#define ErrTimeLore_auf_Return 3000
#define ErrTimeKolben_backagain 1000
#define KolbenFahrzeit 600 //Zeit bis kolben Raus. x2=Kolben Rein und Raus.

//***********************************   Funktionen bekannt machen *******************************************

  void encoderEvent();
  bool ABS();
  int Zyklenzaeler(bool Increment);
  

//***********************************   True und False Defines  *********************************************
// Logik: Pinname+Aus oder +Ein oder +Rein oder +Raus
#define blasenEin true
#define blasenAus false
#define kolbenRaus true
#define kolbenRein false
#define notausInOrdnung false
#define notausAlarm true
#define startPinEin true
#define startPinAus false
#define endePinEin true
#define endePinAus false
#define endschalter_VorneEin false  // ist das bei verschiedenen Endschaltern anders? oben habe ich kontakt definiert. Julian: Ja kann man noch zusammen fassen. Ich würde aber Kontakt löschen!!!
#define endschalter_VorneAus true
#define endschalter_HintenEin false
#define endschalter_HintenAus true
#define endschalter_ZylinderEin false
#define endschalter_ZylinderAus true
#define endschalter_DeckelEin false
#define endschalter_DeckelAus true
#define quitierenEin false
#define quitierenAus true

/*
Error Code:

Aus:     Alles ist aus.
Standby: Grün ist an.
Kalibrieren: Grün blinkt.
Am Arbeiten: Grün ist an.
Error State= Rot ist am Blinken.
Not-Halt Rot ist an.
Kalibirunsfehler Lore: Grün blinkt schnell.
Druckluft aus. (Kalibrierung): Rot blinkt schnell.

*/