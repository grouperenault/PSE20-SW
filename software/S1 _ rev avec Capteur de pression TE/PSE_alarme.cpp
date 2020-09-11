// **********************************************************************************
// PSE project, alarme management
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

#ifndef PSE_alarme_cpp
    #define PSE_alarme_cpp
    
#include "PSE_alarme.h"
#include "./PSE_project.h"
#include "./PSE_perfusion.h"
#include "./PSE_define.h"

/* 
 * Fonction de vérification si la batterie est bien branchée/opérante 
 * Principe : coupure alim pour tester
 * Résultat : 
 *        - Si pas d'alim, reboot de l'arduino
 *        - sinon, ok
 * Sauvegarde d'un parametre (0 ou 1) dans l'eeprom afin de savoir au reboot si la batterie était ko 
 * et donc afficher message d'erreur/bloquant
 */
void checkBatterieDemarrage(Etat_Global &etatLocal) {
  // Debugln(F("Check Batterie"));

  byte cpt = 0;
  // Lecture de 8 valeurs de la tension.
  while (cpt < 8) {
      cpt ++;

      if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
        /* 
        * Lecture de toutes les entrées analogique 
        */
        readAnalogInputs(etatLocal);

        /*
        * Position des états des boutons suite à la lecture précédente
        */
        readAnalogInputTensions(etatLocal.etatTension.pinPhysique,etatLocal);
        readAnalogInputTension(etatLocal.etatTension, etatLocal); // A0
      }
  }

  // reboot et défaut dans la batterie déjà constaté
  if (etatLocal.etatBatAfterReboot == LOW) { 
    // Debugln(F("Reboot après défaut batterie"));
    cleanLCD();
    textLCD(F("Defaut batterie"),1,0);
    textLCD(F("Absente ou HS !"),1,1);

    // le bouton valide débloque et relance le check
    while((etatLocal.btnValid.valeur == etatLocal.btnValid.prevValeur) || etatLocal.btnValid.valeur == LOW) {  
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnValid, etatLocal); 
        }
    }
    cleanLCD();
  }

  // Vérification uniquement si la tension est de 16V
  if (etatLocal.valeurTensionMoy > seuilHaut && etatLocal.etatBatAfterReboot != uncheckBat) {
      // Debugln(F("Tension > seuilHaut"));
      etatLocal.etatBatAfterReboot = LOW;
      sauvegardeEtatBatAfterReboot(etatLocal);

      // Sécurité pour laisser le temps à l'eeprom d'écrire la valeur
      unsigned long tempTimer = millis();
      while (millis() - tempTimer < 100) {}
      
      // On actionne la coupure par Relais
      digitalWrite(etatLocal.relaisSecteur.pinPhysique, HIGH);
//      digitalWrite(etatLocal.ledSecteur.pinPhysique, LOW); // on stoppe la LED durant le test (permet de fonctionner avec la version proto par inversion de l'information LED)
      
      // On laisse le temps à l'arduino de se couper si besoin
      tempTimer = millis();
      while (millis() - tempTimer < 500) {}

      etatLocal.etatBatAfterReboot = HIGH;
      sauvegardeEtatBatAfterReboot(etatLocal);

      // On réaction l'alimentation via le chargeur
      digitalWrite(etatLocal.relaisSecteur.pinPhysique, LOW);
//      digitalWrite(etatLocal.ledSecteur.pinPhysique, HIGH); 
  } else if (etatLocal.etatBatAfterReboot != uncheckBat) {
      etatLocal.etatBatAfterReboot = HIGH;
      sauvegardeEtatBatAfterReboot(etatLocal);
  }
}

