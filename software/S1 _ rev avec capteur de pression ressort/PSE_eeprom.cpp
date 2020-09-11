// **********************************************************************************
// PSE project, eeprom management
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
#ifndef PSE_eeprom_cpp
    #define PSE_eeprom_cpp
    
#include "./PSE_eeprom.h"
#include "./PSE_project.h"

void sauvegardeSeringue(Etat_Global &etatLocal){
  EEPROMWriteInt(0,etatLocal.seringueSel);
}

void sauvegardeDebit(Etat_Global &etatLocal){
  EEPROMWriteInt(2,etatLocal.debitEnCours.volCentaine);
  EEPROMWriteInt(4,etatLocal.debitEnCours.volDixaine);
  EEPROMWriteInt(6,etatLocal.debitEnCours.volUnite);
  EEPROMWriteInt(8,etatLocal.debitEnCours.volDixieme);
}

void sauvegardeAdresseLCD(Etat_Global &etatLocal){
  EEPROMWriteInt(10,etatLocal.adresseLCD);
}

void sauvegardeVolumeBolus(Etat_Global &etatLocal){
  EEPROMWriteLong(12,etatLocal.volumeBolus);
}

void sauvegardeVolumeAvance(Etat_Global &etatLocal){
  EEPROMWriteLong(16,etatLocal.volumeAvance);
}

void sauvegardeVolumeRecul(Etat_Global &etatLocal){
  EEPROMWriteLong(20,etatLocal.volumeRecul);
}

void sauvegardeVolumeBolusMax(Etat_Global &etatLocal){
  EEPROMWriteLong(24,etatLocal.volumeBolusMax);
}

void restaureValeurs(Etat_Global &etatLocal){
  etatLocal.seringueSel              = EEPROMReadInt(0);
  etatLocal.debitEnCours.volCentaine = EEPROMReadInt(2);
  etatLocal.debitEnCours.volDixaine  = EEPROMReadInt(4);
  etatLocal.debitEnCours.volUnite    = EEPROMReadInt(6);
  etatLocal.debitEnCours.volDixieme  = EEPROMReadInt(8);
  etatLocal.adresseLCD               = EEPROMReadInt(10);
  etatLocal.volumeBolus              = EEPROMReadLong(12);
  etatLocal.volumeAvance             = EEPROMReadLong(16);
  etatLocal.volumeRecul              = EEPROMReadLong(20);
  etatLocal.volumeBolusMax           = EEPROMReadLong(24);

  // vérification si la valeur est possible
  if (etatLocal.seringueSel > etatLocal.seringueNbre-1) {etatLocal.seringueSel = 0;}
  if (etatLocal.debitEnCours.volCentaine > 9 || etatLocal.debitEnCours.volCentaine <0) {etatLocal.debitEnCours.volCentaine = 0;}
  if (etatLocal.debitEnCours.volDixaine > 9 || etatLocal.debitEnCours.volDixaine <0) {etatLocal.debitEnCours.volDixaine = 0;}
  if (etatLocal.debitEnCours.volUnite > 9 || etatLocal.debitEnCours.volUnite <0) {etatLocal.debitEnCours.volUnite = 0;}
  if (etatLocal.debitEnCours.volDixieme > 9 || etatLocal.debitEnCours.volDixieme <0) {etatLocal.debitEnCours.volDixieme = 0;}

  // récupération des paramètres
  if (etatLocal.adresseLCD > 127 || etatLocal.adresseLCD <0) {etatLocal.adresseLCD = 0;}
  if (etatLocal.volumeBolus > 10000 || etatLocal.volumeBolus <0) {etatLocal.volumeBolus = 1500;}
  if (etatLocal.volumeAvance > 1000000 || etatLocal.volumeAvance <0) {etatLocal.volumeAvance = 50000;}
  if (etatLocal.volumeRecul > 1000000 || etatLocal.volumeRecul <0) {etatLocal.volumeRecul = 90000;}
  if (etatLocal.volumeBolusMax > 100 || etatLocal.volumeBolusMax <0) {etatLocal.volumeBolusMax = 50;}


  // calcul de la valeur globale du débit
  etatLocal.debitEnCours.volGlobale = etatLocal.debitEnCours.volCentaine * 1000 + etatLocal.debitEnCours.volDixaine * 100 + etatLocal.debitEnCours.volUnite * 10 + etatLocal.debitEnCours.volDixieme;
}

void EEPROMWriteInt(int address, int valeur)
{
  byte two = (valeur & 0xFF);
  byte one = ((valeur >> 8) & 0xFF);
  
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}
 
int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}

long EEPROMReadLong(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWriteLong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

#endif
