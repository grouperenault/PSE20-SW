#ifndef PSE_MOTEUR_h
  #define PSE_MOTEUR_h

  #include <avr/io.h>
  #include <avr/interrupt.h>
  
  #include "PSE_project.h"
  #include "PSE_TimerOne.h"

  #define FULL_STEP 0
  #define HALF_STEP 4
  #define QUARTER_STEP 2
  #define EIGHTH_STEP 1
  #define SIXTEENTH_STEP 7

  #define _PI_ 3.14L
  #define Pas_vis 125L // Pas * 100

class Moteur_Stepper
{
  private:
  void set_stepper_resolution(unsigned char Resolution);

  public:
  
    // properties

    // methods
    void Init_PinDriverMoteur(void);
    void Motor_STOP(void);
//    void Motor_ReStart(void);
    long Conversion(unsigned long Debit, unsigned int Diametre, unsigned char NbreMicroPas);
    void Ctrl_Moteur(bool _Sens_Rotation, unsigned long _Debit, unsigned int Diametre_Seringue);
//    void InitVolumeBolus(void);
    int GetVolumeBolus(void);
    void InitPosition(unsigned long valeur);
    int getTempsRestant();
    long getPosition();
};

extern Moteur_Stepper NEMA17;




#endif
