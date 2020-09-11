// **********************************************************************************
// PSE project, configure management
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
#ifndef PSE_configure_cpp
    #define PSE_configure_cpp
    
#include "PSE_configure.h"
#include "./PSE_project.h"


// Fonction principale qui décide si une configuration est nécessaire ou non
void managementConfig(Etat_Global &etatLocal) {
    // Debugln(F("managementConfig - start"));

    // Check si combinaison de touche pour lancer la configuration
    boolean confForcee = false;
    unsigned long timeConfForcee = millis();
    etatLocal.lastReadAnalog = millis();

    //On regarde si appuie d'un bouton
    while (millis() - timeConfForcee < 2000 && confForcee == false)
    {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {

            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnBolus, etatLocal); 

            // si appuie sur le bouton
            if (etatLocal.btnBolus.valeur == HIGH) {
                confForcee = true;
                digitalWrite(etatLocal.ledOcclusion.pinPhysique, HIGH);
                // petite pause pour laisser à l'opérateur le temps de voir l'extinction de la LED :-)
                timeConfForcee = millis();
                while (millis() - timeConfForcee < 2000) {}
            }
            etatLocal.lastReadAnalog = millis();
        }
    }

    // vérification des paramètres pour lancer une campagne de configuration
    if (confForcee == true || (etatLocal.adresseLCD == 0)) {
        // Debugln(F("managementConfig - debut conf"));

        configureLCD(etatLocal);
        sauvegardeAdresseLCD(etatLocal);

        // initialisation du LCD
        initLCD(etatLocal);

        verifADS(etatLocal); // Message d'erreur si ADS absent
        verifFX29(etatLocal); // Message d'erreur si capteur de pression absent

        etatLocal.volumeBolus = menuConfiguationLong("Vitesse Bolus",4,etatLocal.volumeBolus, etatLocal);
        if (etatLocal.volumeBolus == -1) {etatLocal.volumeBolus = 1500;}
        sauvegardeVolumeBolus(etatLocal);

        etatLocal.volumeBolusMax = menuConfiguationLong("Vol. Bolus max",2,etatLocal.volumeBolusMax, etatLocal);  
        if (etatLocal.volumeBolusMax == -1) {etatLocal.volumeBolusMax = 50;}
        sauvegardeVolumeBolusMax(etatLocal);

        etatLocal.volumeAvance =  menuConfiguationLong("Vitesse Avance",6,etatLocal.volumeAvance, etatLocal);
        if (etatLocal.volumeAvance == -1) {etatLocal.volumeAvance = 50000;}
        sauvegardeVolumeAvance(etatLocal);

        etatLocal.volumeRecul =  menuConfiguationLong("Vitesse Recul",6,etatLocal.volumeRecul, etatLocal);
        if (etatLocal.volumeRecul == -1) {etatLocal.volumeRecul = 90000;}
        sauvegardeVolumeRecul(etatLocal);

        etatLocal.etatBatAfterReboot =  menuConfiguationLong("Test batterie",1,etatLocal.etatBatAfterReboot, etatLocal);
        if (etatLocal.etatBatAfterReboot > uncheckBat) {etatLocal.etatBatAfterReboot = LOW;}
        sauvegardeEtatBatAfterReboot(etatLocal);

        initialisationPositionChariot(etatLocal);
        sauvegardePositionChariot(etatLocal);

        // On force le reboot après la configuration
        wdt_enable(WDTO_1S);
        while(1) {  }
    }
    
    // On remet la LED prête a fonctionner normalement
    digitalWrite(etatLocal.ledOcclusion.pinPhysique, LOW);

}

// Mise en place de la configuration des positions chariots
void initialisationPositionChariot(Etat_Global &etatLocal) {
    byte reponse = 1;
    cleanLCD();
    textLCD(F("Config position"),1,0);    
    textLCD(F(">Oui  -  Non   "),1,1);    
    
    // Evite un passage à l'étape suivante sans le vouloir
    initValid(etatLocal);

    while(etatLocal.btnValid.valeur == LOW) {  
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnUp, etatLocal); 
            readAnalogInput(etatLocal.btnDown, etatLocal); 
            readAnalogInput(etatLocal.btnValid, etatLocal); 

            if (((etatLocal.btnUp.valeur != etatLocal.btnUp.prevValeur) && (etatLocal.btnUp.valeur == HIGH)) || 
                ((etatLocal.btnDown.valeur != etatLocal.btnDown.prevValeur) && (etatLocal.btnDown.valeur == HIGH))) {
                if (reponse == 0) {
                    textLCD(F(">Oui  -  Non"),1,1);    
                    reponse = 1;
                } else {
                    textLCD(F(" Oui  - >Non"),1,1);    
                    reponse = 0;
                }
            }
        }
    }

    if (reponse == 0) { return;}

    // Evite un passage à l'étape suivante sans le vouloir
    initValid(etatLocal);

    cleanLCD();
    textLCD(F("Sering. interdit"),0,0);    
    textLCD(F("Valid pour debut"),0,1);    

    // Attente valid pour démarrer.
    while (etatLocal.btnValid.valeur == LOW)  {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnValid, etatLocal);
            readAnalogInput(etatLocal.finDistrib, etatLocal); 
        }
    }

    cleanLCD();
    textLCD(F("--- En cours ---"),0,0);    
    textLCD(F("<== butee gauche"),0,1);    

    // Avance chariot
    etatLocal.etatNEMA = HIGH;
    NEMA17.Ctrl_Moteur(AVANCE, 50000, 250);

    // Attente capteur fin de perfusion pour arrêter le moteur
    while (etatLocal.finDistrib.valeur == LOW) {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.finDistrib, etatLocal); 
            readAnalogInput(etatLocal.finRetour, etatLocal); 
            readAnalogInput(etatLocal.dixPourCent, etatLocal); 
            readAnalogInput(etatLocal.btnValid, etatLocal); 
        }
    }

    // Arret du moteur
    NEMA17.Motor_STOP();

    // Mise à 0 du compteur de pas
    NEMA17.InitPosition(0);
