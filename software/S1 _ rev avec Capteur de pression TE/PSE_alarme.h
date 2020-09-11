#ifndef PSE_alarme_h
    #define PSE_alarme_h

    #include "PSE_project.h"

    // Fonctions principales
    void alarmeManagement(Etat_Global &etatLocal);
    void buzzerMelodie(Etat_Global &etatLocal);

    // Fonctions gérants les IHMs
    void buzzerManagement(Etat_Global &etatLocal);
    void lightManagement(Etat_Global &etatLocal);
    void ledManagement(Alerte &alerteLocal);
    void checkBatterieDemarrage(Etat_Global &etatLocal);
    
#endif
