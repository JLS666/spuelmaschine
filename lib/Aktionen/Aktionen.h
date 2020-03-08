

   //***********************************   Funtkionen bekannt machen*******************************************/
  extern void en_Init();
  extern void en_Blasen();
 extern void do_Standby();
  extern void Leer();


    //******************************************************************************/
  //Zustandsautomat erstellen. Nach Plan in der Drive
  //States:
 extern  State Init        = State (en_Init,Leer,Leer);
 extern  State BlasenEin   = State (en_Blasen,Leer,Leer);
 extern  State Standby     = State (do_Standby);
 extern  State Rakeln      = State (Leer);
 extern  State Abstreifen  = State (Leer);
  State BlasenAus   = State (Leer);
  State Return      = State (Leer);
  State ErrorState  = State (Leer); 

  //******************************************************************************/
  //Aktionen:
  void en_Init()
  {
    Serial.println("Initialisierung");
    RB_Dfr_444.setMotorStart(Lore_Zurueck);
    //Zylinder ein
    //Led an

      //Wir warten auf den Start.
      Serial.println("getting Ready...");
      delay(500);
    
  }
  void en_Blasen() 
  {
    Serial.println("entry Blasen");

  }
  void do_Standby() 
  {
    Serial.println("Standby");

  }
  void Leer()
  {
    //Nix die ist Leer.
    Serial.println("Welcher Trottel ruft Leer auf?");
  }
  //******************************************************************************/