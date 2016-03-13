/*
 * SevSeg Library
 *
 * Copyright 2014 Dean Reading
 *
 * Limited modifications by Sean Caulfield
 *  
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 * This library allows an Arduino to easily display numbers in decimal format on
 * a 7-segment display without a separate 7-segment display controller.
 * 
 * Direct any questions or suggestions to deanreading@hotmail.com
 * See the included readme for instructions.
 * 
 * CHANGELOG
 * 
 * Version 3.0 - November 2014
 * Library re-design. A display with any number of digits can be used.
 * Floats are supported. Support for using transistors for switching.
 * Much more user friendly. No backwards compatibility.
 * Uploaded to GitHub to simplify any further development.
 * Version 2.3; Allows for brightness control.
 * Version 2.2; Allows 1, 2 or 3 digit displays to be used.
 * Version 2.1; Includes a bug fix.
 * Version 2.0; Now works for any digital pin arrangement.
 * Supports both common anode and common cathode displays.
 *
 */

#include "SevSeg.h"

const byte digitCodeMap[] = {
 //gfedcba <-segments on
  B0111111, // 0
  B0000110, // 1
  B1011011, // 2
  B1001111, // 3
  B1100110, // 4
  B1101101, // 5
  B1111101, // 6
  B0000111, // 7
  B1111111, // 8
  B1101111, // 9
  B1110111, // A
  B1111100, // b
  B0111001, // c
  B1011110, // d
  B1111001, // E
  B1110001, // F
  B0000000, // BLANK
  B1000000, // DASH
};

const long powersOf10[] = {
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

// SevSeg
/******************************************************************************/
SevSeg::SevSeg()
{
  // Initial value
  this->ledOnTime = 1; // Corresponds to a brightness of 0
  this->numDigits = 0;
}


// begin
/******************************************************************************/
// Saves the input pin numbers to the class and sets up the pins to be used.

void SevSeg::begin(byte hardwareConfig, byte numDigitsIn, 
                  const byte digitPinsIn[], const byte segmentPinsIn[]) {
                    
  this->numDigits = numDigitsIn;

  //Limit the max number of digits to prevent overflowing
  if (this->numDigits > MAXNUMDIGITS) this->numDigits = MAXNUMDIGITS;
  this->digitPins = new byte[numDigits];
  this->digitCodes = new byte[numDigits];

  switch (hardwareConfig){

  case 0: // Common cathode
    digitOn = LOW;
    segmentOn = HIGH;
    break;

  case 1: // Common anode
    digitOn = HIGH;
    segmentOn = LOW;
    break;

  case 2: // With active-high, low-side switches (most commonly N-type FETs)
    digitOn = HIGH;
    segmentOn = HIGH;
    break;

  case 3: // With active low, high side switches (most commonly P-type FETs)
    digitOn = LOW;
    segmentOn = LOW;
    break;
  }

  digitOff = !digitOn;
  segmentOff = !segmentOn;

  // Save the input pin numbers to library variables
  for (byte segmentNum = 0 ; segmentNum < 8 ; segmentNum++) {
    segmentPins[segmentNum] = segmentPinsIn[segmentNum];
  }

  for (byte digitNum = 0 ; digitNum < this->numDigits ; digitNum++) {
    digitPins[digitNum] = digitPinsIn[digitNum];
  }

  // Set the pins as outputs, and turn them off
  for (byte digit=0 ; digit < this->numDigits ; digit++) {
    pinMode(digitPins[digit], OUTPUT);
    digitalWrite(digitPins[digit], digitOff);
  }

  for (byte segmentNum=0 ; segmentNum < 8 ; segmentNum++) {
    pinMode(segmentPins[segmentNum], OUTPUT);
    digitalWrite(segmentPins[segmentNum], segmentOff);
  }

  // Initialize display blank
  for (byte digit=0 ; digit < this->numDigits ; digit++) {
    setDigit(digit, BLANK);
  }

}


// refreshDisplay
/******************************************************************************/
// Flashes the output on the seven segment display.
// This is achieved by cycling through all segments and digits, turning the
// required segments on as specified by the array 'digitCodes'.
// There are 2 versions of this function, with the choice depending on the
// location of the current-limiting resistors.

// Without args, calls with current class var state
void SevSeg::refreshDisplay(){
  this->refreshDisplay(this->ledOnTime);
}

void SevSeg::refreshDisplay(int micros){

  // For resistors on *digits* we will cycle through all 8 segments (7 +
  // period), turning on the *digits* as appropriate for a given segment,
  // before moving on to the next segment
  for (byte segmentNum=0 ; segmentNum < 8 ; segmentNum++) {

    // Illuminate the required digits for this segment
    digitalWrite(segmentPins[segmentNum], segmentOn);
    for (byte digitNum=0 ; digitNum < this->numDigits ; digitNum++){
      if (this->digitCodes[digitNum] & (1 << segmentNum)) { // Check a single bit
        digitalWrite(digitPins[digitNum], digitOn);
      }
    }

    //Wait with lights on (to increase brightness)
    delayMicroseconds(micros); 

    //Turn all lights off
    for (byte digitNum=0 ; digitNum < this->numDigits ; digitNum++){
      digitalWrite(digitPins[digitNum], digitOff);
    }
    digitalWrite(segmentPins[segmentNum], segmentOff);
  }

}

// setBrightness
/******************************************************************************/

void SevSeg::setBrightness(int brightness){
  brightness = constrain(brightness, 0, 100);
  this->ledOnTime = map(brightness, 0, 100, 1, 2000);
}

// setDigit
/******************************************************************************/
// Explicitly set a single digitCode element for display

void SevSeg::setDigit(byte digitNum, byte value) {
  if (digitNum >= 0 && digitNum < this->numDigits && value >= 0 && value <= DASH) {
    this->digitCodes[digitNum] = digitCodeMap[value];
  }
}

// setDigits
/******************************************************************************/
// Explicitly set a single digitCode element for display

void SevSeg::setDigits(byte values[], size_t num_values) {
  if (num_values <= this->numDigits) {
    for (size_t i=0; i<num_values; i++) {
      if (values[i] >= 0 && values[i] <= DASH) {
        this->digitCodes[i] = digitCodeMap[values[i]];
      }
    }
  }
}
