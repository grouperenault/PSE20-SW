// **********************************************************************************
// PSE project, distribution management
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
#ifndef PSE_perfusion_cpp
    #define PSE_perfusion_cpp
    
#include "./PSE_perfusion.h"

void moteurManagement(Etat_Global &etatLocal){

  // Mise à jour du compteur de step dans la classe du moteur
  if (etatLocal.etatNEMA == HIGH) {
    // cas du fin de course avant
    if (etatLocal.finDistrib.valeur == HIGH && etatLocal.finDistrib.valeur != etatLocal.finDistrib.prevValeur) {
      NEMA17.InitPosition(0);
      etatLocal.positionChariot = chariotFin;
//      Debug(F("chariot Debut - finDistrib "));
    
    // cas du fin de course arrière
    } else if (etatLocal.finRetour.valeur == HIGH && etatLocal.finRetour.valeur != etatLocal.finRetour.prevValeur) {
      NEMA17.InitPosition(etatLocal.positionFin);
      etatLocal.positionChariot = chariotDebut;
//      Debug(F("chariot Fin - finRetour "));
//      Debugln(etatLocal.positionFin);

    // cas du capteur 10% mais uniquement en marche avant
    } else if (etatLocal.dixPourCent.valeur == HIGH && etatLocal.dixPourCent.valeur != etatLocal.dixPourCent.prevValeur &&
                etatLocal.etatMoteur != moteurRecul && etatLocal.etatMoteur != moteurReculPression) {

      NEMA17.InitPosition(etatLocal.position10);
      
      etatLocal.positionChariot = chariotFin;
//      Debug(F("chariot Fin - 10% "));
//      Debugln(etatLocal.position10);

    } else if (etatLocal.dixPourCent.valeur == LOW && etatLocal.dixPourCent.valeur != etatLocal.dixPourCent.prevValeur &&
                (etatLocal.etatMoteur == moteurRecul || etatLocal.etatMoteur == moteurReculPression)) {
     
      etatLocal.positionChariot = chariotDebut;
//      Debugln(F("chariot Debut - 10%"));
    }
  }

  // L'ordre moteur a changé
  if (etatLocal.prevEtatMoteur != etatLocal.etatMoteur) {
    if (etatLocal.etatMoteur == moteurArret) {
      // Debugln(F("stop moteur"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
    
    } else if (etatLocal.etatMoteur == moteurPerfusion && etatLocal.finDistrib.valeur == LOW && statutFX29() != surpression) {
      // Debugln(F("start perfusion"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.debitEnCours.volGlobale, etatLocal.seringueDiam[etatLocal.seringueSel]);

    } else if (etatLocal.etatMoteur == moteurBolus && etatLocal.finDistrib.valeur == LOW && statutFX29() != surpression) {

      // Debugln(F("start Bolus"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.volumeBolus, etatLocal.seringueDiam[etatLocal.seringueSel]);

    } else if (etatLocal.etatMoteur == moteurAvance && etatLocal.finDistrib.valeur == LOW && statutFX29() != surpression) {

      // Debugln(F("start avance"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(AVANCE,etatLocal.volumeAvance, diametreAvRe);
    
    } else if (etatLocal.etatMoteur == moteurRecul && etatLocal.finRetour.valeur == LOW ) {
      // Debugln(F("start retour"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(RECUL,etatLocal.volumeRecul, diametreAvRe);

    } else if (etatLocal.etatMoteur == moteurReculPression && statutFX29() == surpression ) {

      // Debugln(F("start recul pour arret surPression"));
      etatLocal.etatNEMA = HIGH;
      NEMA17.Ctrl_Moteur(RECUL,etatLocal.debitEnCours.volGlobale, etatLocal.seringueDiam[etatLocal.seringueSel]);

    } else {
      // on stoppe le moteur car on ne connait pas le statut du moteur donc on arrête
      // ce cas ne devrait pas arriver
      // Debugln(F("stop moteur xx"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
    }
    
    etatLocal.prevEtatMoteur = etatLocal.etatMoteur;
  } else { // L'ordre moteur n'a pas changé mais il faut gérer les arrêts
    if (etatLocal.etatMoteur != moteurArret && etatLocal.etatMoteur != moteurReculPression &&
       ((etatLocal.finDistrib.valeur != etatLocal.finDistrib.prevValeur && etatLocal.finDistrib.valeur == HIGH)  ||
       (isChangedFX29() == true && statutFX29() == surpression))) {

      // Debugln(F("stop moteur --"));
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
      

    } else if (etatLocal.etatMoteur == moteurReculPression && statutFX29(reculPression) != surpression ) {

      // Debugln(F("stop moteur - moteurReculPression"));
      etatLocal.etatMoteur = moteurArret;
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  

    } else if (etatLocal.etatMoteur == moteurAvance && isChangedFX29(accostage) == true && statutFX29() == accostage ) {
      // Debugln(F("stop moteur - accostage"));
      etatLocal.etatMoteur = moteurArret;
      etatLocal.etatNEMA = LOW;
      NEMA17.Motor_STOP();  
    }
  }
}

#endif
