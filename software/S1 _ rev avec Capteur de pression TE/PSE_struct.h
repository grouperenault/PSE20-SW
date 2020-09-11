#ifndef PSE_Struct_H
  #define PSE_Struct_H

      #include "Arduino.h"
  
/*
 * Definition des structures du programme
 */
      struct Alerte {
        int      pinPhysique   = 0; // PIN sur laquelle il faut faire l'action
        unsigned int freqHaute = 0; // fréquence de l'alerte en Milliseconde => allumage
        unsigned int freqBasse = 0; // fréquence de l'alerte en Milliseconde => arret de l'allumage
        unsigned long lastTime = 0; // dernier changement d'état en milliseconde
        boolean  etat          = 0; // pour l'alternance (savoir si nous sommes en position haute ou basse)
        boolean  etatDefault   = 0; // état mis en place quand on stoppe les animations
        int      nbIteration   = 0; // nombre d'itérations restantes avant fin alarme (-1 pas d'arrêt avant la validation de l'infirmier)
        unsigned int volume    = 0; // volume = fréquence du buzzer
        boolean  redemarrage   = 0; // 1 => Arret sur bouton valide et prévoir un redemarrage après x seconde (voir dans liste des define)
      };
      
      struct Bouton {
        byte              pinPhysique   = 0; // PIN sur laquelle nous lisons la valeur
        unsigned int      levelMin      = 0; // Voltage minimum de captation
        unsigned int      levelMax      = 0; // Voltage minimum de captation
        unsigned long     lastUp        = 0; // dernier changement d'état en milliseconde
        unsigned long     lastDown      = 0; // dernier changement d'état en milliseconde
        boolean           valeur        = 0; // derniere valeur captée
        boolean           prevValeur    = 0; // avant derniere valeur captée
        byte              cptUp         = 0; // quand > 5 on modifie la valeur  ... permet de filtrer
        byte              cptDown       = 0; // quand > 5 on modifie la valeur  ... permet de filtrer
      };

      struct Volume {
        unsigned int      volGlobale  = 0; // volume en rélle en ml
        byte              volCentaine = 0; // chiffre des centaines
        byte              volDixaine  = 0; // chiffre des dixaines
        byte              volUnite    = 0; // chiffre des unités
        byte              volDixieme  = 0; // chiffre des dixièmes
      };
      
      /******************** configuration structure ********************************/
      struct Etat_Global {
        byte    positionChariot     = 0; // Use constante defined in : #sectionPosChariot
        byte    etatMoteur          = 0; // Use constante defined in : #sectionMoteur
        byte    prevEtatMoteur      = 0; // Etat permettant de mesurer les demandes de changement d'état du moteur
        unsigned int volBolus       = 0;
        unsigned int prevVolBolus   = 0;
        unsigned long position10    = 0; // nombre de pas entre fin de course avant et 10% => variable stocké en eproom
        unsigned long positionFin   = 0; // nombre de pas entre fin de course avant et fin de course arrière => variable stocké en eproom

        // parametre configurable
        unsigned long volumeBolus        = 0; // Volume qui définit la vitesse du bolus => unité dixième de ml
        unsigned long volumeBolusMax     = 0; // Volume max autorisé lors d'un bolus => unité dixième de ml
        unsigned long volumeAvance       = 0; // 5000ml/h de volume pour avoir une vitesse élevée de moteur => unité dixième de ml
        unsigned long volumeRecul        = 0; // 5000ml/h de volume pour avoir une vitesse élevée de moteur => unité dixième de ml
        byte adresseLCD                  = 0; // 0x27   // Adresse de l'écran LCD
        
        // définition du débit 
        Volume debitEnCours;
        Volume avantReglage;
        
        // Elément permettant l'alertance de "X" et "+" pendant le fonctionnement du moteur
        unsigned long lastCarMot      = 0; // dernier changement d'état en milliseconde
        boolean carMot                = 0; // 0 => "X", 1 => "+"
        
        /* Valeur analogique */
        unsigned long lastReadAnalog    = 0;
        unsigned int valeurA0           = 0;
        unsigned int valeurA1           = 0;
        unsigned int valeurA2           = 0;
        unsigned int valeurA3           = 0;
        unsigned int valeurTension[8]   = {1024,1024,1024,1024,1024,1024,1024,1024};
        const int valeurTensionNbre     = 8; 
        byte valeurTensionPos           = 0; 
        unsigned long valeurTensionLast = 0;
        unsigned int valeurTensionMoy   = 1024;
        byte etatBatAfterReboot         = 0; // 0 => pas de batterie, 1 => batterie, 2 => pas de check batterie
        Bouton etatTension;    // Mesure la tension pour vérifier l'état du circuit

        /* Menu/Afficheur */
        byte affichageEnCours   = 0; // 0 => Menu, 1 => Réglage de la seringue, 2 => Réglage du volume, 3 ou 10 => Avance ou retour rapide confirmation
                                    // 4 => Avance rapide en cours, 5 => perfusion en cours, 6 => Bolus confirmation, 7 => Bolus en cours, 8 => Retour Chariot, 
                                    // 9 => Fonction impossible
        boolean choixEnCours    = 0; // 0 => Choix seringue, 1 => Choix volume
        byte positionVol        = 0; // 0 => chiffre des centaines, 1 => chiffre des dizaines, 2 => chiffre des unités, 3 => chiffre des dizièmes
        byte positionSer        = 0; // Id de la seringue en cours de sélection
        byte validationDebit    = 0; // Pour obliger la validation du débit avant perfusion
        byte validationSeringue = 0; // Pour obliger la validation de la seringue avant perfusion
        
        /* Moteur */
        boolean   etatNEMA      = LOW;
        
        /* Pilotage son */
        Alerte buzzer;

        /* Pilotage des LEDs */
        Alerte ledFin;         // LED permettant d'afficher la fin de distribution 
                               // + 10% restant + distribution en cours
                               // => L'alternance permettra de définir le cas
        Alerte ledOcclusion;   // LED permettant d'afficher l'alerte d'occlusion
        Alerte ledPerfEnCours; // LED indiquant quand une perfusion est en cours
        Alerte ledBatterie;    // LED pour indiquer que la batterie est en cours d'utilisation
        Alerte ledSecteur;     // LED pour indiquer que le secteur est branché
        Alerte relaisSecteur;


        /* Liste des boutons */
        Bouton btnUp;             // Bouton de navigation dans les menus
        Bouton btnDown;           // Bouton de navigation dans les menus
        Bouton btnValid;          // Validation des actions de l'infirmière
        Bouton btnStartStop;      // Debut de la distribution
        Bouton btnBolus;          // Petit shoot supplémentaire
        Bouton btnAvance;         // Avance pour permettre de mettre en contact le chariot avec la seringue
        Bouton btnRecul;          // Recul pour reculer le chariot
        
        /* Liste des capteurs (assimilable aux boutons */
        Bouton finDistrib;     // Capteur qui capte que le chariot est en butée avant
        Bouton finRetour;      // Capteur qui capte que le chariot est en butée arrière
        Bouton dixPourCent;    // Capteur qui capte que le chariot est en butée arrière
        //Bouton surPression;    // Capteur qui capte que la surpression de la perfusion
        

        /* Liste alarme */
        boolean alarmeOcclusion      = 0; // 0 => Pas d'alarme, 1 => Alarme en cours
        boolean alarmeDixPourCent    = 0; // 0 => Pas d'alarme, 1 => Alarme en cours
        boolean alarmeFinCourse      = 0; // 0 => Pas d'alarme, 1 => Alarme en cours
        boolean alarmeSecteur        = 0; // 0 => Pas d'alarme, 1 => Alarme en cours
        boolean alarmeBatterie       = 0; // 0 => Pas d'alarme, 1 => Alarme en cours
        boolean alarmeFinBolus       = 0; // 0 => Pas d'alarme, 1 => Limite atteint
        unsigned long alarmeLastDisplay = 0; // délai dans l'affichage des alarmeSecteur
        int displayAlarme           = -1; // -1=> none, 0 => occlusion, 1 => 10%, 2 => fin de course, 3 => batterie, 4 => secteur, 5 => bolus
        int lastDisplayAlarme       = -1; // permet de définir quel message sera affiché après

        /* Valeur seringue */
        byte          seringueSel         = 0; // seringue sélectionnée
        const  char   seringueList[6][17] = {"BD 50ml", "seringue2", "seringue3", "seringue4", "seringue5", "seringue6"};
        const  char   seringueShort[6][17] = {"BD 50ml", "short 2", "short 3", "short 4", "short 5", "short 6"};
        const  int    seringueDiam[6]     = {265        , 262        , 261        , 263        , 264        , 266        };
        const  byte   seringueNbre        = 6; // nombre de seringues dans la liste
        // diametre seringue / ou section

      };

      void initDigitalOutput(byte pin, byte etat, Alerte &alerteLocal);
      void initAnalogOutput(byte pin, byte etat, Alerte &alerteLocal);
      void initDigitalInput(byte pin, Bouton &boutonLocal);
      void initAnalogInput(byte pin, int valeurMin, int valeurMax, Bouton &boutonLocal);
      void readAnalogInputs(Etat_Global &etatLocal);
      void readAnalogInput(Bouton &boutonLocal, Etat_Global &etatLocal);
      void readAnalogInputTensions(byte entree, Etat_Global &etatLocal);
      void readAnalogInputTension(Bouton &boutonLocal, Etat_Global &etatLocal);
      int  getAnalogValue(Etat_Global etatLocal, byte adresse);
      bool existeAlarme(Etat_Global &etatLocal);
      void updateAllBtn(Etat_Global &etatLocal);

      long puissanceDix(byte valeur);
      String calculValeurString(long valeur,byte nbrDigit);
#endif
