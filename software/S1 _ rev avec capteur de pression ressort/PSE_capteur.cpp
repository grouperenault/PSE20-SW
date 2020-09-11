// **********************************************************************************
// PSE project, actionneur management
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

#ifndef PSE_capteur_cpp
    #define PSE_capteur_cpp

#include "PSE_capteur.h"
#include "./PSE_eeprom.h"

void lectureEntrees(Etat_Global &etatLocal){
    if (millis() - etatLocal.lastReadAnalog > 10) {
      /* 
      * Lecture de toutes les entrées analogique 
      */
      readAnalogInputs(etatLocal);

      /*
      * Position des états des boutons suite à la lecture précédente
      */
      readAnalogInputTensions(etatLocal.etatBatterie.pinPhysique,etatLocal);
      readAnalogInputTension(etatLocal.etatBatterie, etatLocal); // A0
      readAnalogInputTension(etatLocal.etatSecteur, etatLocal); // A0

      readAnalogInput(etatLocal.finRetour, etatLocal); // A1
      readAnalogInput(etatLocal.finDistrib, etatLocal); // A1
      readAnalogInput(etatLocal.dixPourCent, etatLocal); // A1
      readAnalogInput(etatLocal.surPression, etatLocal); // A1
      readAnalogInput(etatLocal.btnUp, etatLocal); // A2
      readAnalogInput(etatLocal.btnDown, etatLocal); // A2
      readAnalogInput(etatLocal.btnValid, etatLocal); // A2
      readAnalogInput(etatLocal.btnStartStop, etatLocal); // A3
      readAnalogInput(etatLocal.btnBolus, etatLocal); // A3
      readAnalogInput(etatLocal.btnAvance, etatLocal); // A3
      readAnalogInput(etatLocal.btnRecul, etatLocal); // A3

      etatLocal.lastReadAnalog = millis();
    }
}