/* Boucle principale de la gestion des alarmes */
void alarmeManagement(Etat_Global &etatLocal){
    boolean updateInfo = false;

// ***************************************************************************************************************** //
    /* 
     * Redémarrage du buzzer si alarme encore active après delai défini dans timerRebootBuzzer
     * On en profite pour réafficher le non de l'alarme à l'écran
     */
     if (etatLocal.buzzer.redemarrage == HIGH && (millis() - etatLocal.buzzer.lastTime > timerRebootBuzzer) &&
        existeAlarme(etatLocal)) { 
          // on relance un calcul du son sur les alarmes
          updateInfo = true;
          etatLocal.displayAlarme = alarmRestart; 
          // Debug(F("Relance Buzzer + Display"));
     }


// ***************************************************************************************************************** //
    /* 
     * Alarme surpression 
     * Déclenchement sur capteur de surpression
     * et se désactive (pour le voyant) sur désactivation du capteur de surpression
     */
    // déclenchement de l'alarme surpression
    if (((etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus)) && 
        statutFX29() == surpression && etatLocal.alarmeOcclusion == LOW) {
      
      etatLocal.alarmeOcclusion = HIGH;
      etatLocal.etatMoteur =  moteurReculPression;
      etatLocal.affichageEnCours = menuPrincipal;

      // Alarme visuelle
      digitalWrite(etatLocal.ledOcclusion.pinPhysique, HIGH);
      
      updateInfo = true;
      etatLocal.displayAlarme = alarmOcclusion;
      // Debugln(F("Alarme Occlusion ON"));
    }

// ***************************************************************************************************************** //
    /* 
     * Alarme des 10% 
     * L'alarme se déclanche sur capteur 10%
     * et se désactive (pour le voyant) sur validation alarme fin de distribution ou sur arrêt de la distribution
     */
     // déclenchement de l'alarme
    if (etatLocal.positionChariot != chariotInconnu) {
      if ((etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus) && 
         etatLocal.alarmeDixPourCent == LOW && NEMA17.getTempsRestant() < 10 && etatLocal.alarmeFinCourse == LOW) {
        etatLocal.alarmeDixPourCent = HIGH;
      
//        Debug(F("getTempsRestant "));
//        Debugln(NEMA17.getTempsRestant());

        // Alarme visuelle
        etatLocal.ledFin.freqHaute    = 300;
        etatLocal.ledFin.freqBasse    = 500;
        etatLocal.ledFin.nbIteration  = -1;
              
        updateInfo = true;
        etatLocal.displayAlarme = alarmDixPourCent;
        // Debugln(F("Alarme 10% ON"));
      }
    }

    
// ***************************************************************************************************************** //
    /* 
     * Alarme fin de distibution
     * L'alarme se déclanche sur capteur de fin de course
     * et se désactive (pour le voyant) sur arrêt de la distribution et sur désactivation du capteur de fin de course (retour chariot)
     */
     // déclenchement de l'alarme
    if ((etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus || etatLocal.etatMoteur ==  moteurReculPression) && 
        etatLocal.alarmeFinCourse == LOW && (etatLocal.finDistrib.valeur == HIGH  || 
        (statutFX29() == surpression && etatLocal.positionChariot == chariotFin))) {

      etatLocal.alarmeFinCourse = HIGH;
      etatLocal.alarmeDixPourCent = LOW;
      etatLocal.affichageEnCours = menuPrincipal;

      if (etatLocal.etatMoteur != moteurReculPression) {etatLocal.etatMoteur = moteurArret;}

      // Alarme visuelle
      digitalWrite(etatLocal.ledFin.pinPhysique, HIGH);
      etatLocal.ledFin.nbIteration  = 0;
      etatLocal.ledFin.etat == LOW; // pour ne pas être perturbé par la gestion du clignotement
      
      updateInfo = true;
      etatLocal.displayAlarme = alarmFinCourse;
      // Debugln(F("Alarme Fin de seringue ON"));
    }

      

// ***************************************************************************************************************** //
    /* 
     * Alarme sur l'état du secteur et de la batterie
     * Le déclenchement se fait sur seuil de tension sur l'entrée Analogique A0
     * La désactivation se fait sur seuil de tension sur l'entrée Analogique A0
     */
    // Activation de l'alarme batterie (tension < X et batterie uniquement)
    if (etatLocal.etatTension.prevValeur == etatLocal.etatTension.valeur && etatLocal.etatTension.valeur == LOW 
        && etatLocal.alarmeBatterie == LOW && valeurADS() == adsBatterie) {

      etatLocal.alarmeBatterie = HIGH;

      // Alarme visuelle
      etatLocal.ledBatterie.freqHaute    = 800;
      etatLocal.ledBatterie.freqBasse    = 300;
      etatLocal.ledBatterie.nbIteration  = -1;

      updateInfo = true;
      etatLocal.displayAlarme = alarmBatterie;
      // Debugln(F("Alarme Batterie ON"));
    }
    
    // Désactivation de l'alarme Batterie
    if (etatLocal.etatTension.prevValeur == etatLocal.etatTension.valeur && etatLocal.etatTension.valeur == HIGH 
        && etatLocal.alarmeBatterie == HIGH) {
      etatLocal.alarmeBatterie = LOW;

      // Alarme visuelle .. désactivation
      etatLocal.ledBatterie.nbIteration  = 0;

      etatLocal.displayAlarme = alarmRestart; 
      updateInfo = true;
      // Debugln(F("Alarme Batterie OFF"));
    }

    // Activation de l'alarme secteur
    if (etatLocal.alarmeSecteur == LOW && isChangedADS() && valeurADS() == adsBatterie) {
      etatLocal.alarmeSecteur = HIGH;

      // Alarme visuelle
      etatLocal.ledSecteur.freqHaute    = 800;
      etatLocal.ledSecteur.freqBasse    = 300;
      etatLocal.ledSecteur.nbIteration  = -1;

      etatLocal.displayAlarme = alarmSecteur; 
      updateInfo = true;
      // Debugln(F("Alarme Secteur ON"));
    }

    // Désactivation de l'alarme secteur
    if (etatLocal.alarmeSecteur == HIGH && isChangedADS() && (valeurADS() == adsCharge || valeurADS() == adsFull)) {
      etatLocal.alarmeSecteur = LOW;

      // Alarme visuelle
      etatLocal.ledSecteur.nbIteration  = 0;
      digitalWrite(etatLocal.ledSecteur.pinPhysique, HIGH); 

      etatLocal.displayAlarme = alarmRestart; 
      updateInfo = true;
      // Debugln(F("Alarme Secteur ON"));
    }

// ***************************************************************************************************************** //
    /* 
     * Alarme fin de volume bolus
     */
    // Activation de l'alarme fin de bolus
    if (etatLocal.etatMoteur == moteurBolus) {
      
      etatLocal.prevVolBolus = etatLocal.volBolus;
      etatLocal.volBolus = NEMA17.GetVolumeBolus();

      // Déclenchement de la règle dépassement Bolus
      if (etatLocal.volBolus >= etatLocal.volumeBolusMax){

          etatLocal.alarmeFinBolus = HIGH;
          etatLocal.volBolus = etatLocal.volumeBolusMax;
          etatLocal.etatMoteur = moteurPerfusion;
          bolusLCD(etatLocal);
      } else {
          etatLocal.alarmeFinBolus = LOW;
          bolusLCD(etatLocal);
      }
    }


// ***************************************************************************************************************** //
    /*
     * On joue les alternances des voyants / buzzer / message écran
     */
    if (updateInfo == true) {
      etatLocal.lastDisplayAlarme = -1;
      etatLocal.alarmeLastDisplay = millis() - timerBoucleAlarme - 1;
      affichageAlarme(etatLocal);

      buzzerMelodie(etatLocal); 
    }

    buzzerManagement(etatLocal);
    lightManagement(etatLocal);
}

