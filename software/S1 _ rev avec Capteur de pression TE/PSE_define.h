#ifndef PSE_Define_H
  #define PSE_Define_H

/*
 * Paramètres modifiables à la compilation et via le port serie
 */
  #define diametreAvRe       250    // diamètre pour l'avance ou le recul rapide
  #define timerRebootBuzzer  90000  // timer définissant le reboot du buzzer après arrêt (bouton valide) si alarme encore active
  #define timerBoucleAlarme  2000   // timer définissant le reboot du buzzer après arrêt (bouton valide) si alarme encore active
  #define uncheckBat         2      // Valeur pour interdire le check de la batterie au démarrage
/*
 * Definition des defines du programme
 */
  // Define des Entrées/sorties Analogique
  #define cptChangementEtat 5
  #define freqChangementEtat 5 // en millisecondes
  #define pinA0 22
  #define pinA1 23
  #define pinA2 24
  #define pinA3 25
  #define pinA4 26
  #define pinA5 27

  // Define des Entrées/sorties Digitals
  #define D0 0
  #define D1 1
  #define D2 2
  #define D3 3
  #define D4 4
  #define D5 5
  #define D6 6
  #define D7 7
  
  // Define pour ADS
  #define adsBatterie  1   // Charge Batterie 
  #define adsCharge    -1  // Sur Batterie
  #define adsFull      0   // Soit chargeur sans batterie ou soit batterie pleine

  // Define des niveaux de seuil du capteur de pression en pourcentage
  #define pressionHaute      50
  #define pressionHauteRecul 40
  #define pressionAccostage  10
  #define vide          0
  #define accostage     1
  #define surpression   2
  #define reculPression 3

  // Define des niveaux de seuil issu du RVLV
  #define seuilHaut 538 // Batterie pleine ou absente 768 = 12V
  #define swaLow 0
  #define swaHigh 122
  #define swbLow 193
  #define swbHigh 294
  #define swcLow 390
  #define swcHigh 469
  #define swdLow 594
  #define swdHigh 650

  // Define des menus / affichages
  #define menuPrincipal    0  // Menu principal
  #define reglSeringue     1  // Réglage de la seringue
  #define reglDebit        2  // Réglage du volume
  #define confAvanceRapide 3  // Avance rapide confirmation
  #define confRetourRapide 10 // Retour rapide confirmation
  #define avanceEnCours    4  // Avance rapide en cours
  #define retourEnCours    8  // Retour Chariot
  #define perfEnCours      5  // perfusion en cours
  #define confBolus        6  // Bolus confirmation
  #define bolusEnCours     7  // Bolus en cours
  #define erreurMessage    9  // Fonction impossible
  #define valDebOblig      11 // Validation du débit obligatoire
  #define valSerOblig      12 // Validation d'une seringue obligatoire
  #define displayAccostage 13 // affichage dans le cas d'un accostage

  // #sectionMoteur : Define des états du moteur 
  #define moteurArret         0  // Moteur arrêté
  #define moteurPerfusion     1  // Perfusion en cours
  #define moteurBolus         2  // Moteur en action de Bolus
  #define moteurAvance        3  // Moteur en action d'avance rapide
  #define moteurRecul         4  // Moteur en action de recul rapide
  #define moteurReculPression 5  // Moteur en action de recul pour supprimer la surpression
   
  // #sectionPosChariot : Define des position du chariot
  #define chariotInconnu      0  // Position inconnu (après redémarrage)
  #define chariotFin          1  // Position entre fin de course et capteur intermédiaire
  #define chariotDebut        2  // Position avant le  capteur intermédiaire (avant la zone de fin)
   
  // Define des id alarme
  #define alarmNone           99 // Aucune alarme affichée
  #define alarmRestart        98 // Redemarre l'affichage
  #define alarmOcclusion      0  // Alarme occlusion
  #define alarmDixPourCent    1  // Alarme 10%
  #define alarmFinCourse      2  // Alarme Fin de course
  #define alarmBatterie       3  // Alarme Batterie
  #define alarmSecteur        4  // Alarme Secteur
  #define alarmBolus          5  // Alarme Bolus

  // Define des délais d'affichage
  #define delaiConfirmation 1000 // délai pour confirmer l'action (bolus, avance, recul)
  #define delaiFinBolus     1500 // délai de maintien de l'affichage du bolus à la fin de l'action
  #define delaiMessage      1000 // délai pour l'affichage du message valid seringue ou débit obligatoire
  #define altIndicateurPerf 1000 // délai d'alternance de l'indicateur de perf en cours

  // Define de la sélection dans les menus
  #define choixSeringue    0  // Choix de la seringue
  #define choixDebit       1  // Réglage du débit

  // Define pour le moteur
  #define Pin_Step  9
  #define Pin_Dir  10
  #define Pin_MS1 13 
  #define Pin_MS2 12
  #define Pin_MS3 11
  #define Pin_Reset 8

  #define AVANCE 0
  #define RECUL  1


#endif
