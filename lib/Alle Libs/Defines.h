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
- UART: RX(0), DX(1) -->
- D2: D2, D3 --> Encoder Frabkodierung: grün: gnd, blau: Vcc, gelb: Ausgang A, weiß: Ausgang B
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
- 1 x I2C: SDA, SCL --> Start und Ende vom Stefan
- 3 x I2C: SDA, SCL --> 
- noch Offen: 1 x Stromversorgung, 1 x Endschalter Deckel, 1 x Taster Start, 1 x Taster-Reset
*/
#pragma once
//Pins
#define startPin 19          //getauscht mit 0,1
#define endePin 18
#define encoderA 3              //Interrupt Pin
#define encoderB 2              //Interrupt Pin
#define motortreiberPWM 9       //PWM Pin 
#define motortreiberDIR_A 4     //Dir Pin H=>CW
#define motortreiberDIR_B 5     //Dir Pin H=>CCW
#define endschalter_Vorne 8   //Achtung Pin change 5-8
#define endschalter_Hinten 6
#define endschalter_Zylinder 7
#define endschalter_Deckel 10   // Beispiel Pin NICHT FINAL!!!!!!!!
#define quittieren 11            // Beispiel Pin NICHT FINAL!!!!!!!!
#define kolben A1
#define blasen A0
#define notaus A2
#define led_Rot A4      // wurde umgetauscht                 
#define led_Gruen A3    // wurde umgetauscht

//#define Blasen_ein //Andy: was willst du? ne Funktion wie Blasen_ein(); oder machst du den digitalWrite selber?
#define MotorRechts false       //Andy: false ist durch den Motortreiber und die Klasse Motor vorgegeben.
#define MotorLinks !MotorRechts 
#define Lore_ab MotorRechts    //ab in den Berg. Rein ins dunkle Loch.
#define Lore_auf MotorLinks      
#define Ramp 100                //in ms bis MotSpeed Rampenzeit 
#define MotSpeed 25             //in Prozent       Niemals über 100% gehen! sonst wird Andy böse
#define RealSpeed 160           //in mm/s Geschwindichkeit.  160
#define RealBesch 1000           //in mm/s^2  soll Beschleunigung der Lore.

#define Error -1
#define Ok 0
#define kontakt LOW //Was macht der Endschalter bei Kontakt. //Julian: Ich würde false sagen!!, wegen Pull-Up Wiederständen Julian: Soll weg, ein alter Zopf

// Zeiten und Grenzwerte    Andy:Ramp ist auch eine Zeit!
#define SollEncoderWert 1100 //Encoder in Mitte
#define EncoderEndeWert 2260 //Encoder am Ende
#define Tolleranz 166 //Tolleranz des Encoders +-
#define ErrTimeLore_Kalib 30000 //Error wenn in dieser Zeit nicht am Endschalter unten. Beim Init Kalibirieren.
#define ErrTimeLore_ab_Abstreifen 13000
#define ErrTimeLore_auf_Return 30000
#define ErrTimeKolben_backagain 10000
#define KolbenFahrzeit 3000 //Zeit bis kolben Raus. x2=Kolben Rein und Raus.
#define StreckeProEncoderWert  405 / 2240  // Fahrweg Lore in mm / Encoder ganz hinten  ehemals: 2200/600

//***********************************   Funktionen bekannt machen *******************************************

  void encoderEvent();
  bool ABS();
  int Zyklenzaehler(bool Increment);
  

//***********************************   Speed Einstellungen *******************************************
#define Norm_Leistung 15
#define Rakel_Leistung 21
#define Relkel_Beginn_Weg 777
#define Relkel_Ende_Weg 888


//***********************************   True und False Defines  *********************************************
// Logik: Pinname+Aus oder +Ein oder +Rein oder +Raus
#define blasenEin HIGH
#define blasenAus LOW
#define kolbenRaus HIGH
#define kolbenRein LOW
#define notausInOrdnung LOW
#define notausAlarm HIGH
#define startPinEin HIGH
#define startPinAus LOW
#define endePinEin HIGH
#define endePinAus LOW
#define endschalter_VorneEin LOW  // ist das bei verschiedenen Endschaltern anders? oben habe ich kontakt definiert. Julian: Ja kann man noch zusammen fassen. Ich würde aber Kontakt löschen!!! Andy: ich bin au verwiert. Kontakt wäre mir klarer.
#define endschalter_VorneAus HIGH
#define endschalter_HintenEin LOW
#define endschalter_HintenAus HIGH
#define endschalter_ZylinderEin LOW
#define endschalter_ZylinderAus HIGH
#define endschalter_DeckelEin LOW
#define endschalter_DeckelAus HIGH
#define quitierenEin LOW
#define quitierenAus HIGH

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