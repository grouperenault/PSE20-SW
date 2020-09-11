// **********************************************************************************
// PSE project, caractères management
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

#ifndef PSE_lcdDisplay_cpp
    #define PSE_lcdDisplay_cpp
    
    #include "PSE_lcdDisplay.h"
    #include "./PSE_struct.h"

// select the pins used on the LCD panel
//  LiquidCrystal_I2C lcd(0x38, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
 LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

  /* 
   * Gestion de l'indicateur d'affichage en cours
   * Affichage en alternance du x et o qui alter pour montrer une activité de perfusion
   */
void affichageInidicateurPerf(Etat_Global &etatLocal){
  if (etatLocal.etatNEMA == HIGH) {
    // Gestion de l'alternance
    if (millis() - etatLocal.lastCarMot > altIndicateurPerf) {
      etatLocal.lastCarMot = millis();
      if (etatLocal.carMot == 0) {etatLocal.carMot=1;}
      else {etatLocal.carMot=0;}
    }
    // Gestion de l'affichage
    if (etatLocal.carMot == 0) {textLCD(F("x"),0,0);}
    else {textLCD(F("o"),0,0);}
  }
}


// Initialisation de la communication avec l'écran
void initLCD(Etat_Global &etatLocal){
  LiquidCrystal_I2C lcd_temp(etatLocal.adresseLCD, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
  lcd = lcd_temp;

  lcd.begin(16,2);
  lcd.clear();
}


// Reprise de la fonction classique de clean de l'écran
void cleanLCD() {
  lcd.clear();
}

// Reprise de la fonction classique d'affichage du texte en joignant le setcursor et le print
void textLCD(String text,byte x, byte y) {
  if (y < 2) { 
    lcd.setCursor(x, y);
    lcd.print(text);
    lcd.noBlink();       // ne pas clignoter
  }
}

// Initialisation des éléments pour faire le logo Renault
void initEltLogo() {
  byte slash[8] = {B00001,B00010,B00010,B00100,B00100,B01000,B01000,B10000,};
  byte antis[8] = {B10000,B01000,B01000,B00100,B00100,B00010,B00010,B00001,};
 
  lcd.createChar(0, slash);
  lcd.createChar(1, antis);
}

// Affichage du logo en position "pos"
void dessinLogo(byte pos) {
  lcd.setCursor(pos, 0); lcd.write(byte(0));
  lcd.setCursor(pos+1, 1); lcd.write(byte(0));
  lcd.setCursor(pos+1, 0); lcd.write(byte(1));
  lcd.setCursor(pos, 1); lcd.write(byte(1));
}

// Ecran de démarrage du PSE
void splashLCD() {
  cleanLCD();
  textLCD(F("    ElecSyr     "),0,0);
  textLCD(F(PSE_VERSION),4,1);
  
  initEltLogo();
  dessinLogo(1);
  dessinLogo(13);
}

// Fonction d'affichage du menu avec l'indicateur sur la ligne sélectionnée
void menuLCD(Etat_Global &etatLocal) {
    // si aucune alarme n'est affiché, on peut afficher autre chose ... l'alarme est prioritaire
    if (etatLocal.displayAlarme == alarmNone) {
      cleanLCD();

      // Menu principal
      if (etatLocal.affichageEnCours == menuPrincipal) {
        // textLCD(F("Seringue     mm"),1,0);
        textLCD(F("Sering.        "),1,0);
        textLCD(F("Debit      ml/h"),1,1);
      
        // Affichage du diametre de la seringue sur le menu principal
        // textLCD(String((etatLocal.seringueDiam[etatLocal.seringueSel] * 0.1),1),10,0);
        textLCD(etatLocal.seringueShort[etatLocal.seringueSel],9,0);
        

        // Affichage du débit sur le menu principal
        if (etatLocal.debitEnCours.volGlobale < 100) {
          textLCD(F("  "),7,1);
          textLCD(String((etatLocal.debitEnCours.volGlobale * 0.1), 1),9,1);
        } else if (etatLocal.debitEnCours.volGlobale < 1000) {
          textLCD(F(" "),8,1);
          textLCD(String((etatLocal.debitEnCours.volGlobale * 0.1), 1),8,1);
        } else {
          textLCD(String((etatLocal.debitEnCours.volGlobale * 0.1), 1),7,1);
        }
    
        if (etatLocal.choixEnCours == choixSeringue) { textLCD(F(">"),0, 0);}
        if (etatLocal.choixEnCours == choixDebit) { textLCD(F(">"),0, 1);}
      } else {

        // Sélection de la seringue
        if (etatLocal.affichageEnCours == reglSeringue) {
          textLCD(F("Select Seringue"),0,0);
          textLCD(F(">"),0,1);
      
          textLCD(etatLocal.seringueList[etatLocal.positionSer],1,1);
          lcd.setCursor(1, 1);
          lcd.blink();       // et le fait clignoter
        } else {     
      
          // Réglage du débit
          if (etatLocal.affichageEnCours == reglDebit) {
            textLCD(F("Reglage Debit"),1,0);
            textLCD(F("       ml/h"),1,1);
      
            textLCD(String(etatLocal.debitEnCours.volCentaine),2,1);
            textLCD(String(etatLocal.debitEnCours.volDixaine),3,1);
            textLCD(String(etatLocal.debitEnCours.volUnite),4,1);
            textLCD(F(","),5,1);
            textLCD(String(etatLocal.debitEnCours.volDixieme),6,1);

            // Définition du curseur
            if (etatLocal.positionVol != 3) { lcd.setCursor(2 + etatLocal.positionVol, 1); } 
            else { lcd.setCursor(6, 1);}
            
            lcd.blink();       // et le fait clignoter
          } else {

            // Affichage du message de confirmation de avance rapide
            if ((etatLocal.affichageEnCours == confAvanceRapide) || (etatLocal.affichageEnCours == confRetourRapide)) {
              textLCD(F("Nouvel appui"),1,0);
              textLCD(F("pour confirmer"),1,1);
            } else {
              
            // Affichage du message d'avance rapide
            if (etatLocal.affichageEnCours == avanceEnCours) {
                textLCD(F("Avance rapide"),1,0);
            } else {
      
              // Affichage du message de perfusion 
              if (etatLocal.affichageEnCours == perfEnCours) {
                  volumeLCD(etatLocal.debitEnCours);
              } else {
                
                // Affichage du message de perfusion 
                if (etatLocal.affichageEnCours == confBolus) {
                  textLCD(F("Nouvel appui"),1,0);
                  textLCD(F("pour confirmer"),1,1);
                } else {

                  // Affichage du message de perfusion 
                  if (etatLocal.affichageEnCours == bolusEnCours) {
                    textLCD(F("Bolus en cours"),1,0);
                  } else {

                    // Affichage du message de retour chariot
                    if (etatLocal.affichageEnCours == retourEnCours) {
                      textLCD(F("Retour chariot"),1,0);
                      textLCD(F("en cours"),1,1);
                    } else {

                      // Affichage du message d'erreur : impossible dans ce mode (avance, retour et bolus)
                      if (etatLocal.affichageEnCours == erreurMessage) {
                        textLCD(F("Fct impossible"),1,0);
                        textLCD(F(" dans ce mode"),1,1);
                      } else {

                        // Affichage du message d'erreur : impossible dans ce mode (avance, retour et bolus)
                        if (etatLocal.affichageEnCours == valDebOblig) {
                          textLCD(F("Valid. debit"),1,0);
                          textLCD(F(" obligatoire"),1,1);
                        } else {

                          // Affichage du message d'erreur : impossible dans ce mode (avance, retour et bolus)
                          if (etatLocal.affichageEnCours == valSerOblig) {
                            textLCD(F("Valid. seringue"),1,0);
                            textLCD(F(" obligatoire"),1,1);
                          } else {
                            if (etatLocal.affichageEnCours == displayAccostage) {
                              textLCD(F("Accostage "),1,0);
                              textLCD(F("seringue ok"),1,1);

                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            }
          }
        }
      }
    }
}

void bolusLCD(Etat_Global &etatLocal) {
  
    // Affichage de la valeur si la valeur à changé
    if (etatLocal.prevVolBolus != etatLocal.volBolus) {
      textLCD(F("volume       ml"),1,1);
      if (etatLocal.volBolus < 100) {textLCD(String(etatLocal.volBolus  * 0.1,1),9,1);}
      else if (etatLocal.volBolus < 1000) {textLCD(String(etatLocal.volBolus  * 0.1,1),8,1);}
      else if (etatLocal.volBolus < 10000) {textLCD(String(etatLocal.volBolus  * 0.1,1),7,1);}
    }

    // Changement entête si le volume est au maximum
    if (etatLocal.alarmeFinBolus == HIGH) {
      textLCD(F("Arret bolus Max"),1,0);
    }
}


// Fonction gérant l'affichage du message d'alarme en fonction des priorités
void affichageAlarme(Etat_Global &etatLocal) {
  if (existeAlarme(etatLocal)) {
    // Aucune alarme affichée ou à afficher donc on sort de cette boucle
    if (etatLocal.displayAlarme == alarmNone) {return;}

    if (millis() - etatLocal.alarmeLastDisplay > timerBoucleAlarme ) {
      String messageAlarme = "               "; // Texte à afficher

      if (etatLocal.alarmeOcclusion && (etatLocal.displayAlarme == alarmOcclusion || etatLocal.displayAlarme == alarmRestart)) {
        messageAlarme = "OCCLUSION      ";
      } else if (etatLocal.alarmeBatterie && (etatLocal.displayAlarme == alarmBatterie || etatLocal.displayAlarme == alarmRestart)) {
        messageAlarme = "BATTERIE FAIBLE ";
      } else if (etatLocal.alarmeSecteur && (etatLocal.displayAlarme == alarmSecteur || etatLocal.displayAlarme == alarmRestart)) {
        messageAlarme = "SECTEUR OFF    ";
      } else if (etatLocal.alarmeFinCourse && (etatLocal.displayAlarme == alarmFinCourse || etatLocal.displayAlarme == alarmRestart)) {
        messageAlarme = "FIN SERINGUE   ";
      } else if (etatLocal.alarmeDixPourCent && (etatLocal.displayAlarme == alarmDixPourCent || etatLocal.displayAlarme == alarmRestart)) {
        messageAlarme = "PRE-FIN        ";
      } 


      // Affichage du message d'alarme s'il a changé
      if (messageAlarme != "               " && etatLocal.displayAlarme != etatLocal.lastDisplayAlarme) {
         etatLocal.lastDisplayAlarme = etatLocal.displayAlarme ;
        cleanLCD();
        textLCD(F("ALARME         "),1,0);
        textLCD(messageAlarme,1,1);
        etatLocal.alarmeLastDisplay = millis();
      }
        
      // Calcul du message d'alarme suivant a afficher au prochain passage
      if (etatLocal.displayAlarme == alarmOcclusion) {etatLocal.displayAlarme = alarmBatterie;}
      else if (etatLocal.displayAlarme == alarmBatterie) {etatLocal.displayAlarme = alarmSecteur;}
      else if (etatLocal.displayAlarme == alarmSecteur) {etatLocal.displayAlarme = alarmFinCourse;}
      else if (etatLocal.displayAlarme == alarmFinCourse) {etatLocal.displayAlarme = alarmDixPourCent;}
      else if (etatLocal.displayAlarme == alarmDixPourCent) {etatLocal.displayAlarme = alarmOcclusion;}
      
    }
  } else if (etatLocal.displayAlarme != alarmNone) {
    etatLocal.displayAlarme = alarmNone;
    menuLCD(etatLocal);
  }
}

// Affichage du volume (4 chiffres) sur deux lignes
void volumeLCD(Volume volLocal) {
  enregistreBytes();
  byte positionCar = 1;

  cleanLCD();
  // affichage de la virgule :-)
  textLCD(F(","),12, 1);

  switch(volLocal.volCentaine) {
    case 0 : // pas d'affichage du 0 quand au début pour alléger l'affichage
      break; 
    case 1 :
      writeUn(positionCar);
      break;
    case 2 :
      writeDeux(positionCar);
      break;
    case 3 :
      writeTrois(positionCar);
      break;
    case 4 :
      writeQuatre(positionCar);
      break;
    case 5 :
      writeCinq(positionCar);
      break;
    case 6 :
      writeSix(positionCar);
      break;
    case 7 :
      writeSept(positionCar);
      break;
    case 8 :
      writeHuit(positionCar);
      break;
    case 9 :
      writeNeuf(positionCar);
      break;
  }

  positionCar = positionCar +4;
  switch(volLocal.volDixaine) {
    case 0 : // pas d'affichage du 0 quand au début pour alléger l'affichage
      if (volLocal.volCentaine > 0) {writeZero(positionCar);}
      break; 
    case 1 :
      writeUn(positionCar);
      break;
    case 2 :
      writeDeux(positionCar);
      break;
    case 3 :
      writeTrois(positionCar);
      break;
    case 4 :
      writeQuatre(positionCar);
      break;
    case 5 :
      writeCinq(positionCar);
      break;
    case 6 :
      writeSix(positionCar);
      break;
    case 7 :
      writeSept(positionCar);
      break;
    case 8 :
      writeHuit(positionCar);
      break;
    case 9 :
      writeNeuf(positionCar);
      break;
  }

  positionCar = positionCar +4;
  switch(volLocal.volUnite) {
    case 0 : 
      writeZero(positionCar);
      break; 
    case 1 :
      writeUn(positionCar);
      break;
    case 2 :
      writeDeux(positionCar);
      break;
    case 3 :
      writeTrois(positionCar);
      break;
    case 4 :
      writeQuatre(positionCar);
      break;
    case 5 :
      writeCinq(positionCar);
      break;
    case 6 :
      writeSix(positionCar);
      break;
    case 7 :
      writeSept(positionCar);
      break;
    case 8 :
      writeHuit(positionCar);
      break;
    case 9 :
      writeNeuf(positionCar);
      break;
  }

  positionCar = positionCar +4;
  switch(volLocal.volDixieme) {
    case 0 :
      writeZero(positionCar);
      break; 
    case 1 :
      writeUn(positionCar);
      break;
    case 2 :
      writeDeux(positionCar);
      break;
    case 3 :
      writeTrois(positionCar);
      break;
    case 4 :
      writeQuatre(positionCar);
      break;
    case 5 :
      writeCinq(positionCar);
      break;
    case 6 :
      writeSix(positionCar);
      break;
    case 7 :
      writeSept(positionCar);
      break;
    case 8 :
      writeHuit(positionCar);
      break;
    case 9 :
      writeNeuf(positionCar);
      break;
  }


}

// Affichage sur deux lignes des caractères dessinés dans les tableaux de bytes param1 et param2
void enregistreBytes() {
// Element d'affichage
  byte char1[8] = {B11111,B11111,B11111,B11111,B11111,B01111,B00111,};
  byte char2[8] = {B00111,B01111,B11111,B11111,B11111,B11111,B11111,};
  byte char3[8] = {B11111,B11111,B00000,B00000,B00000,B00000,B00000,};
  byte char4[8] = {B00000,B00000,B00000,B00000,B00000,B11111,B11111,};
  byte char5[8] = {B11100,B11110,B11111,B11111,B11111,B11111,B11111,};
  byte char6[8] = {B11111,B11111,B11111,B11111,B11111,B11110,B11100,};
  byte char7[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,};
  byte char8[8] = {B11111,B11111,B00000,B00000,B00000,B11111,B11111,};
  
  lcd.createChar(0,char1);
  lcd.createChar(1,char2);
  lcd.createChar(2,char3);
  lcd.createChar(3,char4);
  lcd.createChar(4,char5);
  lcd.createChar(5,char6);
  lcd.createChar(6,char7);
  lcd.createChar(7,char8);
}

/*
 * Liste des chiffres pour l'affichage sur deux lignes des chiffres du volume
 */
 
// Affichage du caractère "0" en position "pos"
void writeZero(byte pos) { 
  /* Caractère 0 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(1)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(2)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos,1);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(5)); 
}

// Affichage du caractère "1" en position "pos"
void writeUn(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(2)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(4)); 
  lcd.setCursor(pos,1);   lcd.write(byte(3)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(6)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(3)); 
}

// Affichage du caractère "2" en position "pos"
void writeDeux(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(7)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos,1);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(3)); 
}

// Affichage du caractère "3" en position "pos"
void writeTrois(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(7)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos,1);   lcd.write(byte(3)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(5)); 
}

// Affichage du caractère "4" en position "pos"
void writeQuatre(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(6)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(6)); 
}

// Affichage du caractère "5" en position "pos"
void writeCinq(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(7)); 
  lcd.setCursor(pos,1);   lcd.write(byte(3)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(5)); 
}

// Affichage du caractère "6" en position "pos"
void writeSix(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(1)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(7)); 
  lcd.setCursor(pos,1);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(5)); 
}

// Affichage du caractère "7" en position "pos"
void writeSept(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(2)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(2)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(6)); 
}

// Affichage du caractère "8" en position "pos"
void writeHuit(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(1)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos,1);   lcd.write(byte(0)); 
  lcd.setCursor(pos+1,1); lcd.write(byte(3)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(5)); 
}

// Affichage du caractère "9" en position "pos"
void writeNeuf(byte pos) { 
  /* Caractère 1 sur deux lignes */
  lcd.setCursor(pos,0);   lcd.write(byte(1)); 
  lcd.setCursor(pos+1,0); lcd.write(byte(7)); 
  lcd.setCursor(pos+2,0); lcd.write(byte(4)); 
  lcd.setCursor(pos+2,1); lcd.write(byte(6)); 
}

#endif
