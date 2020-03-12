/********************************************************************
    Autoren:        Anselm Maximilian Lehmann                       
                                                                                    
    Firma:          Hochschule Karlsruhe - Technik u. Wirtschaft    
    Datei:          Defines.h                                       
    Beschreibung:   Auflistung aller Zustände damit diese nicht die 
                    "main" vollmüllen :)                          
                    Alle Zustände sollten diese drei Aktionen     
                    besitzen auch wenn diese leer sind:
                    en_#
                    do_#
                    ex_#

    Version:      0.1                                                               
/********************************************************************/

#include <Defines.h>

//Init
void en_Init();
void do_Init();
void ex_Init();

//Standby
void en_Standby();
void do_Standby();
void ex_Standby();

//Rakeln
void en_Rakeln();
void do_Rakeln();
void ex_Rakeln();

//Blasen
void en_Blasen();
void do_Blasen();
void ex_Blasen();

//Abstreifen
void en_Abstreife();
void do_Abstreife();
void ex_Abstreife();

//Lore nach Vorne
void en_Lore_vor();
void do_Lore_vor();
void ex_Lore_vor();

//Error
void en_Error();
void do_Error();
void ex_Error();

//Kalibrierung
