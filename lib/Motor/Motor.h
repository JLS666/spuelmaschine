/* 
Motor Klasse Optimiert für BB-VNH3SP30 DC Treiber. Auf Arduino UNO.
Autor: Andy 
*/

class Motor
{
  public:                                     
    Motor(int PWM_Pin, int DIR_Pin_A, int DIR_Pin_B); 
    int Run();                                //in loop!!!! -1=Error Richtungswechsel, 0=Volle Fahrt, 1=Hochfahren
    void setMotorStart(bool mRichtung);       //Startet den Motor. Richtungswechsel nur mit vorigen Halt möglich! Halt kann mit getMotorSpeed()==0 ermittelt werden.
    void setMotorStopp();                     //Anhalten
    int getMotorSpeed();                      //gibt die Geschwindigkeit in % zurück.
    bool getMotoristRichtung();               //gibt die aktuelle Drehrichtung
    void Not_Aus();                           //sofort Abschalten.
    void setFrequenz(int f);                  //ändert die Motorfrequenz zwischen 800-10.000 Hz.
    void changeSpeed(int speed);              //anpassen der Default Geschwindigkeit.  VORSICHT!!
    int Fehlererkennung();                    //Nicht Implementiert.
    void Bremsen();                           //Nicht Implementiert. //Max: was soll der Unterschied sein zwischen Bremsen, Not_aus, setMotorStop?
    ~Motor();                    

    

  private:                                    // privat
    void AusgangsPower(int Pin, int Power);
    unsigned int istSpeed=0;
    bool Stopp=true;
    bool sollRichtung;
    bool istRichtung;
    unsigned int maxSpeed;
    int Frequenz;
};
