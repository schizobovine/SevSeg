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

// If you use current-limiting resistors on your segment pins instead of the
// digit pins, then change the '0' in the line below to a '1'
#define RESISTORS_ON_SEGMENTS 0
#define MAXNUMDIGITS 8 //Increase this number to support larger displays


#ifndef SevSeg_h
#define SevSeg_h

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

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#error "Archietecture unrecognized, how I PROGMEM?"
#endif

#define BLANK 16 // Must match with 'digitCodeMap'
#define DASH  17

const byte digitCodeMap[] PROGMEM = {
 //Pgfedcba <-segments on
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111, // 9
  B01110111, // A
  B00000000, // b
  B00000000, // c
  B00000000, // d
  B00000000, // E
  B00000000, // F
  B00000000, // BLANK
  B01000000, // DASH
};

const long powersOf10[] PROGMEM = {
  1,          // 10^0
  10,         // 10^1
  100,        // 10^2
  1000,       // 10^3
  10000,      // 10^4
  100000,     // 10^5
  1000000,    // 10^6
  10000000,   // 10^7
  100000000,  // 10^8
  1000000000, // 10^9
};

class SevSeg
{
public:
  SevSeg();

  void refreshDisplay();
  void refreshDisplay(int micros);
  void begin(byte hardwareConfig, byte numDigitsIn, const byte digitPinsIn[], const byte segmentPinsIn[]);
  void setBrightness(int brightnessIn); // A number from 0..100

  void setNumber(long numToShow, byte decPlaces);
  void setNumber(unsigned long numToShow, byte decPlaces);
  void setNumber(int numToShow, byte decPlaces);
  void setNumber(unsigned int numToShow, byte decPlaces);
  void setNumber(char numToShow, byte decPlaces);
  void setNumber(byte numToShow, byte decPlaces);
  void setNumber(float numToShow, byte decPlaces);

private:
  void setNewNum(long numToShow, byte decPlaces);
  void findDigits(long numToShow, byte decPlaces, byte nums[]);
  void setDigitCodes(byte nums[], byte decPlaces);

  boolean digitOn,digitOff,segmentOn,segmentOff;
  byte digitPins[MAXNUMDIGITS];
  byte segmentPins[8];
  byte numDigits;
  byte digitCodes[MAXNUMDIGITS];
  int ledOnTime;
  //const static long powersOf10[10];

};

#endif //SevSeg_h
/// END ///
