// **********************************************************************************
// PSE project, alarme management
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

#ifndef PSE_alarme_cpp
    #define PSE_alarme_cpp
    
#include "./PSE_alarme.h"
#include "./PSE_project.h"
#include "./PSE_perfusion.h"
#include "./PSE_define.h"

/* Boucle principale de la gestion des alarmes */
void alarmeManagement(Etat_Global &etatLocal){
    boolean modifDisplay = false;
    boolean modifBuzzer = false;

// ***************************************************************************************************************** //
    /* 
     * Redémarrage du buzzer si alarme encore active après delai défini dans timerRebootBuzzer
     * On en profite pour réafficher le non de l'alarme à l'écran
     */
     if (etatLocal.buzzer.redemarrage == HIGH && (millis() - etatLocal.buzzer.lastTime > timerRebootBuzzer) &&
        existeAlarme(etatLocal)) { 
          // on relance un calcul du son sur les alarmes
          modifBuzzer = true;          
          modifDisplay = true;
          etatLocal.displayAlarme = alarmRestart; 
          Debug("Relance Buzzer + Display");
     }


// ***************************************************************************************************************** //
    /* 
     * Alarme surpression 
     * Déclenchement sur capteur de surpression
     * et se désactive (pour le voyant) sur désactivation du capteur de surpression
     */
    // déclenchement de l'alarme surpression
    if (( (etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus)) && 
        (etatLocal.surPression.valeur != etatLocal.surPression.prevValeur) && (etatLocal.surPression.valeur == HIGH) && etatLocal.alarmeOcclusion == LOW) {
      
      etatLocal.alarmeOcclusion = HIGH;
      
      // Alarme visuelle
      digitalWrite(etatLocal.ledOcclusion.pinPhysique, HIGH);
      
      modifBuzzer = true;
      modifDisplay = true;
      etatLocal.displayAlarme = alarmOcclusion;
      Debugln(F("Alarme Occlusion ON"));
    }
    // Extinction de l'alarme surpression
    if ((etatLocal.surPression.valeur != etatLocal.surPression.prevValeur) && (etatLocal.surPression.valeur == LOW) && etatLocal.alarmeOcclusion == HIGH) {
      
      etatLocal.alarmeOcclusion = LOW;
      
      // Alarme visuelle
      digitalWrite(etatLocal.ledOcclusion.pinPhysique, LOW);

      etatLocal.displayAlarme = alarmRestart; 
      modifBuzzer = true;
      modifDisplay = true;
      Debugln(F("Alarme Occlusion OFF"));
    }

// ***************************************************************************************************************** //
    /* 
     * Alarme des 10% 
     * L'alarme se déclanche sur capteur 10%
     * et se désactive (pour le voyant) sur validation alarme fin de distribution ou sur arrêt de la distribution
     */
     // déclenchement de l'alarme
    if ((etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus) && (etatLocal.dixPourCent.valeur != etatLocal.dixPourCent.prevValeur) && 
        (etatLocal.dixPourCent.valeur == HIGH) && etatLocal.alarmeDixPourCent == LOW) {
      
      etatLocal.alarmeDixPourCent = HIGH;
      
      // Alarme visuelle
      etatLocal.ledFin.freqHaute    = 200;
      etatLocal.ledFin.freqBasse    = 500;
      etatLocal.ledFin.nbIteration  = -1;
            
      modifBuzzer = true;
      modifDisplay = true;
      etatLocal.displayAlarme = alarmDixPourCent;
      Debugln(F("Alarme 10% ON"));
    }
    
    // désactivation de l'alarme 10%
    if (etatLocal.etatMoteur == moteurRecul && etatLocal.alarmeDixPourCent == HIGH && etatLocal.alarmeFinCourse == LOW) {
      etatLocal.alarmeDixPourCent = LOW;

      // Alarme visuelle
      digitalWrite(etatLocal.ledFin.pinPhysique, LOW);
      etatLocal.ledFin.nbIteration  = 0;
      etatLocal.ledFin.etat == LOW; // pour ne pas être perturbé par la gestion du clignotement
                        
      etatLocal.displayAlarme = alarmRestart; 
      modifBuzzer = true;
    }
    
// ***************************************************************************************************************** //
    /* 
     * Alarme fin de distibution
     * L'alarme se déclanche sur capteur de fin de course
     * et se désactive (pour le voyant) sur arrêt de la distribution et sur désactivation du capteur de fin de course (retour chariot)
     */
     // déclenchement de l'alarme
    if ((etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus) && /*(etatLocal.finDistrib.valeur != etatLocal.finDistrib.prevValeur) && */
      (etatLocal.finDistrib.valeur == HIGH) && etatLocal.alarmeFinCourse == LOW) {
      etatLocal.alarmeFinCourse = HIGH;
      etatLocal.alarmeDixPourCent = LOW;
      
      // Alarme visuelle
      digitalWrite(etatLocal.ledFin.pinPhysique, HIGH);
      etatLocal.ledFin.nbIteration  = 0;
      etatLocal.ledFin.etat == LOW; // pour ne pas être perturbé par la gestion du clignotement
      
      modifBuzzer = true;
      modifDisplay = true;
      etatLocal.displayAlarme = alarmFinCourse;
      Debugln(F("Alarme Fin de seringue ON"));
    }

    // désactivation de l'alarme
    if ((etatLocal.finDistrib.valeur != etatLocal.finDistrib.prevValeur) && (etatLocal.finDistrib.valeur == LOW) && etatLocal.alarmeFinCourse == HIGH) {
      etatLocal.alarmeFinCourse = LOW;
      
      etatLocal.etatMoteur = moteurArret;
      etatLocal.affichageEnCours = menuPrincipal;
      digitalWrite(etatLocal.ledPerfEnCours.pinPhysique, LOW);

      // Alarme visuelle
      digitalWrite(etatLocal.ledFin.pinPhysique, LOW);
      etatLocal.ledFin.nbIteration  = 0;
      etatLocal.ledFin.etat == HIGH; // pour ne pas être perturbé par la gestion du clignotement
      
      etatLocal.displayAlarme = alarmRestart; 
      modifBuzzer = true;
      modifDisplay = true;
      Debugln(F("Alarme Fin de seringue OFF"));
    }

// ***************************************************************************************************************** //
    /* 
     * Alarme sur l'état du secteur
     * Le déclenchement se fait sur seuil de tension sur l'entrée Analogique A0
     * La désactivation se fait sur seuil de tension sur l'entrée Analogique A0
     */
    // Activation de l'alarme secteur
    if ((etatLocal.etatSecteur.valeur == LOW) && etatLocal.alarmeSecteur == LOW) {
      etatLocal.alarmeSecteur = HIGH;
      
      // Alarme visuelle
      etatLocal.ledSecteur.freqHaute    = 800;
      etatLocal.ledSecteur.freqBasse    = 300;
      etatLocal.ledSecteur.nbIteration  = -1;

      // on veut que cela sonne au moins une fois donc on repasse à l'état bas le redémarrage :-)
      etatLocal.buzzer.redemarrage = LOW;
      modifBuzzer = true;
      modifDisplay = true;
      etatLocal.displayAlarme = alarmSecteur;
      Debugln(F("Alarme Secteur ON"));
    }
    
    // Désactivation de l'alarme secteur
    if ((etatLocal.etatSecteur.valeur == HIGH) && etatLocal.alarmeSecteur == HIGH) {
      etatLocal.alarmeSecteur = LOW;
      // Alarme visuelle
      etatLocal.ledSecteur.nbIteration  = 0;
      
      etatLocal.displayAlarme = alarmRestart; 
      modifBuzzer = true;
      modifDisplay = true;
      Debugln(F("Alarme Secteur OFF"));
    }

// ***************************************************************************************************************** //
    /* 
     * Alarme sur l'état de la batterie
     * Le déclenchement se fait sur seuil de tension sur l'entrée Analogique A0
     * La désactivation se fait sur seuil de tension sur l'entrée Analogique A0
     */
    // Activation de l'alarme batterie
    if ((etatLocal.etatBatterie.valeur == LOW) && etatLocal.alarmeBatterie == LOW) {
      etatLocal.alarmeBatterie = HIGH;
      // Alarme visuelle
      etatLocal.ledBatterie.freqHaute    = 800;
      etatLocal.ledBatterie.freqBasse    = 300;
      etatLocal.ledBatterie.nbIteration  = -1;

      modifBuzzer = true;
      modifDisplay = true;
      etatLocal.displayAlarme = alarmBatterie;
      Debugln(F("Alarme Batterie ON"));
    }

    // Désactivation de l'alarme batterie
    if ((etatLocal.etatBatterie.valeur == HIGH) && etatLocal.alarmeBatterie == HIGH) {
      etatLocal.alarmeBatterie = LOW;
      // Alarme visuelle
      etatLocal.ledBatterie.nbIteration  = 0;
      
      etatLocal.displayAlarme = alarmRestart; 
      modifBuzzer = true;
      modifDisplay = true;
      Debugln(F("Alarme Batterie OFF"));
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
     * On joue les alternances des voyants / buzzer
     */
    if (modifDisplay == true) {
      etatLocal.alarmeLastDisplay = millis() - timerBoucleAlarme - 1;
      affichageAlarme(etatLocal);
    }
    if (modifBuzzer == true) {
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
      etatLocal.buzzer.volume       = 255;
      etatLocal.buzzer.nbIteration  = -1;
      
    } else if (etatLocal.alarmeFinCourse == HIGH) {
      etatLocal.buzzer.freqHaute    = 600;
      etatLocal.buzzer.freqBasse    = 200;
      etatLocal.buzzer.volume       = 255;
      etatLocal.buzzer.nbIteration  = -1;
      
    } else if (etatLocal.alarmeBatterie == HIGH) {
      etatLocal.buzzer.freqHaute    = 800;
      etatLocal.buzzer.freqBasse    = 300;
      etatLocal.buzzer.volume       = 255;
      etatLocal.buzzer.nbIteration  = -1;

    } else if (etatLocal.alarmeDixPourCent == HIGH) {
      etatLocal.buzzer.freqHaute    = 200;
      etatLocal.buzzer.freqBasse    = 500;
      etatLocal.buzzer.volume       = 255;
      etatLocal.buzzer.nbIteration  = -1;
  
    } else if (etatLocal.alarmeSecteur == HIGH && etatLocal.buzzer.redemarrage == LOW) { // pas de redémarrage ... uniquement la premirèe fois
      etatLocal.buzzer.freqHaute    = 800;
      etatLocal.buzzer.freqBasse    = 300;
      etatLocal.buzzer.volume       = 255;
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
            analogWrite(etatLocal.buzzer.pinPhysique, etatLocal.buzzer.volume); // Faire du bruit
            etatLocal.buzzer.lastTime = currentMillis;
            etatLocal.buzzer.etat = HIGH;
            if (etatLocal.buzzer.nbIteration > 0) { etatLocal.buzzer.nbIteration = etatLocal.buzzer.nbIteration - 1; }
          }
       } else {
          // gestion quand le son a l'état haut
          if (currentMillis - etatLocal.buzzer.lastTime > etatLocal.buzzer.freqHaute || currentMillis < etatLocal.buzzer.lastTime) {
            analogWrite(etatLocal.buzzer.pinPhysique, LOW); // Silence ....
            etatLocal.buzzer.lastTime = currentMillis;
            etatLocal.buzzer.etat = LOW;
            if (etatLocal.buzzer.nbIteration > 0) { etatLocal.buzzer.nbIteration = etatLocal.buzzer.nbIteration - 1; }
          }
       }
    } else if (etatLocal.buzzer.etat == 1) { // le son a été joué le temps voulu. On stoppe tout proprement si le son fonctionne encore
        analogWrite(etatLocal.buzzer.pinPhysique, LOW); // Silence ....
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
