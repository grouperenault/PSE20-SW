#ifndef PSE_eeprom_h
    #define PSE_eeprom_h

    #include "PSE_project.h"

    void sauvegardeSeringue(Etat_Global &etatLocal);
    void sauvegardeDebit(Etat_Global &etatLocal);    
    void sauvegardeAdresseLCD(Etat_Global &etatLocal);
    void sauvegardeVolumeBolus(Etat_Global &etatLocal);
    void sauvegardeVolumeBolus(Etat_Global &etatLocal);
    void sauvegardeVolumeAvance(Etat_Global &etatLocal);
    void sauvegardeVolumeRecul(Etat_Global &etatLocal);
    void sauvegardeVolumeBolusMax(Etat_Global &etatLocal);
    void sauvegardeEtatBatAfterReboot(Etat_Global &etatLocal);
    void sauvegardePositionChariot(Etat_Global &etatLocal);

    void restaureValeurs(Etat_Global &etatLocal);
    void EEPROMWriteInt(byte address, int value);
    int EEPROMReadInt(byte address);
    long EEPROMReadLong(byte address);
    void EEPROMWriteLong(byte address, long value);
#endif
