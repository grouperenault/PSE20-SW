// **********************************************************************************
// PSE project, eeprom management
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

void sauvegardeEtatBatAfterReboot(Etat_Global &etatLocal){
  EEPROMWriteLong(28,etatLocal.etatBatAfterReboot);
}

void sauvegardePositionChariot(Etat_Global &etatLocal) {
  EEPROMWriteLong(32,etatLocal.position10);
  EEPROMWriteLong(36,etatLocal.positionFin);

}  

void restaureValeurs(Etat_Global &etatLocal){
  etatLocal.seringueSel              = EEPROMReadInt(0);   // 2 bytes
  etatLocal.debitEnCours.volCentaine = EEPROMReadInt(2);   // 2 bytes
  etatLocal.debitEnCours.volDixaine  = EEPROMReadInt(4);   // 2 bytes
  etatLocal.debitEnCours.volUnite    = EEPROMReadInt(6);   // 2 bytes
  etatLocal.debitEnCours.volDixieme  = EEPROMReadInt(8);   // 2 bytes
  etatLocal.adresseLCD               = EEPROMReadInt(10);  // 2 bytes
  etatLocal.volumeBolus              = EEPROMReadLong(12); // 4 bytes
  etatLocal.volumeAvance             = EEPROMReadLong(16); // 4 bytes
  etatLocal.volumeRecul              = EEPROMReadLong(20); // 4 bytes
  etatLocal.volumeBolusMax           = EEPROMReadLong(24); // 4 bytes
  etatLocal.etatBatAfterReboot       = EEPROMReadInt(28);  // 2 bytes
  etatLocal.position10               = EEPROMReadLong(32);  // 4 bytes
  etatLocal.positionFin              = EEPROMReadLong(36);  // 4 bytes

  // vérification si la valeur est possible
  if (etatLocal.seringueSel > etatLocal.seringueNbre-1) {etatLocal.seringueSel = 0;}
  if (etatLocal.debitEnCours.volCentaine > 9 || etatLocal.debitEnCours.volCentaine <0) {etatLocal.debitEnCours.volCentaine = 0;}
  if (etatLocal.debitEnCours.volDixaine > 9 || etatLocal.debitEnCours.volDixaine <0) {etatLocal.debitEnCours.volDixaine = 0;}
  if (etatLocal.debitEnCours.volUnite > 9 || etatLocal.debitEnCours.volUnite <0) {etatLocal.debitEnCours.volUnite = 0;}
  if (etatLocal.debitEnCours.volDixieme > 9 || etatLocal.debitEnCours.volDixieme <0) {etatLocal.debitEnCours.volDixieme = 0;}
  if (etatLocal.etatBatAfterReboot > 2) {etatLocal.etatBatAfterReboot = 1;}

  // récupération des paramètres
  if (etatLocal.adresseLCD > 127 || etatLocal.adresseLCD <0) {etatLocal.adresseLCD = 0;}
  if (etatLocal.volumeBolus > 10000 || etatLocal.volumeBolus <0) {etatLocal.volumeBolus = 1500;}
  if (etatLocal.volumeAvance > 1000000 || etatLocal.volumeAvance <0) {etatLocal.volumeAvance = 50000;}
  if (etatLocal.volumeRecul > 1000000 || etatLocal.volumeRecul <0) {etatLocal.volumeRecul = 90000;}
  if (etatLocal.volumeBolusMax > 100 || etatLocal.volumeBolusMax <0) {etatLocal.volumeBolusMax = 50;}


  // calcul de la valeur globale du débit
  etatLocal.debitEnCours.volGlobale = etatLocal.debitEnCours.volCentaine * 1000 + etatLocal.debitEnCours.volDixaine * 100 + etatLocal.debitEnCours.volUnite * 10 + etatLocal.debitEnCours.volDixieme;
  if (etatLocal.debitEnCours.volGlobale == 0) { // interdiction du volume 0 pour éviter la division par 0
    etatLocal.debitEnCours.volGlobale = 1;
    etatLocal.debitEnCours.volDixieme = 1;
  }

}

void EEPROMWriteInt(byte address, int valeur)
{
  byte two = (valeur & 0xFF);
  byte one = ((valeur >> 8) & 0xFF);
  
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}
 
int EEPROMReadInt(byte address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}

long EEPROMReadLong(byte address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWriteLong(byte address, long value) {
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
