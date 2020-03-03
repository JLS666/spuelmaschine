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
- A3: A3. A4 --> LEDs
- 4 x I2C: SDA, SCL
*/


#define startPin 0
#define endePin 1
#define encoder1 2
#define encoder2 3
#define motortreiberPWM 5
#define motortreiberDIR 6
#define endschalter1 7
#define endschalter2 8
#define endschalter3 9
#define ventil1 A0
#define ventil2 A1
#define notaus A2
#define led1 A3
#define led2 A4



