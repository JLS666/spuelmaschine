class Motor
{
  public:                               // öffentlich
    Motor();                            // der Default-Konstruktor
    int Run();                          //in loop!!!! -1=Error Richtungswechsel, 0=Volle Fahrt, 1=Hochfahren
    void setMotorStart(bool mRichtung); //Startet den Motor. Richtungswechsel nur mit vorigen Motor Stopp möglich! 
    void setMotorStopp();               //Anhalten
    int getMotorSpeed();                //int??? gibt die Geschwindigkeit in % zurück.
    bool getMotoristRichtung();         //sagt einem die aktuelle Drehrichtung
    void Not_Aus();
    void setFrequenz(int f);            //ändert die Motorfrequenz zwischen 800-10.000 Hz.
    void changeSpeed(int speed);        //anpassen der Default Geschwindigkeit.  VORSICHT!
    ~Motor();                    

    

  private:                              // privat
    void AusgangsPower(int Pin, int Power);
    unsigned int istSpeed=0;
    bool Stopp=true;
    bool sollRichtung;
    bool istRichtung;
    unsigned int maxSpeed;
    int Frequenz;
};