// ***************************************************************************************************************** //
/* Définition mélodie */
void buzzerMelodie(Etat_Global &etatLocal){
    // ordre de priorité : occlusion, fin de perf, batterie, 10%, secteur
    if (etatLocal.alarmeOcclusion == HIGH) {
      etatLocal.buzzer.freqHaute    = 300;
      etatLocal.buzzer.freqBasse    = 200;
      etatLocal.buzzer.volume       = 2000;
      etatLocal.buzzer.nbIteration  = -1;
      
    } else if (etatLocal.alarmeFinCourse == HIGH) {
      etatLocal.buzzer.freqHaute    = 600;
      etatLocal.buzzer.freqBasse    = 200;
      etatLocal.buzzer.volume       = 2000;
      etatLocal.buzzer.nbIteration  = -1;
      
    } else if (etatLocal.alarmeBatterie == HIGH) {
      etatLocal.buzzer.freqHaute    = 800;
      etatLocal.buzzer.freqBasse    = 300;
      etatLocal.buzzer.volume       = 2000;
      etatLocal.buzzer.nbIteration  = -1;

    } else if (etatLocal.alarmeSecteur == HIGH && etatLocal.displayAlarme != alarmRestart) { // elle ne sonnes qu'une seule fois
      etatLocal.buzzer.freqHaute    = 800;
      etatLocal.buzzer.freqBasse    = 300;
      etatLocal.buzzer.volume       = 1000;
      etatLocal.buzzer.nbIteration  = -1;

    } else if (etatLocal.alarmeDixPourCent == HIGH) {
      etatLocal.buzzer.freqHaute    = 300;
      etatLocal.buzzer.freqBasse    = 500;
      etatLocal.buzzer.volume       = 2000;
      etatLocal.buzzer.nbIteration  = -1;
  
    } else { // cas ou il n'y a plus d'alarme
      etatLocal.buzzer.volume       = 0;
      etatLocal.buzzer.nbIteration  = 0;     
    }

    // on attendra une nouvelle action utilisateur pour relancer les sons
    etatLocal.buzzer.redemarrage = LOW; 
}

