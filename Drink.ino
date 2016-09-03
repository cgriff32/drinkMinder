#include <Time.h>
#include <TimeLib.h>
#include <avr/sleep.h>

// constants won't change. They're used here to
// set pin numbers:
const int switchPin = 3;     // (Pin 2 on ATtiny)
const int redPin = 0;       // (Pin 5 on ATtiny)
const int greenPin = 1;     // (Pin 6 on ATtiny)
const int bluePin = 4;      // (Pin 3 on ATtiny)

// variables will change:
volatile int currentTime = (hour() * 60) + minute();
volatile int drinkAlarm = 3;           //1 = manually turned off, 0 = automatically

void setup() {
  // initialize the LED pin as an output:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  //Setup interrupt on pin 2
  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK = 0b00000100;    // turn on interrupts on pin 7 (PB3)
  sei();                 // enables interrupts
  
  LEDCheck();            // Display welcome led sequence
}

void loop() {
  // Since hour timer will roll over at 23 to 0, make sure we account for the difference in timing
  if (currentTime >= 3570){
    currentTime = currentTime - 3600;
  }
  
  // find elapsed time since last action
  int difference = ((hour() * 60) + minute()) - currentTime;

  //  Detect drink, start timer for 30 minutes
  if (drinkAlarm == 2){ // This means last action was a drink
    LEDCheck();         // Confirm drink
    alarmOff();         // Turn lights off, reset timer
    drinkAlarm = 1;     // 1 means alarm was manually turned off
    
  }else if(drinkAlarm == 1){  // Last action involved alarm being manually turned off
    if (difference >= 35){    // Light has been on for 5 minutes
      alarmOff();             // Turn drink alarm off, user won't be taking a drink
      drinkAlarm = 0;         // 0 means alarm was automatically turned off

      startSleep();           // Set the device in sleep mode, user must be away from drink
      
      alarmOff();             // Device has returned from sleep mode, reset timer.
      
    }else if(difference >= 30){ // 30 minutes since last drink
      rainbowLED();             // Let's see some colors (drink alarm on)
    }
  }else{
    startSleep();   // This occurs when the device was turned off automatically by being left alone for too long
  }
}

ISR(PCINT0_vect){
  drinkAlarm = 2;   // Interrupt used to return from sleep mode. The user must have taken a drink
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
  setColourRgb(0, 0, 0);                  // turn off the LED
  currentTime = (hour() * 60) + minute(); // Start a new timer, measured in minutes
}

//Used to write a value to each pin of the RGB LED
void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
 }
 
void rainbowLED(){
    unsigned int rgbColour[3] = {255, 0, 0};  // Start at red

  // Choose the colours to increment and decrement.
  for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = decColour == 2 ? 0 : decColour + 1;

    // cross-fade the two colours.
    for(int i = 0; i < 255; i += 1) {
      if (drinkAlarm == 2){ //The user drank, let's prematurely end this loop
        i = 255;
      }
      
      //Cycle through those colors
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;
      
      setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
      delay(7);
    }
  }
}

//  Blink red, green, blue as a welcome and confirmation message
void LEDCheck(){
  setColourRgb(255,0,0);
  delay(500);
  setColourRgb(0,75,0);
  delay(500);
  setColourRgb(0,0,75);
  delay(500);
  setColourRgb(0,0,0);
}

