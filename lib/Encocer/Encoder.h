class Encoder
{
private:
    int zaehler;
public:
    Encoder();
    ~Encoder();
    void resetZaehler();
    int getZaehler();
    void inkrementZaehler();
    void dekrementZaehler();
    void encoderEvent();
};
