// **********************************************************************************
// PSE project, distribution management
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
#ifndef PSE_perfusion_cpp
    #define PSE_perfusion_cpp
    
#include "./PSE_perfusion.h"

void moteurManagement(Etat_Global &etatLocal){

  // L'ordre moteur a changé
  if (etatLocal.prevEtatMoteur != etatLocal.etatMoteur) {
    if (etatLocal.etatMoteur == moteurArret) {
      Debugln(F("stop moteur"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
    
    } else if (etatLocal.etatMoteur == moteurPerfusion && etatLocal.finDistrib.valeur == LOW && etatLocal.surPression.valeur == LOW) { 
      Debugln(F("start perfusion"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.debitEnCours.volGlobale, etatLocal.seringueDiam[etatLocal.seringueSel]);
    
    } else if (etatLocal.etatMoteur == moteurBolus && etatLocal.finDistrib.valeur == LOW && etatLocal.surPression.valeur == LOW) {
      Debugln(F("start Bolus"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.volumeBolus, etatLocal.seringueDiam[etatLocal.seringueSel]);

    } else if (etatLocal.etatMoteur == moteurAvance && etatLocal.finDistrib.valeur == LOW && etatLocal.surPression.valeur == LOW) {
      Debugln(F("start avance"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.volumeAvance, diametreAvRe);
    
    } else if (etatLocal.etatMoteur == moteurRecul && etatLocal.finRetour.valeur == LOW ) {
      Debugln(F("start retour"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(RECUL,etatLocal.volumeRecul, diametreAvRe);

    } else {
      // on stoppe le moteur car on ne connait pas le statut du moteur donc on arrête
      // ce cas ne devrait pas arriver
      Debugln(F("stop moteur xx"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
    }
    
    etatLocal.prevEtatMoteur = etatLocal.etatMoteur;
  } else { // L'ordre moteur n'a pas changé mais il faut gérer les arrêts
    if (etatLocal.etatMoteur != moteurArret &&
       ((etatLocal.finDistrib.valeur != etatLocal.finDistrib.prevValeur && etatLocal.finDistrib.valeur == HIGH) ||
       (etatLocal.surPression.valeur != etatLocal.surPression.prevValeur && etatLocal.surPression.valeur == HIGH))) {

      Debugln(F("stop moteur --"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
      
    } else if (etatLocal.surPression.valeur != etatLocal.surPression.prevValeur && etatLocal.surPression.valeur == LOW) {
  
        etatLocal.prevEtatMoteur = 0;
  
    }
  }
}

#endif
