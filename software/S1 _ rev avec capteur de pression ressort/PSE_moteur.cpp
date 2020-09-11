// **********************************************************************************
// PSE project, moteor management
// **********************************************************************************
// The source code is protected by intellectual and/or industrial property rights.
// Copyright Renault S.A.S. 2020
//
// For any explanation about key project or use, contact me
// jimmy.jaumotte@renault.com
//
// Written by Stefan MARRY for Renault S.A.S. usage ONLY
//
// History : V1.00 2020-03-31 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef PSE_MOTEUR_cpp
#define PSE_MOTEUR_cpp

#include "PSE_moteur.h"
// #include "TimerOne.h" -> présent dans moteur.h
#include "PSE_project.h"

unsigned int compteur_step_moteur;
unsigned int volume_bolus;

unsigned int NBRE_PAS_POUR_0_1ML; // pas *2

Moteur_Stepper NEMA17;

void Moteur_Stepper::set_stepper_resolution(unsigned char Resolution)
{
  digitalWrite(Pin_MS1, Resolution & 0x04);
  digitalWrite(Pin_MS2, Resolution & 0x02); 
  digitalWrite(Pin_MS3, Resolution & 0x01);   
  digitalWrite(Pin_Reset, HIGH);
  for (int i = 0; i < 5; i++) { // pour être sur que l'action a été prise en compte
    digitalWrite(Pin_Reset, LOW);
  }
  digitalWrite(Pin_Reset, HIGH);
}

void Moteur_Stepper::Motor_STOP()
{
  Timer1.StopTimer();
  Timer1.disablePwm(Pin_Step);
//  Timer1.Disable_ISR_OC(Pin_Step);  => Plante l'arduino et declenche un watchdog
}

/*void Moteur_Stepper::Motor_ReStart()
{
 Timer1.Enable_ISR_OC(Pin_Step);  
 Timer1.RestartPwm(Pin_Step);
}*/

void Moteur_Stepper::Init_PinDriverMoteur()
{
  pinMode(Pin_Step, OUTPUT);
  pinMode(Pin_Dir, OUTPUT); 
  
  pinMode(Pin_MS1, OUTPUT);
  pinMode(Pin_MS2, OUTPUT);   
  pinMode(Pin_MS3, OUTPUT);   
  pinMode(Pin_Reset, OUTPUT);   
  set_stepper_resolution(SIXTEENTH_STEP);     // Configuration de MS1 / MS2 / MS3
}
/*
void Moteur_Stepper::InitVolumeBolus()
{
  volume_bolus = 0;
}
*/
int Moteur_Stepper::GetVolumeBolus()
{
  return volume_bolus;
}


void Moteur_ISR_Step()
{
  // 0.1 ml tous les 46 pas
  //  /!\ Déclenchement de 2 IT pour un pas

  if (compteur_step_moteur++ > (NBRE_PAS_POUR_0_1ML))
  {
    compteur_step_moteur = 0;
    volume_bolus++;  // Variable a mettre à 0 au lancement du bolus

#ifdef DEBUG_MOTEUR
//    digitalWrite (13, !digitalRead(13));
    DebugMoteur(" Volume ");
    DebugMoteurln(volume_bolus);
#endif    
  }
  else
  {
    /* Nothing to do */
  } 
}

void Moteur_Stepper::Ctrl_Moteur(bool Sens_Rotation, unsigned long Debit, unsigned int Diametre_Seringue)
{
//  set_stepper_resolution(SIXTEENTH_STEP);     // Configuration de MS1 / MS2 / MS3
  digitalWrite(Pin_Dir, Sens_Rotation);   // Configuration sens de rotation Avance / Recul
  
  // Configuration de l'Output Compare A du Timer1 pour générer les step
  Timer1.initialize_timer(   Conversion(Debit, Diametre_Seringue, 16),     TIMER_us); 
  volume_bolus = 0;
  
  Timer1.attach_OCA_Interrupt( Moteur_ISR_Step );
  Timer1.pwm(Pin_Step,512);
  Timer1.Enable_ISR_OC(Pin_Step);

#ifdef DEBUG_MOTEUR
  DebugMoteur(" Debit *10 ");
  DebugMoteurln(Debit);  
  DebugMoteur(" Diametre seringue *10 ");  
  DebugMoteurln(Diametre_Seringue);  
#endif
}



// Debit en ml/h *10
// Diametre en mm *10
long Moteur_Stepper::Conversion(unsigned long Debit, unsigned int Diametre, unsigned char NbreMicroPas)
{
  double Nbre_Cycles;
  double variable_intermediaire;
  
  variable_intermediaire = _PI_*Pas_vis*Diametre*Diametre;  
  Nbre_Cycles = 36*variable_intermediaire / (NbreMicroPas * Debit);
  
// calcul bolus
  NBRE_PAS_POUR_0_1ML = (2*12800000000)/variable_intermediaire;
  
#ifdef DEBUG_MOTEUR
  DebugMoteur(F("NBRE_PAS_POUR_0_1ML :")); 
  DebugMoteurln(NBRE_PAS_POUR_0_1ML); 
  DebugMoteurln(variable_intermediaire); 
 
  
  if (Nbre_Cycles > 0x3FFFC00)
  {
    DebugMoteur(F("overflow"));  
  }
#endif

  return (long)Nbre_Cycles; //0x3FF FC00

}



#endif