void boutonManagement(Etat_Global &etatLocal){
    boolean modifDisplay = false;

    /*
     * Traitement des actions en fonction des boutons/actions utilisateurs
     */
    // Arret des alarmes sonore avec le bouton validation
    if (etatLocal.btnValid.valeur == HIGH && etatLocal.displayAlarme != alarmNone) {
      Debugln(F("Suspension alarmes"));
      // Arrêt des alertes sonore
      etatLocal.buzzer.nbIteration  = 0;
      etatLocal.buzzer.lastTime = millis();
      etatLocal.buzzer.redemarrage = HIGH; // on programme un redémarrage du buzzer (et écran)

      // Arrêt des affichages d'alarme
      etatLocal.displayAlarme = alarmNone;
      etatLocal.lastDisplayAlarme = alarmNone;

      modifDisplay = true;
    } else {
      // Actions autorisées unique en mode : arret de la diffusion
      if (etatLocal.etatMoteur == moteurArret || etatLocal.etatMoteur == moteurAvance || etatLocal.etatMoteur == moteurRecul) {
        /* 
         * Actions autorisées quand le menu principal est affiché 
         */
        if (etatLocal.affichageEnCours == menuPrincipal) {
  
            // boutons Up and Down sur l'écran de menu principal
            if ((etatLocal.btnUp.valeur != etatLocal.btnUp.prevValeur && etatLocal.btnUp.valeur == HIGH) || 
                (etatLocal.btnDown.valeur != etatLocal.btnDown.prevValeur && etatLocal.btnDown.valeur == HIGH)){
  
                if (etatLocal.choixEnCours == choixSeringue) { etatLocal.choixEnCours = choixDebit;} 
                  else {etatLocal.choixEnCours = choixSeringue;}
  
                modifDisplay = true;
            } else {
  
              // Valide sur l'écran de menu on rentre dans un sous menu 
              if ((etatLocal.btnValid.valeur != etatLocal.btnValid.prevValeur) && (etatLocal.btnValid.valeur == HIGH)) {
                  if (etatLocal.choixEnCours == choixSeringue) { 
                    etatLocal.positionSer = etatLocal.seringueSel; // Report de la valeur de seringue en cours
                    etatLocal.affichageEnCours = reglSeringue;
  
                    modifDisplay = true;
                  } else { 
                    etatLocal.affichageEnCours = reglDebit;
                    etatLocal.avantReglage.volGlobale = etatLocal.debitEnCours.volGlobale;
                    
                    modifDisplay = true;
                  }
              } else {
              
                // Affichage message si bouton d'avance rapide appuyé pour la première fois ou une seconde fois > 1s
                if ((etatLocal.btnAvance.valeur != etatLocal.btnAvance.prevValeur) && (etatLocal.btnAvance.valeur == HIGH)) {          
                  etatLocal.affichageEnCours = confAvanceRapide;
  
                  modifDisplay = true;
                } else  {
            
                  // Affichage message si bouton d'avance rapide appuyé pour la première fois ou une seconde fois > 1s
                  if ((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur) && (etatLocal.btnRecul.valeur == HIGH)) {          
                    etatLocal.affichageEnCours = confRetourRapide;
  
                    modifDisplay = true;
                  } else {
                    
                    // Message d'erreur pour le bouton de bolus
                    if ((etatLocal.btnBolus.valeur != etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == HIGH)) {
                      etatLocal.affichageEnCours = erreurMessage;
  
                      modifDisplay = true;
                    } else {
  
                      // Démarrage de la perfusion
                      if ((etatLocal.btnStartStop.valeur != etatLocal.btnStartStop.prevValeur) && (etatLocal.btnStartStop.valeur == HIGH)) {          
                        if (etatLocal.avantReglage.volGlobale != etatLocal.debitEnCours.volGlobale) {
                            sauvegardeDebit(etatLocal); // Sauvegarde sur Eeprom
                        }
                        
                        // Statut permettant de démarrer la perfusion
                        etatLocal.etatMoteur = moteurPerfusion;
  
                        digitalWrite(etatLocal.ledPerfEnCours.pinPhysique, HIGH); 
                        etatLocal.affichageEnCours = perfEnCours;
                          
                        modifDisplay = true;
                      }
                    }
                  }
                }
              }
            }
        } else {
         /* 
          * Actions quand le réglage de seringue est affiché
          */
          if (etatLocal.affichageEnCours == reglSeringue) {
            // Up sur l'écran de sélection de la seringue
            if ((etatLocal.btnDown.valeur != etatLocal.btnDown.prevValeur) && (etatLocal.btnDown.valeur == HIGH)) {
              // Gestion de l'affichage cyclique des valeurs
              if (etatLocal.positionSer != 0) { etatLocal.positionSer = etatLocal.positionSer - 1;}
                else if (etatLocal.positionSer == 0) { etatLocal.positionSer = etatLocal.seringueNbre - 1;}
              
              modifDisplay = true;
            } else {
  
              // Down sur l'écran de sélection de la seringue
              if ((etatLocal.btnUp.valeur != etatLocal.btnUp.prevValeur) && (etatLocal.btnUp.valeur == HIGH)) {
                // Gestion de l'affichage cyclique des valeurs
                if (etatLocal.positionSer != etatLocal.seringueNbre - 1) { etatLocal.positionSer = etatLocal.positionSer + 1;}
                else if (etatLocal.positionSer == etatLocal.seringueNbre - 1) { etatLocal.positionSer = 0;}
  
                modifDisplay = true;
              } else {
                
                // Valide sur l'écran de réglage de la seringue
                if ((etatLocal.btnValid.valeur != etatLocal.btnValid.prevValeur) && (etatLocal.btnValid.valeur == 1)) {
  
                  // sauvegarde sur eeprom si changement de valeur
                  if (etatLocal.positionSer != etatLocal.seringueSel) {
                    etatLocal.seringueSel = etatLocal.positionSer;
                    sauvegardeSeringue(etatLocal); // Sauvegarde sur Eeprom
                  }
                  etatLocal.affichageEnCours = menuPrincipal;
  
                  modifDisplay = true;
                }
              }
            }
          } else {
           /* 
            * Actions quand le réglage du débit est affiché
            */
            if (etatLocal.affichageEnCours == reglDebit) {
              
              // Valide sur l'écran de réglage du débit 
              if ((etatLocal.btnValid.valeur != etatLocal.btnValid.prevValeur) && (etatLocal.btnValid.valeur == HIGH)) {
                etatLocal.positionVol = etatLocal.positionVol + 1; // changement de digit pour le réglage du débit
                if (etatLocal.positionVol == 4) { // dernière position ... on quitte l'écran
                  etatLocal.positionVol = 0;
                  etatLocal.affichageEnCours = menuPrincipal;
                  
                  // Calcul du débit complet
                  etatLocal.debitEnCours.volGlobale = etatLocal.debitEnCours.volCentaine * 1000 + etatLocal.debitEnCours.volDixaine * 100 + etatLocal.debitEnCours.volUnite * 10 + etatLocal.debitEnCours.volDixieme;
                }
  
                modifDisplay = true;
              } else {
                // Up sur l'écran de sélection du débit sur le digit en cours de sélection
                if ((etatLocal.btnUp.valeur != etatLocal.btnUp.prevValeur) && (etatLocal.btnUp.valeur == HIGH)) {
                    if ((etatLocal.positionVol == 0) && (etatLocal.debitEnCours.volCentaine ==0)) { etatLocal.debitEnCours.volCentaine = 9;}
                    else if ((etatLocal.positionVol == 0) && (etatLocal.debitEnCours.volCentaine !=0)) { etatLocal.debitEnCours.volCentaine = etatLocal.debitEnCours.volCentaine - 1;}
                    if ((etatLocal.positionVol == 1) && (etatLocal.debitEnCours.volDixaine ==0)) { etatLocal.debitEnCours.volDixaine = 9;}
                    else if ((etatLocal.positionVol == 1) && (etatLocal.debitEnCours.volDixaine !=0)) { etatLocal.debitEnCours.volDixaine = etatLocal.debitEnCours.volDixaine - 1;}
                    if ((etatLocal.positionVol == 2) && (etatLocal.debitEnCours.volUnite ==0)) { etatLocal.debitEnCours.volUnite = 9;}
                    else if ((etatLocal.positionVol == 2) && (etatLocal.debitEnCours.volUnite !=0)) { etatLocal.debitEnCours.volUnite = etatLocal.debitEnCours.volUnite - 1;}
                    if ((etatLocal.positionVol == 3) && (etatLocal.debitEnCours.volDixieme ==0)) { etatLocal.debitEnCours.volDixieme = 9;}
                    else if ((etatLocal.positionVol == 3) && (etatLocal.debitEnCours.volDixieme !=0)) { etatLocal.debitEnCours.volDixieme = etatLocal.debitEnCours.volDixieme - 1;}
  
                    modifDisplay = true;
                } else {
                  
                  // Down sur l'écran de sélection du débit sur le digit en cours de sélection
                  if ((etatLocal.btnDown.valeur != etatLocal.btnDown.prevValeur) && (etatLocal.btnDown.valeur == HIGH)) {
                    if ((etatLocal.positionVol == 0) && (etatLocal.debitEnCours.volCentaine ==9)) { etatLocal.debitEnCours.volCentaine = 0;}
                    else if ((etatLocal.positionVol == 0) && (etatLocal.debitEnCours.volCentaine !=9)) { etatLocal.debitEnCours.volCentaine = etatLocal.debitEnCours.volCentaine + 1;}
                    if ((etatLocal.positionVol == 1) && (etatLocal.debitEnCours.volDixaine ==9)) { etatLocal.debitEnCours.volDixaine = 0;}
                    else if ((etatLocal.positionVol == 1) && (etatLocal.debitEnCours.volDixaine !=9)) { etatLocal.debitEnCours.volDixaine = etatLocal.debitEnCours.volDixaine + 1;}
                    if ((etatLocal.positionVol == 2) && (etatLocal.debitEnCours.volUnite ==9)) { etatLocal.debitEnCours.volUnite = 0;}
                    else if ((etatLocal.positionVol == 2) && (etatLocal.debitEnCours.volUnite !=9)) { etatLocal.debitEnCours.volUnite = etatLocal.debitEnCours.volUnite + 1;}
                    if ((etatLocal.positionVol == 3) && (etatLocal.debitEnCours.volDixieme ==9)) { etatLocal.debitEnCours.volDixieme = 0;}
                    else if ((etatLocal.positionVol == 3) && (etatLocal.debitEnCours.volDixieme !=9)) { etatLocal.debitEnCours.volDixieme = etatLocal.debitEnCours.volDixieme + 1;}
  
                    modifDisplay = true;
                  }
                }
              }
            } else {
             /* 
              * Actions quand l'écran de confirmation de l'avance rapide est affiché
              */
              if (etatLocal.affichageEnCours == confAvanceRapide) {
                
                // On déclenche l'avance rapide
                if (((etatLocal.btnAvance.valeur != etatLocal.btnAvance.prevValeur) && (etatLocal.btnAvance.valeur == HIGH)) && 
                    (millis() - etatLocal.btnAvance.lastUp < delaiConfirmation)) {          

                   etatLocal.etatMoteur = moteurAvance;
                   etatLocal.affichageEnCours = avanceEnCours;
                   
                   modifDisplay = true;
                 } else {
                 
                   // On reviens au menu principal si le déclanchement de l'avance rapide est plus vieille de 1s
                   if (millis() - etatLocal.btnAvance.lastUp > delaiConfirmation) {          
                     etatLocal.affichageEnCours = menuPrincipal;
  
                     modifDisplay = true;
                   }
                 }
              } else {
                /*
                 * Actions quand l'écran d'avance rapide est affiché
                 */
                // Arrêt de l'avance rapide
                if ((etatLocal.affichageEnCours == avanceEnCours) && (((etatLocal.btnAvance.valeur != etatLocal.btnAvance.prevValeur) && (etatLocal.btnAvance.valeur == LOW))
                    || (etatLocal.finDistrib.valeur == HIGH) || (etatLocal.surPression.valeur == HIGH))) {

                  etatLocal.etatMoteur = moteurArret;
                  etatLocal.affichageEnCours = menuPrincipal;
  
                  modifDisplay = true;
                } else {
                 /* 
                  * Actions quand l'écran de confirmation de l'avance rapide est affiché
                  */
                  if (etatLocal.affichageEnCours == confRetourRapide) {
           
                    // On déclenche le retour
                    if (((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur) && (etatLocal.btnRecul.valeur == HIGH)) && 
                        (millis() - etatLocal.btnRecul.lastUp < delaiConfirmation)) {          

                      etatLocal.etatMoteur = moteurRecul;
                      etatLocal.affichageEnCours = retourEnCours;
  
                      modifDisplay = true;
                    } else {
                      
                      // On reviens au menu si le déclanchement du retour rapide est plus vieille de 1s
                      if (millis() - etatLocal.btnRecul.lastUp > delaiConfirmation) {
                        etatLocal.affichageEnCours = menuPrincipal;
  
                        modifDisplay = true;
                      }
                    }
                  } else {
                    /*
                     * Gestion du retour
                     */
                    // Arrêt du retour
                    if ((etatLocal.affichageEnCours == retourEnCours) && ((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur 
                        && etatLocal.btnRecul.valeur == LOW) || etatLocal.finRetour.valeur == HIGH)) {
                      
                      etatLocal.etatMoteur = moteurArret;
                      etatLocal.affichageEnCours = menuPrincipal;
  
                      modifDisplay = true;
      
                    } else {
                      /*
                       * Gestion du message d'erreur Bolus
                       */
                      // Affichage du message d'erreur pour le bouton de bolus
                      if ((etatLocal.affichageEnCours == erreurMessage) && (etatLocal.btnBolus.valeur != etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == LOW)) {
                        etatLocal.affichageEnCours = menuPrincipal;
  
                        modifDisplay = true;
                      }
                    }
  
                  }
                }
              }
  
            }
          }
      
        }
  // ***************************************************************************************************************** //
      } else if (etatLocal.etatMoteur == moteurPerfusion || etatLocal.etatMoteur ==  moteurBolus){  // Actions autorisées unique en mode : diffusion en cours
       /*
        * Diffusion affiché sur l'écran
        */
        if (etatLocal.affichageEnCours == perfEnCours) { 
          // Message d'erreur pour les boutons avance et retour chariot
          if (((etatLocal.btnAvance.valeur != etatLocal.btnAvance.prevValeur) && (etatLocal.btnAvance.valeur == HIGH)) ||
              ((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur) && (etatLocal.btnRecul.valeur == HIGH))) {
    
            etatLocal.affichageEnCours = erreurMessage;
  
            modifDisplay = true;
          } else {
    
              // Arrêt de la perfusion
              if ((etatLocal.btnStartStop.valeur != etatLocal.btnStartStop.prevValeur) && (etatLocal.btnStartStop.valeur == HIGH)) {
                
                digitalWrite(etatLocal.ledPerfEnCours.pinPhysique, LOW);
                
                etatLocal.etatMoteur =  moteurArret;
                etatLocal.affichageEnCours = menuPrincipal;

                modifDisplay = true;
              } else {
  
                // Affichage message si bouton de bolus appuyé pour la première fois ou une seconde fois > 1s
                if ((etatLocal.btnBolus.valeur != etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == HIGH)) {          

                  etatLocal.affichageEnCours = confBolus;
                  modifDisplay = true;
                }
              }
            }
          } else {
           /* 
            *  Affichage de l'écran d'erreur
            */
            if (etatLocal.affichageEnCours == erreurMessage) {
              // Suppression du message d'erreur pour les boutons avance et retour chariot
              if (((etatLocal.btnAvance.valeur != etatLocal.btnAvance.prevValeur) && (etatLocal.btnAvance.valeur == LOW)) ||
                  ((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur) && (etatLocal.btnRecul.valeur == LOW))) {

                etatLocal.affichageEnCours = perfEnCours;
                modifDisplay = true;
              }
            } else {
             /* 
              *  Affichage de l'écran de confirmation du bolus
              */
              if (etatLocal.affichageEnCours == confBolus) {
                  
                // On déclenche le bolus
                if ((etatLocal.btnBolus.valeur != etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == HIGH) && 
                   (millis() - etatLocal.btnBolus.lastUp < delaiConfirmation)) {          
                  
                  etatLocal.affichageEnCours = bolusEnCours;
                  etatLocal.volBolus = -1;
                  etatLocal.etatMoteur =  moteurBolus;
                  
                  modifDisplay = true;
                } else {
                  
                  // On reviens au menu si le déclanchement de l'avance rapide est plus vieille de 1s
                  if (millis() - etatLocal.btnBolus.lastUp > delaiConfirmation) {          
                    
                    etatLocal.affichageEnCours = perfEnCours;
      
                    modifDisplay = true;
                  }
                }
              } else {
               /* 
                *  Affichage de l'écran de bolus en cours
                */
                if (etatLocal.affichageEnCours == bolusEnCours) {
                  // Arrêt du bolus
                  if ((etatLocal.btnBolus.valeur != etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == LOW)) {
                    // Remise à l'état de perfusion du moteur
                    etatLocal.etatMoteur =  moteurPerfusion;
                    
                    textLCD("Arret bolus    ",1,0);
                  } else {
/*                    // Rafraichissement affichage pour le Bolus
                    if ((etatLocal.btnBolus.valeur == etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == HIGH)) {          
                      
                      refreshBolus(etatLocal);
                    } else {
*/
                       if ((etatLocal.btnBolus.valeur == etatLocal.btnBolus.prevValeur) && (etatLocal.btnBolus.valeur == LOW)
                        && (millis() - etatLocal.btnBolus.lastUp > delaiFinBolus)) {
                        
                        etatLocal.affichageEnCours = perfEnCours;
                        modifDisplay = true;
                      }
//                    }
                  }
                }
              }
            }
          }
      } 
    }
    if (modifDisplay == true) {menuLCD(etatLocal);} // Mise à jour du menu
}

#endif
