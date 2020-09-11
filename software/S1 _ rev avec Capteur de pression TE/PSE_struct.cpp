// **********************************************************************************
// PSE project, ensemble des fonctions attachées aux structures (init, manipulation,...)
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

#ifndef PSE_struct_cpp
    #define PSE_struct_cpp

#include "./PSE_struct.h"
#include "./PSE_project.h"

// Initialise (variable + physique) la sortie digital x avec la valeur par défault
void initDigitalOutput(byte pin, byte etat, Alerte &alerteLocal) {
  alerteLocal.pinPhysique = pin;
  alerteLocal.etatDefault = etat;
  pinMode(pin,OUTPUT);
  digitalWrite(pin,etat);
}

// Initialise (variable + physique) la sortie digital x avec la valeur par défault
void initAnalogOutput(byte pin, byte etat, Alerte &alerteLocal) {
  alerteLocal.pinPhysique = pin;
  alerteLocal.etatDefault = etat;
  pinMode(pin,OUTPUT);
  analogWrite(pin,etat);
}

// Initialise (variable + physique) l'entrée digital x sans valeur de déclenchement
void initDigitalInput(byte pin, Bouton &boutonLocal) {
  boutonLocal.pinPhysique = pin;
  pinMode(pin,OUTPUT);
}

// Initialise (variable + physique) l'entrée analogique x avec deux valeurs de déclenchement
void initAnalogInput(byte pin, int valeurMin, int valeurMax, Bouton &boutonLocal) {
  boutonLocal.pinPhysique = pin;
  boutonLocal.levelMin = valeurMin;
  boutonLocal.levelMax = valeurMax;
  pinMode(pin,OUTPUT);
}

// relevé de toutes les valeurs sur les entrées analogique
void readAnalogInputs(Etat_Global &etatLocal) {
  etatLocal.valeurA0 = analogRead(pinA0); 
  etatLocal.valeurA1 = analogRead(pinA1); 
  etatLocal.valeurA2 = analogRead(pinA2); 
  etatLocal.valeurA3 = analogRead(pinA3);
}

// obtenir la valeur d'entrée relevée précédemment
int getAnalogValue(Etat_Global etatLocal, byte adresse) {
  if (adresse == pinA0) {return etatLocal.valeurA0;}
  if (adresse == pinA1) {return etatLocal.valeurA1;}
  if (adresse == pinA2) {return etatLocal.valeurA2;}
  if (adresse == pinA3) {return etatLocal.valeurA3;}
  return -1;
}

// Lecture si un bouton a été actionné ou non
void readAnalogInput(Bouton &boutonLocal, Etat_Global &etatLocal){
  int sensorValue = getAnalogValue(etatLocal, boutonLocal.pinPhysique);
    
  if ((boutonLocal.levelMin <= sensorValue) && (sensorValue <= boutonLocal.levelMax)) {
    boutonLocal.cptUp = boutonLocal.cptUp + 1;
    boutonLocal.cptDown = 0;

    if (boutonLocal.cptUp >= cptChangementEtat) {
      boutonLocal.prevValeur = boutonLocal.valeur;
      boutonLocal.valeur = 1;
      boutonLocal.lastUp = millis();
      boutonLocal.cptUp = cptChangementEtat; // pour limiter ce compteur en cas de maintien sur le bouton
    }
  } else {
    boutonLocal.cptUp = 0;
    boutonLocal.cptDown = boutonLocal.cptDown + 1;

    if (boutonLocal.cptDown >= cptChangementEtat) {
        boutonLocal.prevValeur = boutonLocal.valeur;
        boutonLocal.valeur = 0;
        boutonLocal.lastDown = millis();
        boutonLocal.cptDown = cptChangementEtat; // pour limiter ce compteur en cas de maintien sur le bouton
      }
  }

}

