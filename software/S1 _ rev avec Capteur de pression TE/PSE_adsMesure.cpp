// **********************************************************************************
// PSE project, ads management (tension measurement)
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

#ifndef PSE_adsMesure_cpp
    #define PSE_adsMesure_cpp
    
    #include "PSE_adsMesure.h"
    #include "./PSE_struct.h"

  Adafruit_ADS1115 ads(0x48);
  int newSensADS     = 0;
  int prevSensADS    = 0;

// Initialisation de la communication avec l'écran
void initADS(){
	// initilize the ADS1115
	ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.0078125mV
	ads.begin();
}

/* 
 * Lecture de l'intensité via l'ads
 * -1 => valeur < -20 mA
 *  0 => valeur entre -20 mA et +20 mA
 *  1 => valeur > 20 mA
 */
int sensADS() {
    // < -256 (-20mA) => Charge batterie
    // > +256 (+20mA) => Appareil sur batterie (pas de chargeur)
    // entre les deux => Soit chargeur sans batterie ou soit batterie pleine
  int valeurTemp = ads.readADC_Differential_0_1();
  //Debug(F("Valeur ADS"));
//  Debugln(valeurTemp);
  if (valeurTemp < -256) { return adsCharge; } 
  else if (valeurTemp > 256) { return adsBatterie; } 
  else { return adsFull;}
}

void readADS() {
  newSensADS = sensADS();
}

void changeADS() {
  prevSensADS = newSensADS;
}

boolean isChangedADS() {
  return prevSensADS != newSensADS;
}

int valeurADS() {
  return newSensADS;
}

#endif
