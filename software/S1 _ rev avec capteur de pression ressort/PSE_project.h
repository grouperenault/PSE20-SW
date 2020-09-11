#ifndef PSE_PROJECT_H
  #define PSE_PROJECT_H

  // Version de l'application
  #define PSE_VERSION "V0.1.5"

  #define DEBUG true
  //#define DEBUG_MOTEUR true

  #include <Arduino.h>
  #include <avr/wdt.h>
  #include <LiquidCrystal_I2C.h>
  #include <Wire.h>
  #include <EEPROM.h>

  #include "PSE_define.h"
  #include "PSE_Struct.h"
  #include "PSE_lcdDisplay.h"
  #include "PSE_capteur.h"  
  #include "PSE_perfusion.h"  
  #include "PSE_alarme.h"  
  #include "PSE_eeprom.h"
  #include "PSE_TimerOne.h"
  #include "PSE_moteur.h"
  #include "PSE_configure.h"

  #define DEBUG_SERIAL Serial

  
  // I prefix debug macro to be sure to use specific for THIS library
  // debugging, this should not interfere with main sketch or other 
  // libraries
  #ifdef DEBUG
    #define Debug(x)    DEBUG_SERIAL.print(x)
    #define Debugln(x)  DEBUG_SERIAL.println(x)
    #define DebugF(x)   DEBUG_SERIAL.print(F(x))
    #define DebuglnF(x) DEBUG_SERIAL.println(F(x))
    #define Debugf(...) DEBUG_SERIAL.printf(__VA_ARGS__)
    #define Debugflush  DEBUG_SERIAL.flush
  #else
    #define Debug(x)    {}
    #define Debugln(x)  {}
    #define DebugF(x)   {}
    #define DebuglnF(x) {}
    #define Debugf(...) {}
    #define Debugflush  {}
  #endif

  #ifdef DEBUG_MOTEUR
    #define DebugMoteur(x)    DEBUG_SERIAL.print(x)
    #define DebugMoteurln(x)  DEBUG_SERIAL.println(x)
    #define DebugMoteurF(x)   DEBUG_SERIAL.print(F(x))
    #define DebugMoteurlnF(x) DEBUG_SERIAL.println(F(x))
    #define DebugMoteurf(...) DEBUG_SERIAL.printf(__VA_ARGS__)
    #define DebugMoteurflush  DEBUG_SERIAL.flush
  #else
    #define DebugMoteur(x)    {}
    #define DebugMoteurln(x)  {}
    #define DebugMoteurF(x)   {}
    #define DebugMoteurlnF(x) {}
    #define DebugMoteurf(...) {}
    #define DebugMoteurflush  {}
  #endif


#endif