// Lecture de la valeur sur l'entrée tension pour ensuite faire une moyenne
void readAnalogInputTensions(byte entree, Etat_Global &etatLocal) {
  if (millis() - etatLocal.valeurTensionLast > 50) {
    int sensorValue = getAnalogValue(etatLocal, entree);
    etatLocal.valeurTension[etatLocal.valeurTensionPos] = sensorValue;
    etatLocal.valeurTensionPos ++;
    if (etatLocal.valeurTensionPos == etatLocal.valeurTensionNbre) {etatLocal.valeurTensionPos = 0;}
    int sommeValue = 0;
    for (int i= 0; i < etatLocal.valeurTensionNbre; i++) {
      sommeValue = sommeValue + etatLocal.valeurTension[i];
    }
    etatLocal.valeurTensionMoy =  sommeValue / etatLocal.valeurTensionNbre; // division qui donne un entier
  }
}

// Lecture si un bouton a été actionné ou non
void readAnalogInputTension(Bouton &boutonLocal, Etat_Global &etatLocal){
  int sensorValue = etatLocal.valeurTensionMoy;
  boutonLocal.prevValeur = boutonLocal.valeur;
    
  if ((boutonLocal.levelMin <= sensorValue) && (sensorValue <= boutonLocal.levelMax)) {
    boutonLocal.cptUp = boutonLocal.cptUp + 1;
    boutonLocal.cptDown = 0;

    if (boutonLocal.cptUp >= cptChangementEtat * 5) {
      boutonLocal.valeur = 1;
      boutonLocal.lastUp = millis();
      boutonLocal.cptUp = cptChangementEtat * 5; // pour limiter ce compteur en cas de maintien sur le bouton
    }
  } else {
    boutonLocal.cptUp = 0;
    boutonLocal.cptDown = boutonLocal.cptDown + 1;

    if (boutonLocal.cptDown >= cptChangementEtat * 5) {
        boutonLocal.valeur = 0;
        boutonLocal.lastDown = millis();
        boutonLocal.cptDown = cptChangementEtat * 5; // pour limiter ce compteur en cas de maintien sur le bouton
      }
  }
}

bool existeAlarme(Etat_Global &etatLocal) {
  if (etatLocal.alarmeSecteur) {return true;}
  else if (etatLocal.alarmeOcclusion) {return true;}
  else if (etatLocal.alarmeDixPourCent) {return true;}
  else if (etatLocal.alarmeFinCourse) {return true;}
  else if (etatLocal.alarmeBatterie) {return true;}

  return false;
}


// calcule 10 puissance le paramètre
long puissanceDix(byte valeur) {
    long result = 10;
    if (valeur == 0) {return 0;}

    for (byte i = 1; i < valeur; i++) {
        result = result * 10;
    }
    return result;
}

// Affiche une valeur préfixée par des 0
String calculValeurString(long valeur,byte nbrDigit) {
    long valeurTemp = puissanceDix(nbrDigit) + valeur;
    return String(valeurTemp).substring(1, nbrDigit + 1);
}

// Copie la valeur dans la valeur Prev => Evite les rebonds 
void updateAllBtn(Etat_Global &etatLocal) {
  etatLocal.finRetour.prevValeur = etatLocal.finRetour.valeur;
  etatLocal.finDistrib.prevValeur = etatLocal.finDistrib.valeur;
  etatLocal.dixPourCent.prevValeur = etatLocal.dixPourCent.valeur;
//  etatLocal.surPression.prevValeur = etatLocal.surPression.valeur; // Unactive after the pressure sensor
  etatLocal.btnUp.prevValeur = etatLocal.btnUp.valeur;
  etatLocal.btnDown.prevValeur = etatLocal.btnDown.valeur;
  etatLocal.btnValid.prevValeur = etatLocal.btnValid.valeur;
  etatLocal.btnStartStop.prevValeur =  etatLocal.btnStartStop.valeur;
  etatLocal.btnBolus.prevValeur = etatLocal.btnBolus.valeur;
  etatLocal.btnAvance.prevValeur = etatLocal.btnAvance.valeur;
  etatLocal.btnRecul.prevValeur = etatLocal.btnRecul.valeur;
  etatLocal.etatTension.prevValeur = etatLocal.etatTension.valeur;
}

#endif
