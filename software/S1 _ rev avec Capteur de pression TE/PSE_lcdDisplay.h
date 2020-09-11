#ifndef PSE_lcdDisplay_h
    #define PSE_lcdDisplay_h

    #include "PSE_project.h"

    void initLCD(Etat_Global &etatLocal);
    void cleanLCD();
    void affichageInidicateurPerf(Etat_Global &etatLocal);
    void textLCD(String text,byte x, byte y);
    void splashLCD();
    void volumeLCD(Volume volLocal);
    void menuLCD(Etat_Global &etatLocal);
    void affichageAlarme(Etat_Global &etatLocal);
    void bolusLCD(Etat_Global &etatLocal);
    void initEltLogo();
    void dessinLogo(byte pos);

    void enregistreBytes();
    void writeZero(byte pos);
    void writeUn(byte pos);
    void writeDeux(byte pos);
    void writeTrois(byte pos);
    void writeQuatre(byte pos);
    void writeCinq(byte pos);
    void writeSix(byte pos);
    void writeSept(byte pos);
    void writeHuit(byte pos);
    void writeNeuf(byte pos);

#endif
