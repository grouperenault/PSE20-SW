// **********************************************************************************
// PSE project, fx29 management (presure measurement)
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

#ifndef PSE_fx29Mesure_cpp
    #define PSE_fx29Mesure_cpp
    
    #include "PSE_fx29Mesure.h"
    #include "./PSE_struct.h"

    float valeurPression[8]   = {0,0,0,0,0,0,0,0};
    const int valeurPressionNbre     = 8; 
    byte valeurPressionPos           = 0; 
    int preValueFX29     = 0;

    void initFX29() {
        Wire.begin();
    }

    void readFX29() {
     
        Wire.beginTransmission(adressFX29);
        Wire.write(0x51);

        int nbreBytes = 4;
        long bytesRead[nbreBytes];
    
        Wire.requestFrom(adressFX29, nbreBytes);   

        int cpt = 0;
        while(Wire.available())   
        {
            bytesRead[cpt] = Wire.read();
            cpt ++;
        }
      
        float Pscope,Pdisplay,Lmax=100,Lmin=0;
        uint32_t Pvalue,Pspan, I2C_ERR;
        uint16_t P1=1000,P2=15000;

        if((bytesRead[0]&0xc0)==0x00)
            {
                Pvalue=(bytesRead[0]<<8)   |   bytesRead[1];
                I2C_ERR=0;
            }
            else
                I2C_ERR=1;
                    
        if(I2C_ERR==0)
        {
            Pspan=P2-P1;
            Pdisplay=(Pvalue-1000)*(Lmax-Lmin)/Pspan;//+Lmin;//100L

            if (Pdisplay >=0 && Pdisplay<=100) {
                valeurPression[valeurPressionPos]   = Pdisplay;
                valeurPressionPos ++;
                if (valeurPressionPos == valeurPressionNbre) {valeurPressionPos = 0;}
            }
        }
        
        Wire.endTransmission();

    }

    float valeurFX29() {
        float sommeValue = 0;
        for (int i= 0; i < valeurPressionNbre; i++) {
            sommeValue = sommeValue + valeurPression[i];
        }
        return sommeValue / valeurPressionNbre; 
    }

    int statutFX29(int typo) {  
        if (valeurFX29() < pressionAccostage) { return vide; }
        else if (type == surpression) {
            if (valeurFX29() < pressionHaute) { return accostage; }
            else { return surpression; }
        } else if (type == reculPression) {
            if (valeurFX29() < pressionHauteRecul) { return accostage; }
            else { return surpression; }
        }
    }

    void changeFX29() {
        preValueFX29 = statutFX29();
    }

    boolean isChangedFX29(int typo) { // type surpression ou accostage pour connaitre l'entrée ou la sortie dans ce statut
        int newValueFX29 = statutFX29();
        if (typo == surpression) {
            if (newValueFX29 == surpression or preValueFX29 == surpression) {
                return preValueFX29 != newValueFX29;
            } else { return false;} // changement de statut sur un statut qui n'interresse pas le tracking dans ce cas
        } else if (typo == accostage) {
            if (newValueFX29 == accostage or preValueFX29 == accostage) {
                return preValueFX29 != newValueFX29;
            } else { return false;} // changement de statut sur un statut qui n'interresse pas le tracking dans ce cas
        }
        return false; // cas d'un mauvais paramètre dans le champ typo
    }

#endif


