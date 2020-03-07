/*
Seeed Shield für Arduino:
- UART: RX(0), DX(1) --> Start und Ende vom Stefan
- D2: D2, D3 --> Encoder
- D3: D3, D4 --> -
- D4: D4, D5 --> 
- D5: D5, D6 --> Endschalter Vorne
- D6: D6, D7 --> Endschalter Hinten 
- D7: D7, D8 --> Endschalter Zylinder
- D8: D8, D9 --> Motortreiber
- A0: A0, A1 --> Ventiltreiber
- A1: A1, A2 --> Notaus
- A2: A2, A3 --> -
- A3: A3, A4 --> LEDs
- 4 x I2C: SDA, SCL
*/


#define startPin 0
#define endePin 1
#define encoderA 2          //Interrupt Pin
#define encoderB 3          //Interrupt Pin
#define motortreiberPWM 9  //PWM Pin Andy: möchte lieber die 9+10 haben. // da hast du 9 und 8
#define motortreiberDIR 8   //Dir Pin
#define endschalter_Vorne 5
#define endschalter_Hinten 6
#define endschalter_Zylinder 7
#define ventil1 A0
#define ventil2 A1
#define notaus A2
#define led1 A3
#define led2 A4

#define MotorRechts true
#define MotorLinks !MotorRechts //Andy: Kann man so invertieren?
#define Lore_Vor MotorRechts
#define Lore_Zurueck MotorLinks
#define Ramp 300 //in ms bis MotSpeed Rampenzeit
#define MotSpeed 30 //in Prozent       Niemals über 100% gehen! sonst wird Andy böse

#define Error -1
#define Ok 0
#define kontakt true //Was macht der Endschalter bei Kontakt.

// Zeiten
