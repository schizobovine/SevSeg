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

// SevSeg
/******************************************************************************/
SevSeg::SevSeg()
{
  // Initial value
  this->numDigits = 0;
  this->pos = 0;
  this->skip_cycles = 0;
  this->max_pos = NUM_SEGMENTS; // Max brightness
}


// begin
/******************************************************************************/
// Saves the input pin numbers to the class and sets up the pins to be used.

void SevSeg::begin(byte hardwareConfig, byte numDigitsIn, 
                  const byte digitPinsIn[], const byte segmentPinsIn[]) {
                    
  this->numDigits = numDigitsIn;

  //Limit the max number of digits to prevent overflowing
  if (this->numDigits > MAXNUMDIGITS) this->numDigits = MAXNUMDIGITS;

  // Allocate memory for anode/cathode arrays
  this->digitPins = new byte[numDigits];
  this->digitCodes = new byte[numDigits];
  this->segmentPins = new byte[NUM_SEGMENTS];

  switch (hardwareConfig){

  case 0: // Common cathode
    this->digitOn = LOW;
    this->segmentOn = HIGH;
    break;

  case 1: // Common anode
    this->digitOn = HIGH;
    this->segmentOn = LOW;
    break;

  case 2: // With active-high, low-side switches (most commonly N-type FETs)
    this->digitOn = HIGH;
    this->segmentOn = HIGH;
    break;

  case 3: // With active low, high side switches (most commonly P-type FETs)
    this->digitOn = LOW;
    this->segmentOn = LOW;
    break;
  }

  this->digitOff = !this->digitOn;
  this->segmentOff = !this->segmentOn;

  // Save the input pin numbers to library variables
  for (byte segmentNum = 0 ; segmentNum < NUM_SEGMENTS ; segmentNum++) {
    segmentPins[segmentNum] = segmentPinsIn[segmentNum];
  }

  for (byte digitNum = 0 ; digitNum < this->numDigits ; digitNum++) {
    digitPins[digitNum] = digitPinsIn[digitNum];
  }

  // Set the pins as outputs, and turn them off
  for (byte digit=0 ; digit < this->numDigits ; digit++) {
    pinMode(digitPins[digit], OUTPUT);
    digitalWrite(digitPins[digit], this->digitOff);
  }

  for (byte segmentNum=0 ; segmentNum < NUM_SEGMENTS ; segmentNum++) {
    pinMode(segmentPins[segmentNum], OUTPUT);
    digitalWrite(segmentPins[segmentNum], this->segmentOff);
  }

  // Initialize display blank
  for (byte digit=0 ; digit < this->numDigits ; digit++) {
    setDigit(digit, BLANK);
  }

}

// setDigitOn
/******************************************************************************/
// Enable a given digit

void SevSeg::setDigitOn(byte digitNum) {
  byte segments = this->digitCodes[digitNum];
  for (byte segmentNum=0; segmentNum<NUM_SEGMENTS; segmentNum++) {
    if (segments & 1) {
      digitalWrite(segmentPins[segmentNum], this->segmentOn);
    }
    segments >>= 1;
  }
  digitalWrite(digitPins[digitNum], this->digitOn);
}

// setDigitOff
/******************************************************************************/
// Disable a given digit

void SevSeg::setDigitOff(byte digitNum) {
  digitalWrite(digitPins[digitNum], this->digitOff);
  for (byte segmentNum=0; segmentNum<NUM_SEGMENTS; segmentNum++) {
    digitalWrite(segmentPins[segmentNum], this->segmentOff);
  }
}

// illuminateNext
/******************************************************************************/
// Activates a single segment on the display, deactivating any other segment.
// Useful for calling from a timer interrupt for more consistent brightness.

void SevSeg::illuminateNext() {
  byte digitNum;

  // Disable previous digits and segment
  if (this->pos < NUM_SEGMENTS) {
    digitalWrite(segmentPins[this->pos], this->segmentOff);
    for (digitNum=0; digitNum < this->numDigits; digitNum++) {
      digitalWrite(digitPins[digitNum], this->digitOff);
    }
  }

  // Advance to next segment or empty cycle
  this->pos = (this->pos + 1) % this->max_pos;

  // Illuminate the required digits for this segment
  if (this->pos < NUM_SEGMENTS) {
    digitalWrite(segmentPins[this->pos], this->segmentOn);
    for (digitNum=0; digitNum < this->numDigits; digitNum++) {
      if (this->digitCodes[digitNum] & (1 << this->pos)) {
        digitalWrite(digitPins[digitNum], this->digitOn);
      }
    }
  }

}

// setBrightness
/******************************************************************************/

void SevSeg::setBrightness(byte brightness){
  this->skip_cycles = map(brightness, 0, 255, MAX_SKIP_CYCLES, 0);
  this->max_pos = this->skip_cycles + NUM_SEGMENTS;
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
