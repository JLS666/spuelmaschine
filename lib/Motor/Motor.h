
class Motor
{
  public:                               // öffentlich
    Motor();                            // der Default-Konstruktor
    int Run();                          //in loop!!!! -1=Error Richtungswechsel, 0=Volle Fahrt, 1=Hochfahren
    void setMotorStart(bool mRichtung); //Startet den Motor. Richtungswechsel nur mit vorigen Motor Stopp möglich! 
    void setMotorStopp();               //Anhalten
    int getMotorSpeed();                //int??? gibt die Geschwindigkeit in % zurück.
    bool getMotoristRichtung();         //sagt einem die aktuelle Drehrichtung
    //void changeSpeed(int mnewSpeed);
    //void Not_Aus();
    ~Motor();                    

    

  private:                              // privat
    unsigned int istSpeed;
    bool Stopp=true;
    bool sollRichtung;
    bool istRichtung;
    unsigned int maxSpeed;
};