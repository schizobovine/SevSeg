/* SevSeg Library
 
 Copyright 2014 Dean Reading
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 
 This library allows an Arduino to easily display numbers in decimal format on
 a 7-segment display without a separate 7-segment display controller.
 
 Direct any questions or suggestions to deanreading@hotmail.com
 See the included readme for instructions.
 */


#ifndef __SEVSEG_H
#define __SEVSEG_H

#define MAXNUMDIGITS 8 // Increase this number to support larger displays
                       // NB: other arrays will need redefined

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Use defines to link the hardware configurations to the correct numbers
#define COMMON_CATHODE 0
#define COMMON_ANODE 1
#define N_TRANSISTORS 2
#define P_TRANSISTORS 3
#define NP_COMMON_CATHODE 1
#define NP_COMMON_ANODE 0

// Must match with 'digitCodeMap'
#define BLANK 16
#define DASH  17

class SevSeg
{
public:
  SevSeg();

  void refreshDisplay();
  void refreshDisplay(int micros);
  void begin(byte hardwareConfig, byte numDigitsIn, const byte digitPinsIn[], const byte segmentPinsIn[]);
  void setBrightness(int brightnessIn); // A number from 0..100

  void setDigit(byte digitNum, byte value);
  void setDigits(byte values[], size_t num_values);

private:

  boolean digitOn,digitOff,segmentOn,segmentOff;
  byte segmentPins[8];
  byte numDigits;
  byte *digitPins;
  byte *digitCodes;
  int ledOnTime;

};

#endif //SevSeg_h