//    Debug("position fin course");
//    Debugln(NEMA17.getPosition());

    textLCD(F("butee droite ==>"),0,1);    

    // Recul chariot jusqu'au capteur arrière
    etatLocal.etatNEMA = HIGH;
    NEMA17.Ctrl_Moteur(RECUL, 50000, 250);

    // Attente capteur fin de la butée arrière pour arrêter le moteur
    while (etatLocal.finRetour.valeur == LOW) {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.finRetour, etatLocal); 
            readAnalogInput(etatLocal.dixPourCent, etatLocal); 
            readAnalogInput(etatLocal.btnValid, etatLocal); 
        }
    }

    // Arret du moteur
    NEMA17.Motor_STOP();

    // récupération de la position de la butée arrière
    etatLocal.positionFin = NEMA17.getPosition();
//    Debug("position arrière");
//    Debugln(NEMA17.getPosition());

    textLCD(F("<== butee milieu"),0,1);    

    // Avance chariot jusqu'à la position du capteur 10%
    etatLocal.etatNEMA = HIGH;
    NEMA17.Ctrl_Moteur(AVANCE, 50000, 250);

    // Attente capteur 10% pour arrêter le moteur
    while (etatLocal.dixPourCent.valeur == LOW) {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.dixPourCent, etatLocal); 
            readAnalogInput(etatLocal.btnValid, etatLocal); 
        }
    }

    // Arret du moteur
    NEMA17.Motor_STOP();

    // récupération de la position de la butée arrière
    etatLocal.position10 = NEMA17.getPosition();
//    Debug("position milieu");
//    Debugln(NEMA17.getPosition());

    cleanLCD();
    textLCD(F("Config. terminee"),0,0);    
    textLCD(F("Valid pour debut"),0,1);    

    // Attente valid pour continuer.
    while (etatLocal.btnValid.valeur == LOW)  {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnValid, etatLocal);
        }
    }
}

