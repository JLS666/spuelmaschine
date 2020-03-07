class Motor
{
  public:                                     // öffentlich
    Motor(int PWM_Pin, int DIR_Pin);          // der Konstruktor
    int Run();                                //in loop!!!! -1=Error Richtungswechsel, 0=Volle Fahrt, 1=Hochfahren
    void setMotorStart(bool mRichtung);       //Startet den Motor. Richtungswechsel nur mit vorigen Halt möglich! Halt kann mit getMotorSpeed()==0 ermittelt werden.
    void setMotorStopp();                     //Anhalten
    int getMotorSpeed();                      //gibt die Geschwindigkeit in % zurück.
    bool getMotoristRichtung();               //gibt die aktuelle Drehrichtung
    void Not_Aus();                           //sofort Abschalten.
    void setFrequenz(int f);                  //ändert die Motorfrequenz zwischen 800-10.000 Hz.
    void changeSpeed(int speed);              //anpassen der Default Geschwindigkeit.  VORSICHT!!
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