/* Die Testmain soll die Lore testen.
Gesch Test.
Sprungantwort.
Reglersettings.
ABS Test.
Musik Test.
Die Lippen müssen für diesen Verusch entfernt werden, damit alles frei läuft.
Zu beginn muss die Lore in den Vorderen anschlag gedrückt werden zum Kalibieren! */

//KP wie man die main zwingt vor der anderen zu laufen.


#include <Arduino.h>
#include "Defines.h"
#include "Motor.h"
#include "Encoder.h"
#include "Regler.h"
#include <EEPROM.h>
#include "LED.h"
#include "Objekte_Variablen_Zustandsfunktionen.h"   // Dekleration alle Objekte, aller globalen Variablen, alle Zustandsfunktionen

#define StartPos 55 //Hier soll die Lore Starten   
#define EndPos 1888 //Hier macht die Lore stopp damit wir nicht diese blöden enden mit drinn haben.

void setup() {
  pinMode(startPin, INPUT);   // INPUT Pull-Down hardwarebasiert auf Lochraster    
  pinMode(endePin, OUTPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(endschalter_Vorne, INPUT_PULLUP);
  pinMode(endschalter_Hinten, INPUT_PULLUP);
  pinMode(endschalter_Zylinder, INPUT_PULLUP);
  pinMode(endschalter_Deckel, INPUT_PULLUP);
  pinMode(quittieren, INPUT_PULLUP);
  pinMode(kolben, OUTPUT);
  pinMode(blasen, OUTPUT);
  pinMode(notaus, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderA), encoderEvent, RISING); // Interrupt für den Encoder, bei einer steigenden Flanke am Eingang A wird eine ISR ausgelöst
  Serial.begin(115200); // haben ja keine Zeit zum trödeln!
  OnBoardLED.SchnellBlinken(); 
  GrueneLED.Blinken();
//Encoder Reset erzwingen (While)

  Serial.println("Setup Abgeschlossen !");
}

 int zaehlerAlt = 0;
 unsigned long alteZeit = 0;
 float GeschArr[100];
 int i=0;
 bool ausgabefertig = false;
 #define anzahlWerte 100

 bool once = true;

