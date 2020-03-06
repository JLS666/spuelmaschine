/*
Seeed Shield für Arduino:
- UART: RX(0), DX(1) --> Start und Ende vom Stefan
- D2: D2, D3 --> Encoder
- D3: D3, D4 --> -
- D4: D4, D5 --> -
- D5: D5, D6 --> Motortreiber
- D6: D6, D7 --> Endschalter1
- D7: D7, D8 --> Endschalter2
- D8: D8, D9 --> Endschalter3
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
#define motortreiberPWM 5   //PWM Pin
#define motortreiberDIR 6   //PWM Pin
#define endschalter_Vorne 7
#define endschalter_Hinten 8
#define endschalter_Zylinder 9
#define ventil1 A0
#define ventil2 A1
#define notaus A2
#define led1 A3
#define led2 A4

#define MotorRechts true
#define MotorLinks !MotorRechts //Andy: Kann man so invertieren?
#define Lore_Vor MotorRechts
#define Lore_Zurueck MotorLinks
#define Ramp 100 //in ms bis MotSpeed Rampenzeit
#define MotSpeed 30 //in Prozent       Niemals über 100% gehen! sonst wird Andy böse

#define Error -1
#define Ok 0
#define kontakt true //Was macht der Endschalter bei Kontakt.
