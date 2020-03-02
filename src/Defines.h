/*
Seeed Shield für Arduino:
- UART: RX(0), DX(1) --> Start und Ende vom Stefan
- D2: D2, D3 --> Endschalter1
- D3: D3, D4 --> Endschalter2
- D4: D4, D5 --> Endschalter3
- D5: D5, D6 --> Motortreiber
- D6: D6, D7 --> 
- D7: D7, D8 --> Encoder
- D8: D8, D9 --> Not-Aus
- A0: A0, A1 --> Ventiltreiber
- A1: A1, A2
- A2: A2, A3
- A3: A3. A4
- 4 x I2C: SDA, SCL
*/


#define startPin 0
#define endePin 1
#define endschalter1 2
#define endschalter2 3
#define endschalter3 4
#define motortreiberPWM 5
#define motortreiberDIR 6
#define encoder1 7
#define encoder2 8
#define notaus 9
#define ventil1 A0
#define ventil2 A1