int TestNr = 0;
void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi

  //delay(1000);
  //while(Serial.available())
  //  TestNr=Serial.read();
  //TestNr = 5;
  
  while(Serial.available()) 
   {
       String Eingabe =  Serial.readStringUntil('\n');
        if (Eingabe.substring(0, 1) == "S")
        {
          TestNr = Eingabe.substring(1, Eingabe.length()).toInt();
          Serial.println("Eingabe ist " + (String)TestNr);
        }
   }

  switch (TestNr)
  {
    case 0:
        //Nix
        break;
    case 1: //Const Fahren ans Ende
      RB_Dfr_444.changeSpeed(18);
      RB_Dfr_444.setMotorStart(Lore_ab);
      if(derEncoder.getZaehler()>EndPos){
        RB_Dfr_444.setMotorStopp();
        Serial.println("Const Test Ende");
        TestNr=0;
      }
        break;
    case 2: //Const mit Gesch Messung
      if(once)
      {
        RB_Dfr_444.changeSpeed(18);
        RB_Dfr_444.setMotorStart(Lore_ab);
        once = false;
      }
      Serial.println( (String) millis() + ";" + (String) derEncoder.getGeschwindigkeitMicros());
      if(derEncoder.getZaehler()>EndPos){
        RB_Dfr_444.setMotorStopp();
        Serial.println("Const Test Ende");
        TestNr=0;
        once = true;
      }
        break;
    case 3: //Sprungantwort ohne Regler
    RB_Dfr_444.changeSpeed(15);
      RB_Dfr_444.setMotorStart(Lore_ab);
      if(derEncoder.getZaehler()>EndPos/3)
        RB_Dfr_444.changeSpeed(25);
      if(derEncoder.getZaehler()>EndPos){
        RB_Dfr_444.setMotorStopp();
        Serial.println("Sprung Test Ende");
        TestNr=0;
      }
      break;
    case 4: //Geglerantwort
      Serial.println( (String) micros() + ";" + (String) derEncoder.getGeschwindigkeitMicros() );

      break;
    case 5: //Hand Gesch. Messung
      //Serial.println("Handmessung");
      Serial.println( (String) micros() + ";" + (String) derEncoder.getGeschwindigkeitMicros() );
      break;
    case 6: //Const Fahren vor +zurück.
    RB_Dfr_444.changeSpeed(18);
      RB_Dfr_444.setMotorStart(Lore_ab);
      if(derEncoder.getZaehler()>EndPos)
        RB_Dfr_444.setMotorStopp();
    if(RB_Dfr_444.getMotorSpeed()==0 && derEncoder.getZaehler()>EndPos)
    RB_Dfr_444.setMotorStart(Lore_auf);
      if(derEncoder.getZaehler()<=StartPos){
        RB_Dfr_444.setMotorStopp();
        Serial.println("Const Test Vor/Zurueck Ende");
        TestNr=0;
      }
        break;
    case 7:   // Encoder Ausgabe
      Serial.println(derEncoder.getZaehler());
      break;
    case 8:   // Geschwindigkeitausgabe + Reglerausgabe
      RB_Dfr_444.setMotorStart(true);
      TestNr = 0;
      break;
    case 9:   // Reglertest
      if(once)
      {
        RB_Dfr_444.setMotorStart(Lore_ab);
        once = false;
      }
      RB_Dfr_444.Run();
      //Serial.println( (String) micros() + ";" + (String) derEncoder.getGeschwindigkeitMicros() + ";" + (String) RB_Dfr_444.getMotorSpeed());
      meinRegler.Notiz();
      if(derEncoder.getZaehler()>EndPos)
      {
        RB_Dfr_444.setMotorStopp();
        Serial.println("Regler Test Ende");
        TestNr=0;
        once = true;
      }
      break;
    case 10:
      RB_Dfr_444.setMotorStopp();
      TestNr = 0;
      break;
    default:
    RoteLED.An();
    //Serial.println("Ungueltig");
    Serial.println(TestNr);
    //delay(1000);
        break;
  }
  MotorStatus=RB_Dfr_444.Run(); //Managed den Motor und gibt den Zustand an.

 
      if(timerIndex < 100)
      {
        //LoopTimeArray[timerIndex] = micros() - lastTime;
        
        lastTime = micros();
        timerIndex++;
      }
      else
      {
        for(int j = 0; j<100; j++)
        {
          //Serial.print(LoopTimeArray[j]);Serial.print(" ; ");
        }
        timerIndex= 0;    
      }
    
 
} // Loop Endeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeendeende



void encoderEvent() //ISR
{
    if(digitalRead(encoderB))
        derEncoder.inkrementZaehler();
    else
      derEncoder.dekrementZaehler();
}

bool ABS() //Gibt ein Error zurück wenn die Lore festhängt.
{
  static int Position=0;
  static unsigned long Zeit=0;
  if(RB_Dfr_444.getMotorSpeed()>1 && millis()>Zeit+Ramp/2) 
  {
    Zeit=millis();
    if(Position<=derEncoder.getZaehler()-Tolleranz || Position>=derEncoder.getZaehler()+Tolleranz){
      Serial.println(" ABS Eingriff !");
        return Ok;// Wird später gemacht. return Error;
      }
    Position=derEncoder.getZaehler();
    return Ok;       
  }
  else
  {
    if(RB_Dfr_444.getMotorSpeed()>1)
      Zeit=millis();
    return Ok;
  }
}

ISR(TIMER2_COMPA_vect){    //This is the interrupt request
  OnBoardLED.Flashen();
  OnBoardLED.refresh();     //Kann auch in die loop
  RoteLED.Flashen();
  RoteLED.refresh();        
  GrueneLED.Flashen();
  GrueneLED.refresh();      
};

int Zyklenzaehler(bool Increment) //mit True aufrufen um Hochzuzählen.
{
  static int RAM=0;
  int ROM=EEPROM.get(0,ROM);
  if(Increment){
    ROM++;
    RAM++;
  }
  EEPROM.put(0,ROM);
  delay(100); // Zum Speichern. Sollte nix stören. Wird nur 1mal pro Zyklus aufgerufen. Und Encoder läuft über Interrupt. // Julian: Mich stört das hier!!!! //Andy: beweiß es ;)
  Serial.print("Bereits "); Serial.print(ROM);Serial.println(" Zyklen insgesammt bearbeitet.");
  Serial.print("Bereits "); Serial.print(RAM);Serial.println(" Zyklen seit letztem Neustart bearbeitet.");
  return RAM;
};
