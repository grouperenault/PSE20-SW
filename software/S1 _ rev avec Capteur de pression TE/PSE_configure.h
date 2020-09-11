#ifndef PSE_configure_h
    #define PSE_configure_h

    #include "PSE_project.h"

    void managementConfig(Etat_Global &etatLocal);
    void configureLCD(Etat_Global &etatLocal);
    void configureVolBolus(Etat_Global &etatLocal);
    void verifADS(Etat_Global &etatLocal);
    void verifFX29(Etat_Global &etatLocal);
    long menuConfiguationLong(String titre, byte nbrDigit, long valeur, Etat_Global &etatLocal);
    void initialisationPositionChariot(Etat_Global &etatLocal);
    void initValid(Etat_Global &etatLocal);
#endif
