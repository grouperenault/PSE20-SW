#ifndef PSE_lcdDisplay_h
    #define PSE_lcdDisplay_h

    #include "PSE_project.h"

    void initLCD(Etat_Global &etatLocal);
    void cleanLCD();
    void affichageInidicateurPerf(Etat_Global &etatLocal);
    void textLCD(String text,int x, int y);
    void splashLCD();
    void volumeLCD(Volume volLocal);
    void menuLCD(Etat_Global &etatLocal);
    void affichageAlarme(Etat_Global &etatLocal);
    void bolusLCD(Etat_Global &etatLocal);

    void enregistreBytes();
    void writeZero(int pos);
    void writeUn(int pos);
    void writeDeux(int pos);
    void writeTrois(int pos);
    void writeQuatre(int pos);
    void writeCinq(int pos);
    void writeSix(int pos);
    void writeSept(int pos);
    void writeHuit(int pos);
    void writeNeuf(int pos);

#endif
