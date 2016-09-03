#include <Time.h>
#include <TimeLib.h>
#include <avr/sleep.h>

/*
  Button

 Turns on and off a light emitting diode(LED) connected to digital
 pin 13, when pressing a pushbutton attached to pin 2.


 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground

 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.


 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 3;     // the number of the pushbutton pin (Pin 2 on ATtiny)
const int redPin = 0;       // (Pin 5 on ATtiny)
const int greenPin = 1;     // (Pin 6 on ATtiny)
const int bluePin = 4;      // (Pin 3 on ATtiny)

// variables will change:
volatile int currentTime = (hour() * 60) + minute();
volatile int drinkAlarm = 3;           //1 = manually turned off, 0 = automatically

void setup() {
  // initialize the LED pin as an output:
  pinMode(13, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  //Setup interrupt on pin 2
  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK = 0b00000100;    // turn on interrupts on pin 7 (PB3)
  sei();                 // enables interrupts
  
  LEDCheck();
}

void loop() {
  if (currentTime >= 3570){
    currentTime = currentTime - 3600;
  }
  
int difference = ((hour() * 60) + minute()) - currentTime;

  //  Detect drink, start timer for 30 minutes
  if (drinkAlarm == 2){
    LEDCheck();
    alarmOff();
    drinkAlarm = 1;
    
  }else if(drinkAlarm == 1){
    if (difference >= 60){
      alarmOff();
      drinkAlarm = 0;

      startSleep();
      
      alarmOff();
      
    }else if(difference >= 30){
      rainbowLED();
    }
  }else{
    startSleep();
  }
}

ISR(PCINT0_vect){
  drinkAlarm = 2;
}

void startSleep(){
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
  
  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep
  
  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on
  
  sei();                                  // Enable interrupts
}

void alarmOff(){
  setColourRgb(0, 0, 0);
  currentTime = (hour() * 60) + minute();
}

void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
 }
 
void rainbowLED(){
    unsigned int rgbColour[3] = {255, 0, 0};

  // Choose the colours to increment and decrement.
  for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = decColour == 2 ? 0 : decColour + 1;

    // cross-fade the two colours.
    for(int i = 0; i < 255; i += 1) {
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;
      
      setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
      delay(7);
    }
  }
}

void LEDCheck(){
  setColourRgb(255,0,0);
  delay(500);
  setColourRgb(0,75,0);
  delay(500);
  setColourRgb(0,0,75);
  delay(500);
  setColourRgb(0,0,0);
}

