#ifndef PSE_configure_h
    #define PSE_configure_h

    #include "PSE_project.h"

    void managementConfig(Etat_Global &etatLocal);
    void configureLCD(Etat_Global &etatLocal);
    void configureVolBolus(Etat_Global &etatLocal);
    long puissanceDix(int valeur);
    String calculValeurString(long valeur,int nbrDigit);

    long menuConfiguationLong(String titre, int nbrDigit,  long valeur, Etat_Global &etatLocal);
#endif
