// **********************************************************************************
// PSE project, configure management
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
#ifndef PSE_configure_cpp
    #define PSE_configure_cpp
    
#include "./PSE_project.h"
//#include "./PSE_eeprom.h"

// Fonction principale qui décide si une configuration est nécessaire ou non
void managementConfig(Etat_Global &etatLocal) {
    Debugln(F("managementConfig - start"));

    // Check si combinaison de touche pour lancer la configuration
    boolean confForcee = false;
    unsigned long timeConfForcee = millis();
    etatLocal.lastReadAnalog = millis();

    //On regarde si appuie d'un bouton
    while (millis() - timeConfForcee < 2000 && confForcee == false)
    {
        if (millis() - etatLocal.lastReadAnalog > 10) {

            // Lecture des entrées analogique + bouton bolus
            readAnalogInputs(etatLocal);
            readAnalogInput(etatLocal.btnBolus, etatLocal); 

            // si appuie sur le bouton
            if (etatLocal.btnBolus.valeur == HIGH) {
                Debugln(F("managementConfig - bouton bolus"));
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
        configureLCD(etatLocal);

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

        // On force le reboot après la configuration
        wdt_enable(WDTO_1S);
        while(1) {  }
    }
    
    // On remet la LED prête a fonctionner normalement
    digitalWrite(etatLocal.ledOcclusion.pinPhysique, LOW);

}

// Configuration du LCD par affichage de message dessus
void configureLCD(Etat_Global &etatLocal) {

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

            if (error == 0)
            {
                Debug("first i2c - adress : ");
                Debugln(address);
                
                // Test l'affichage d'un texte sur l'i2C trouvé
                LiquidCrystal_I2C lcd_temp(address, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
                lcd_temp.begin(16,2);
                lcd_temp.clear();
                lcd_temp.setCursor(0, 0); lcd_temp.print("Config ecran");
                lcd_temp.setCursor(0, 1); lcd_temp.print("bouton valider");

                // Attente 5s de validation de l'utilisateur
                attendreValidation = millis();
                valid = false;
                while (valid == false && (millis() - attendreValidation < 5000)) {
                    readAnalogInputs(etatLocal);
                    readAnalogInput(etatLocal.btnValid, etatLocal);
                    if (etatLocal.btnValid.valeur == HIGH) {
                        valid = true;
                        etatLocal.adresseLCD = address;
                    }
                }
                lcd_temp.clear();
            }
        }
    }

    // Alerte avec les LED si pas d'adresse I2C trouvée
    // On fait tout clignoter sans s'arrêter ... sans écran rien ne peut fonctionner
    if (nbBoucle == 5) {
        DebuglnF("Pas d'adresse i2C trouvée");
        etatLocal.ledFin.freqHaute    = 500;
        etatLocal.ledFin.freqBasse    = 500;
        etatLocal.ledFin.nbIteration  = -1;
        etatLocal.ledBatterie.freqHaute    = 500;
        etatLocal.ledBatterie.freqBasse    = 500;
        etatLocal.ledBatterie.nbIteration  = -1;
        etatLocal.ledPerfEnCours.freqHaute    = 500;
        etatLocal.ledPerfEnCours.freqBasse    = 500;
        etatLocal.ledPerfEnCours.nbIteration  = -1;
        etatLocal.ledSecteur.freqHaute    = 500;
        etatLocal.ledSecteur.freqBasse    = 500;
        etatLocal.ledSecteur.nbIteration  = -1;

        while (true)
        {
            lightManagement(etatLocal);
        }
        
    }

    // sauvegarde dans l'eeprom de l'adresse du LCD
    sauvegardeAdresseLCD(etatLocal);
}


// fonction basique de menu de configuration
long menuConfiguationLong(String titre, int nbrDigit,  long valeur, Etat_Global &etatLocal) {
    // n'autorise pas les configurations de plus de 6 digits
    if (nbrDigit > 6) {return valeur;}
    // vitesse d'incrément qui évolue au cours du temps d'appuie
    long vitesse = 1;
    long maxValeur = puissanceDix(nbrDigit);
    unsigned long lastUpdate = millis();
    
    cleanLCD();
    // Affichage du titre
    textLCD(titre,1,0);

    // Affichage de la valeur
    textLCD(calculValeurString(valeur,nbrDigit),5,1);

    // Lecture des entrées analogique + bouton bolus
    readAnalogInputs(etatLocal);
    readAnalogInput(etatLocal.btnValid, etatLocal); 

    while((etatLocal.btnValid.valeur == etatLocal.btnValid.prevValeur) || etatLocal.btnValid.valeur == LOW) {  
        if (millis() - etatLocal.lastReadAnalog > 10) {
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
                if (valeur < maxValeur) {valeur = valeur + vitesse;}
                if (vitesse != 500) {
                    if (vitesse > 99) { vitesse = vitesse + 100;}
                    else if (vitesse > 9) { vitesse = vitesse + 10;}
                    else { vitesse ++;} 
                }
                textLCD(calculValeurString(valeur,nbrDigit),5,1);
                lastUpdate = millis();
            } else if (etatLocal.btnUp.valeur == HIGH && (millis() - lastUpdate > 300)) {
                if (valeur != 0) {valeur = valeur - vitesse;}
                if (vitesse != 500) {
                    if (vitesse > 99) { vitesse = vitesse + 100;}
                    else if (vitesse > 9) { vitesse = vitesse + 10;}
                    else { vitesse ++;} 
                }
                textLCD(calculValeurString(valeur,nbrDigit),5,1);
                lastUpdate = millis();
            }

        }
    }
    return valeur;

} 

// calcule 10 puissance le paramètre
long puissanceDix(int valeur) {
    long result = 10;
    if (valeur == 0) {return 0;}

    for (int i = 1; i < valeur; i++) {
        result = result * 10;
    }
    return result;
}

// Affiche une valeur préfixée par des 0
String calculValeurString(long valeur,int nbrDigit) {
    long valeurTemp = puissanceDix(nbrDigit) + valeur;
    return String(valeurTemp).substring(1, nbrDigit + 1);
}
#endif
