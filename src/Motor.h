
class Motor
{
  public:                              // öffentlich
    Motor();                      // der Default-Konstruktor
    bool Run();               //in loop!
    void setMotorStart(bool mRichtung); 
    void setMotorStopp();     //Anhalten
    void getMotorSpeed();  //int??? gibt die Geschwindigkeit in % zurück.
    void getMotoristRichtung(); //sagt einem die aktuelle Drehrichtung
    ~Motor();                    

    

  private:                             // privat
    unsigned int istSpeed;
    bool Stopp=true;
    bool sollRichtung;
    bool istRichtung;
};