// **********************************************************************************
// PSE project, Config file management
// **********************************************************************************
// The source code is protected by intellectual and/or industrial property rights.
// Copyright Renault S.A.S. 2020
//
// Written by A006239 for Renault S.A.S. usage ONLY
//
// History : included in file readme.me
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "PSE_project.h"


// Etat global du système
Etat_Global etatGlobal;

/****************************** Setup ***************************************/
void setup() {
  #ifdef DEBUG
    DEBUG_SERIAL.begin(115200);
    Debugln(F("\r\n\r\n=============="));
    Debug(F("Pousse Seringue Electrique "));
    Debugln(F(PSE_VERSION));
    Debugln(F("Renault S.A.S."));
    Debugln(F("\r\n=============="));
    Debugln();
    Debugflush();
  #endif
  
  /* 
   * Définition des d'entrée / sortie
   */
  initAnalogOutput(D3,LOW,etatGlobal.buzzer);
  initDigitalOutput(D6,LOW,etatGlobal.ledFin);
  initDigitalOutput(D7,LOW,etatGlobal.ledOcclusion);
  initDigitalOutput(D5,LOW,etatGlobal.ledPerfEnCours);
  initDigitalOutput(D2,HIGH,etatGlobal.ledBatterie); // Par défaut le voyant est allumé et clignote en cas d'alarme
  initDigitalOutput(D0,HIGH,etatGlobal.ledSecteur); // Par défaut le voyant est allumé et clignote en cas d'alarme
  initDigitalOutput(D4,LOW,etatGlobal.relaisSecteur);


//  initAnalogInput(pinA0, 473, 538, etatGlobal.etatTension); 
  initAnalogInput(pinA0, 473, 1024, etatGlobal.etatTension); 
  initAnalogInput(pinA1, swaLow, swaHigh, etatGlobal.finRetour);
  initAnalogInput(pinA1, swbLow, swbHigh, etatGlobal.finDistrib);
  initAnalogInput(pinA1, swcLow, swcHigh, etatGlobal.dixPourCent);  
  //initAnalogInput(pinA1, swdLow, swdHigh, etatGlobal.surPression);  // Unactive after the pressure sensor
  initAnalogInput(pinA2, swaLow, swaHigh, etatGlobal.btnDown);
  initAnalogInput(pinA2, swbLow, swbHigh, etatGlobal.btnUp);   
  initAnalogInput(pinA2, swcLow, swcHigh, etatGlobal.btnValid);     
  initAnalogInput(pinA3, swaLow, swaHigh, etatGlobal.btnStartStop); 
  initAnalogInput(pinA3, swbLow, swbHigh, etatGlobal.btnBolus);     
  initAnalogInput(pinA3, swcLow, swcHigh, etatGlobal.btnRecul);    
  initAnalogInput(pinA3, swdLow, swdHigh, etatGlobal.btnAvance);     

  // Eviter les alertes au démarrage 
  etatGlobal.etatTension.valeur = HIGH;

  // Lecture des valeurs stockées dans l'eeprom
  restaureValeurs(etatGlobal);

  // Initialisation du LCD
  initLCD(etatGlobal);

  // Initialisation de l'ADS
  initADS();

  // Initialisation du FX29
  initFX29();

  // Initialisation des PINs Moteur
  NEMA17.Init_PinDriverMoteur(); 

  // vérifie si la batterie est branchée ou non.
  checkBatterieDemarrage(etatGlobal);

  // Affichage du splash screen
  cleanLCD();
  splashLCD();
  // delay(1000); // délai retrouvé dans la gestion de la config en temps d'attente :-)

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
  etatGlobal.positionChariot = chariotInconnu;
}
 
/****************************** Loop ****************************************/
void loop() {
  wdt_reset(); // uncomment to avoid reboot

  lectureEntrees(etatGlobal);        // Lecture des entrées analogiques + changement des états boutons
  boutonManagement(etatGlobal);      // Lecture des boutons et switch + gestion des boutons et des règles associées
  moteurManagement(etatGlobal);      // Gestion de l'état moteur
  alarmeManagement(etatGlobal);     
  affichageAlarme(etatGlobal);       // permet d'afficher les alarmes en boucle et de les relancer en cas de besoin
  affichageInidicateurPerf(etatGlobal);

}
