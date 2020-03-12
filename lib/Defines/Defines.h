/********************************************************************/
/*  Autoren:      Andreas Bank, Anselm Maximilian Lehmann           */
/*                Julian Schweizerhof                               */                 
/*  Firma:        Hochschule Karlsruhe - Technik u. Wirtschaft      */
/*  Datei:        Defines.h                                         */
/*  Beschreibung: Defines für Spülmaschine                          */
/*  Version:      0.1                                               */                 
/********************************************************************/
/*
Seeed Shield für Arduino:
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
#define endschalter_Deckel 
#define quitieren
#define ventil1 A0
#define ventil2 A1
#define notaus A2
#define led_Rot A3                 
#define led_Gruen A4

#define MotorRechts false       //Andy: false ist durch den Motortreiber und die Klasse Motor vorgegeben.
#define MotorLinks !MotorRechts //Andy: Kann man so invertieren? //Max: ja, etwas ungewohnt aber
#define Lore_nachHinten MotorRechts    //Vorne ist die Plattenaufnahme Position.
#define Lore_Zurueck MotorLinks         //Max: "Zurueck" find ich nicht so gut als begriff, mach lieber "vorfahren", "vor"
#define Ramp 300                //in ms bis MotSpeed Rampenzeit //Max: da sind wir schon durchgefahren bis die Lore auf max. v ist
#define MotSpeed 30             //in Prozent       Niemals über 100% gehen! sonst wird Andy böse

#define Error -1
#define Ok 0
#define kontakt true //Was macht der Endschalter bei Kontakt. //Julian: Ich würde false sagen!!, wegen Pull-Up Wiederständen

// Zeiten und Grenzwerte    Andy:Ramp ist auch eine Zeit!
#define SollEncoderWert 444 //Encoder in Mitte
#define Tolleranz 66 //Tolleranz des Encoders +-



//***********************************   Funtkionen bekannt machen *******************************************/
  void en_Init();
  void en_Blasen();
  void do_Standby();
  void Leer();

  void encoderEvent();
  bool ABS();
