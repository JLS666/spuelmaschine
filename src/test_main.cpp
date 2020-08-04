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
  OnBoardLED.SchnellBlinken();  //Andy: Kleines Beispiel für ne LED
//Encoder Resett erzwingen (While)

  Serial.println("Setup Abgeschlossen !");

}
 int zaehlerAlt = 0;
 unsigned long alteZeit = 0;
 float GeschArr[100];
 int i=0;
 bool ausgabefertig = false;
 #define anzahlWerte 100

 //bool timerModus = true;
void loop() { //Looplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplooplupi
  // Encoder Test
 /*
  if (zaehlerAlt != derEncoder.getZaehler() && millis() > (alteZeit + 10))
  {
      //Serial.println(i);
      //Serial.println(derEncoder.getZaehler());
      zaehlerAlt = derEncoder.getZaehler();
      if (i<anzahlWerte)
      {
      GeschArr[i]=(float)meinRegler.Notiz();
      i++;
      }
      else if(ausgabefertig == false)
      {
        Spuelautomat.transitionTo(Nothalt);
        delay(1000);
        for(int x=0;x<anzahlWerte;x++)
        {
          Serial.println(GeschArr[x]);
        }
        ausgabefertig = true;
      }
      
      alteZeit = millis();
  }*/
  // Endoder Test Ende
  int TestNr=0;
  if(Serial.available())
  TestNr=Serial.read();
  switch (TestNr)
  {
    case 0:
        //Nix
        break;
    case 1: //Const Fahren
      RB_Dfr_444.changeSpeed(18);
      RB_Dfr_444.setMotorStart(Lore_ab);
      if(derEncoder.getZaehler()>EndPos)
      RB_Dfr_444.setMotorStopp();
      Serial.println("Const Test Ende");
        break;
    case 2: //Const mit Gesch Messung
    case 3: //Sprungantwort ohne Regler
    case 4: //Geglerantwort
  
    default:
    Serial.println("Ungueltig");
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