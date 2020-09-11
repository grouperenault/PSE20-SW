#ifndef PSE_fx29Mesure_h
    #define PSE_fx29Mesure_h

    #include "PSE_project.h"
    #define adressFX29 0x28

    void initFX29();
    void readFX29();
    float valeurFX29();
    int statutFX29(int typo = surpression);
    void changeFX29();
    boolean isChangedFX29(int typo = surpression);
#endif