/* Gestion du buzzer */
void buzzerManagement(Etat_Global &etatLocal){
    // Si le son est activé par une des alertes, on pilote la fréquence des bips
    if ((etatLocal.buzzer.nbIteration > 0) || (etatLocal.buzzer.nbIteration == -1)) { // pas de buzzer activé
       
       // il reste des sons à jouer. On doit permutter l'état du buzzer
       unsigned long currentMillis = millis();

       if (etatLocal.buzzer.etat == LOW) {
          // gestion quand le son a l'état bas
          if (currentMillis - etatLocal.buzzer.lastTime > etatLocal.buzzer.freqBasse || currentMillis < etatLocal.buzzer.lastTime) {
//            analogWrite(etatLocal.buzzer.pinPhysique, etatLocal.buzzer.volume); // Faire du bruit
            tone(etatLocal.buzzer.pinPhysique, etatLocal.buzzer.volume);
//            Debugln(F("Start buzzer"));
            etatLocal.buzzer.lastTime = currentMillis;
            etatLocal.buzzer.etat = HIGH;
            if (etatLocal.buzzer.nbIteration > 0) { etatLocal.buzzer.nbIteration = etatLocal.buzzer.nbIteration - 1; }
          }
       } else {
          // gestion quand le son a l'état haut
          if (currentMillis - etatLocal.buzzer.lastTime > etatLocal.buzzer.freqHaute || currentMillis < etatLocal.buzzer.lastTime) {
//            analogWrite(etatLocal.buzzer.pinPhysique, LOW); // Silence ....
//            Debugln(F("Stop buzzer"));
            noTone(etatLocal.buzzer.pinPhysique);
            etatLocal.buzzer.lastTime = currentMillis;
            etatLocal.buzzer.etat = LOW;
            if (etatLocal.buzzer.nbIteration > 0) { etatLocal.buzzer.nbIteration = etatLocal.buzzer.nbIteration - 1; }
          }
       }
    } else if (etatLocal.buzzer.etat == 1) { // le son a été joué le temps voulu. On stoppe tout proprement si le son fonctionne encore
//        analogWrite(etatLocal.buzzer.pinPhysique, LOW); // Silence ....
        noTone(etatLocal.buzzer.pinPhysique);
        etatLocal.buzzer.etat = LOW;
    }
}

// ***************************************************************************************************************** //
/* Gestion des affichages lumineux*/
void lightManagement(Etat_Global &etatLocal){
    ledManagement(etatLocal.ledFin);
//    ledManagement(etatLocal.ledOcclusion); // gérer de façon fixe (sans clignotement)
    ledManagement(etatLocal.ledPerfEnCours);
    ledManagement(etatLocal.ledBatterie);
    ledManagement(etatLocal.ledSecteur);
}

/* Gestion d'une LED */
void ledManagement(Alerte &alerteLocal) {
    // Si la led est activée par une des alertes, on pilote la fréquence de la lumière
    if ((alerteLocal.nbIteration > 0) || (alerteLocal.nbIteration == -1)) { 
       
       // il reste des sons à jouer. On doit permutter l'état du buzzer
       unsigned long currentMillis = millis();

       if (alerteLocal.etat == LOW) {
          // gestion quand la lumière a l'état bas
          if (currentMillis - alerteLocal.lastTime > alerteLocal.freqBasse || currentMillis < alerteLocal.lastTime) {
            digitalWrite(alerteLocal.pinPhysique, HIGH); // On éclaire
            alerteLocal.lastTime = currentMillis;
            alerteLocal.etat = HIGH;
            if (alerteLocal.nbIteration > 0) { alerteLocal.nbIteration = alerteLocal.nbIteration - 1; }
          }
       } else {
          // gestion quand la lumière a l'état haut
          if (currentMillis - alerteLocal.lastTime > alerteLocal.freqHaute || currentMillis < alerteLocal.lastTime) {
            digitalWrite(alerteLocal.pinPhysique, LOW); // Dans le noir ....
            alerteLocal.lastTime = currentMillis;
            alerteLocal.etat = LOW;
            if (alerteLocal.nbIteration > 0) { alerteLocal.nbIteration = alerteLocal.nbIteration - 1; }
          }
       } // l'animation lumineuse a été jouée le temps voulu. On met la lumière à l'état par défaut
    } else if (((alerteLocal.etat == HIGH) && (alerteLocal.etatDefault == LOW)) || ((alerteLocal.etat == LOW) && (alerteLocal.etatDefault == HIGH))) {
        digitalWrite(alerteLocal.pinPhysique, alerteLocal.etatDefault); 
        alerteLocal.etat = alerteLocal.etatDefault;
        alerteLocal.lastTime = 0;
    }
}

#endif
