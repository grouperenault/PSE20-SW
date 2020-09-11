// **********************************************************************************
// PSE project, ensemble des fonctions attachées aux structures (init, manipulation,...)
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

#ifndef PSE_struct_cpp
    #define PSE_struct_cpp

#include "./PSE_struct.h"
#include "./PSE_project.h"

// Initialise (variable + physique) la sortie digital x avec la valeur par défault
void initDigitalOutput(int pin, int etat, Alerte &alerteLocal) {
  alerteLocal.pinPhysique = pin;
  alerteLocal.etatDefault = etat;
  pinMode(pin,OUTPUT);
  digitalWrite(pin,etat);
}

// Initialise (variable + physique) la sortie digital x avec la valeur par défault
void initAnalogOutput(int pin, int etat, Alerte &alerteLocal) {
  alerteLocal.pinPhysique = pin;
  alerteLocal.etatDefault = etat;
  pinMode(pin,OUTPUT);
  analogWrite(pin,etat);
}

// Initialise (variable + physique) l'entrée digital x sans valeur de déclenchement
void initDigitalInput(int pin, Bouton &boutonLocal) {
  boutonLocal.pinPhysique = pin;
  pinMode(pin,OUTPUT);
}

// Initialise (variable + physique) l'entrée analogique x avec deux valeurs de déclenchement
void initAnalogInput(int pin, int valeurMin, int valeurMax, Bouton &boutonLocal) {
  boutonLocal.pinPhysique = pin;
  boutonLocal.levelMin = valeurMin;
  boutonLocal.levelMax = valeurMax;
  pinMode(pin,OUTPUT);
}

// relevé de toutes les valeurs sur les entrées analogique
void readAnalogInputs(Etat_Global &etatLocal) {
  etatLocal.valeurA0 = analogRead(22); 
  etatLocal.valeurA1 = analogRead(23); 
  etatLocal.valeurA2 = analogRead(24); 
  etatLocal.valeurA3 = analogRead(25);
}

// obtenir la valeur d'entrée relevée précédemment
int getAnalogValue(Etat_Global etatLocal, int adresse) {
  if (adresse == 22) {return etatLocal.valeurA0;}
  if (adresse == 23) {return etatLocal.valeurA1;}
  if (adresse == 24) {return etatLocal.valeurA2;}
  if (adresse == 25) {return etatLocal.valeurA3;}
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
      boutonLocal.cptUp = 5; // pour limiter ce compteur en cas de maintien sur le bouton
    }
  } else {
    boutonLocal.cptUp = 0;
    boutonLocal.cptDown = boutonLocal.cptDown + 1;

    if (boutonLocal.cptDown >= cptChangementEtat) {
        boutonLocal.prevValeur = boutonLocal.valeur;
        boutonLocal.valeur = 0;
        boutonLocal.lastDown = millis();
        boutonLocal.cptDown = 5; // pour limiter ce compteur en cas de maintien sur le bouton
      }
  }

}

// Lecture de la valeur sur l'entrée tension pour ensuite faire une moyenne
void readAnalogInputTensions(int entree, Etat_Global &etatLocal) {
  if (millis() - etatLocal.valeurTensionLast > 50) {
    int sensorValue = getAnalogValue(etatLocal, entree);
    etatLocal.valeurTension[etatLocal.valeurTensionPos] = sensorValue;
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
    
  if ((boutonLocal.levelMin <= sensorValue) && (sensorValue <= boutonLocal.levelMax)) {
    boutonLocal.cptUp = boutonLocal.cptUp + 1;
    boutonLocal.cptDown = 0;

    if (boutonLocal.cptUp >= cptChangementEtat * 5) {
      boutonLocal.prevValeur = boutonLocal.valeur;
      boutonLocal.valeur = 1;
      boutonLocal.lastUp = millis();
      boutonLocal.cptUp = 5; // pour limiter ce compteur en cas de maintien sur le bouton
    }
  } else {
    boutonLocal.cptUp = 0;
    boutonLocal.cptDown = boutonLocal.cptDown + 1;

    if (boutonLocal.cptDown >= cptChangementEtat * 5) {
        boutonLocal.prevValeur = boutonLocal.valeur;
        boutonLocal.valeur = 0;
        boutonLocal.lastDown = millis();
        boutonLocal.cptDown = 5; // pour limiter ce compteur en cas de maintien sur le bouton
      }
  }
}

bool existeAlarme(Etat_Global &etatLocal) {
  if (etatLocal.alarmeBatterie) {return true;}
  else if (etatLocal.alarmeOcclusion) {return true;}
  else if (etatLocal.alarmeDixPourCent) {return true;}
  else if (etatLocal.alarmeFinCourse) {return true;}
  else if (etatLocal.alarmeSecteur) {return true;}

  return false;
}
#endif