// Configuration du LCD par affichage de message dessus
void configureLCD(Etat_Global &etatLocal) {
    // Debugln(F("managementConfig - configure LCD"));

    Wire.begin();
    byte error, address;
    unsigned long attendreValidation = 0;
    boolean valid = false;
    etatLocal.adresseLCD = 0;
    int nbBoucle = 0;

    while (etatLocal.adresseLCD == 0 && nbBoucle < 5) {
        nbBoucle ++;
        for(address = 1; address < 127; address++ ) 
        {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();

            if (error == 0 && address != 72 && address != 40) // on traite différement l'ADS
            {
                LiquidCrystal_I2C lcd_temp(address, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
                lcd_temp.begin(16,2);
                lcd_temp.clear();

                lcd_temp.setCursor(1, 0); lcd_temp.print("Config ecran");
                lcd_temp.setCursor(1, 1); lcd_temp.print("bouton valider");

                // Attente 5s de validation de l'utilisateur
                attendreValidation = millis();
                valid = false;
                while (valid == false && millis() - attendreValidation < 10000) {
                    readAnalogInputs(etatLocal);
                    readAnalogInput(etatLocal.btnValid, etatLocal);

                    if (etatLocal.btnValid.valeur == HIGH) {
                        valid = true;
                        if (etatLocal.adresseLCD != address) {etatLocal.adresseLCD = address;}
                    }
                }
                lcd_temp.clear();
            }
        }
    }
}

// Vérifie si l'ADS est présent ou non ... message technique
void verifADS(Etat_Global &etatLocal) {
    // Debugln(F("managementConfig - test ADS"));

    Wire.begin();
    byte error ;
    boolean valid = false;
    int nbBoucle = 0;

    Wire.beginTransmission(72); // adresse de l'ADS
    error = Wire.endTransmission();

    cleanLCD();
    // affichage du Logo
    initEltLogo();
    dessinLogo(0);
    textLCD(F("bouton valider"),2,1);
 
    if (error == 0) { textLCD(F("Verif ADS: OK"),2,0); } 
    else { textLCD(F("Verif ADS: KO"),2,0); }
    
    // Evite un passage à l'étape suivante sans le vouloir
    initValid(etatLocal);

    // Attendre que l'utilisateur valide
    while (valid == false) {
        readAnalogInputs(etatLocal);
        readAnalogInput(etatLocal.btnValid, etatLocal);

        if (etatLocal.btnValid.valeur == HIGH) {
            valid = true;
        }
    }
    cleanLCD();
}

// Vérifie si le FX29 est présent ou non ... message technique
void verifFX29(Etat_Global &etatLocal) {
    // Debugln(F("managementConfig - test FX29"));

    Wire.begin();
    byte error ;
    boolean valid = false;
    int nbBoucle = 0;

    Wire.beginTransmission(40); // adresse de l'ADS
    error = Wire.endTransmission();

    cleanLCD();
    // affichage du Logo
    initEltLogo();
    dessinLogo(0);
    textLCD(F("bouton valider"),2,1);
 
    if (error == 0) { textLCD(F("Verif FX29: OK"),2,0); } 
    else { textLCD(F("Verif FX29: KO"),2,0); }
    
    // Evite un passage à l'étape suivante sans le vouloir
    initValid(etatLocal);

    // Attendre que l'utilisateur valide
    while (valid == false) {
        readAnalogInputs(etatLocal);
        readAnalogInput(etatLocal.btnValid, etatLocal);

        if (etatLocal.btnValid.valeur == HIGH) {
            valid = true;
        }
    }
    cleanLCD();
}


// fonction basique de menu de configuration
long menuConfiguationLong(String titre, byte nbrDigit,  long valeur, Etat_Global &etatLocal) {
    // Debug(F("managementConfig - "));
    // Debugln(titre);
    unsigned long attendreValidation = 0;

    // n'autorise pas les configurations de plus de 6 digits
    if (nbrDigit > 6) {return valeur;}
    // vitesse d'incrément qui évolue au cours du temps d'appuie
    long vitesse = 1;
    long maxValeur = puissanceDix(nbrDigit);
    unsigned long lastUpdate = millis();
    
    cleanLCD();
    // affichage du Logo
    initEltLogo();
    dessinLogo(0);
    // Affichage du titre
    textLCD(titre,2,0);

    // Affichage de la valeur
    textLCD(calculValeurString(valeur,nbrDigit),5,1);

    // Evite un passage à l'étape suivante sans le vouloir
    initValid(etatLocal);

    while(etatLocal.btnValid.valeur == LOW) {  
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnUp, etatLocal); 
            readAnalogInput(etatLocal.btnDown, etatLocal); 
            readAnalogInput(etatLocal.btnValid, etatLocal); 
            readAnalogInput(etatLocal.btnRecul, etatLocal); 

            if ((etatLocal.btnRecul.valeur != etatLocal.btnRecul.prevValeur) && (etatLocal.btnRecul.valeur == LOW)) {
                return -1;
            } else if (((etatLocal.btnUp.valeur != etatLocal.btnUp.prevValeur) && (etatLocal.btnUp.valeur == LOW)) ||
                ((etatLocal.btnDown.valeur != etatLocal.btnDown.prevValeur) && (etatLocal.btnDown.valeur == LOW))) {
                vitesse = 1;
            } else if (etatLocal.btnDown.valeur == HIGH && (millis() - lastUpdate > 300)) {
                // reduction de la valeur uniquement si le resultat est < maxValeur
                if (valeur + vitesse < maxValeur ) {valeur = valeur + vitesse;}
                else {valeur = maxValeur - 1;}
                // accélération de la vitesse
                if (vitesse != 500) {
                    if (vitesse > 99) { vitesse = vitesse + 100;}
                    else if (vitesse > 9) { vitesse = vitesse + 10;}
                    else { vitesse ++;} 
                }
                textLCD(calculValeurString(valeur,nbrDigit),5,1);
                lastUpdate = millis();
            } else if (etatLocal.btnUp.valeur == HIGH && (millis() - lastUpdate > 300)) {
                // reduction de la valeur uniquement si le resultat est > 1
                if (valeur > vitesse + 1) {valeur = valeur - vitesse;}
                else {valeur = 1;}
                // accélération de la vitesse
                if (vitesse != 500) {
                    if (vitesse > 99) { vitesse = vitesse + 100;}
                    else if (vitesse > 9) { vitesse = vitesse + 10;}
                    else { vitesse ++;} 
                }
                textLCD(calculValeurString(valeur,nbrDigit),5,1);
                lastUpdate = millis();
            }
            etatLocal.lastReadAnalog = millis();
        }
    }
    return valeur;

} 

// Fonction qui permet d'attendre le temps que le bouton valide soit relaché (pour éviter les actions en chaine)
void initValid(Etat_Global &etatLocal) {
    while (etatLocal.btnValid.valeur == HIGH)  {
        if (millis() - etatLocal.lastReadAnalog > freqChangementEtat) {
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnValid, etatLocal);
        }
    }
}


#endif
