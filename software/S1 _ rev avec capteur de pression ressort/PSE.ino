// **********************************************************************************
// PSE project, Config file management
// **********************************************************************************
// The source code is protected by intellectual and/or industrial property rights.
// Copyright Renault S.A.S. 2020
//
// For any explanation about key project or use, contact me
// jimmy.jaumotte@renault.com
//
// Written by Jimmy JAUMOTTE for Renault S.A.S. usage ONLY
//
// History : V1.00 2020-03-31 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "PSE_project.h"


// Etat global du système
Etat_Global etatGlobal;

/****************************** Setup ***************************************/
void setup() {
  DEBUG_SERIAL.begin(115200);
  Debugln(F("\r\n\r\n=============="));
  Debug(F("Pousse Seringue Electrique "));
  Debugln(F(PSE_VERSION));
  Debugln(F("Renault S.A.S."));
  Debugln(F("\r\n=============="));
  Debugln();
  Debugflush();

  /* 
   * Définition des d'entrée / sortie
   */
  initAnalogOutput(D3,LOW,etatGlobal.buzzer);
  initDigitalOutput(D6,LOW,etatGlobal.ledFin);
  initDigitalOutput(D7,LOW,etatGlobal.ledOcclusion);
  initDigitalOutput(D5,LOW,etatGlobal.ledPerfEnCours);
  initDigitalOutput(D2,HIGH,etatGlobal.ledBatterie); // Par défaut le voyant est allumé et clignote en cas d'alarme
  initDigitalOutput(D4,HIGH,etatGlobal.ledSecteur); // Par défaut le voyant est allumé et clignote en cas d'alarme



  initAnalogInput(A0, 473, 1024, etatGlobal.etatBatterie); 
  etatGlobal.etatBatterie.valeur = HIGH;
  initAnalogInput(A0, 538, 1024, etatGlobal.etatSecteur);  
  etatGlobal.etatSecteur.valeur = HIGH;
  initAnalogInput(A1, swaLow, swaHigh, etatGlobal.finRetour);
  initAnalogInput(A1, swbLow, swbHigh, etatGlobal.finDistrib);
  initAnalogInput(A1, swcLow, swcHigh, etatGlobal.dixPourCent);  
  initAnalogInput(A1, swdLow, swdHigh, etatGlobal.surPression);  
  initAnalogInput(A2, swaLow, swaHigh, etatGlobal.btnDown);
  initAnalogInput(A2, swbLow, swbHigh, etatGlobal.btnUp);   
  initAnalogInput(A2, swcLow, swcHigh, etatGlobal.btnValid);     
  initAnalogInput(A3, swaLow, swaHigh, etatGlobal.btnStartStop); 
  initAnalogInput(A3, swbLow, swbHigh, etatGlobal.btnBolus);     
  initAnalogInput(A3, swcLow, swcHigh, etatGlobal.btnRecul);    
  initAnalogInput(A3, swdLow, swdHigh, etatGlobal.btnAvance);     

  // Lecture des valeurs stockées dans l'eeprom
  restaureValeurs(etatGlobal);

  // Initialisation du LCD
  initLCD(etatGlobal);

  // Initialisation des PINs Moteur
  NEMA17.Init_PinDriverMoteur(); 

  // Affichage du splash screen
  cleanLCD();
  splashLCD();
  //delay(3000); // délai retrouvé dans la gestion de la config en temps d'attente :-)

  // Lance la gestion de la configuration
  managementConfig(etatGlobal);

  // Affichage du menu principal
  menuLCD(etatGlobal);
  
  /* 
   * Définition du watchdog
   */
  wdt_enable(WDTO_2S);     // List of value in file "readme.me"

  etatGlobal.lastReadAnalog = millis();
  etatGlobal.alarmeLastDisplay = millis();
}

/****************************** Loop ****************************************/
void loop() {
  wdt_reset(); // uncomment to avoid reboot
  lectureEntrees(etatGlobal);        // Lecture des entrées analogiques + changement des états boutons
  boutonManagement(etatGlobal);      // Lecture des boutons et switch + gestion des boutons et des règles associées
  alarmeManagement(etatGlobal);     
  moteurManagement(etatGlobal);      // Gestion de l'état moteur
  affichageAlarme(etatGlobal);       // permet d'afficher les alarmes en boucle et de les relancer en cas de besoin
  affichageInidicateurPerf(etatGlobal);

}
