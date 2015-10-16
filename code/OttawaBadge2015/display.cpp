#include "Arduino.h"
#include "display.h"

#define NUM_PINS 5
#define NUM_LEDS 20

volatile uint32_t framebuffer;
volatile uint8_t currentLed;

void display_init() {
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  framebuffer = 0;
  currentLed = 0;
}

void display_setPixel(uint8_t ledNumber, uint8_t val) {
  if(ledNumber >= NUM_LEDS) ledNumber = NUM_LEDS - 1; // silently limits parameter to actual number of LEDs. so don't use imaginary LEDs.
  
  if(val) {
    framebuffer |= (1 << ledNumber);
  } else {
    framebuffer &= ~(1 << ledNumber);
  }
}

uint8_t display_getPixel(uint8_t ledNumber) {
  return (framebuffer & (1 << ledNumber));
}

void display_setFrame(uint32_t frame) {
  framebuffer = frame;
}

uint32_t display_getFrame() {
  return framebuffer;
}

void display_update() {
  if(currentLed >= NUM_LEDS) {
    currentLed = 0;
  }

  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
    
  // this switch statement sets the anode
  if(framebuffer & (uint32_t)(1UL << currentLed)) {
    switch(currentLed) {
      case 0:
      case 1:
      case 2:
      case 3:
        pinMode(4, OUTPUT);
        digitalWrite(4, HIGH);
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        pinMode(5, OUTPUT);
        digitalWrite(5, HIGH);
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        pinMode(6, OUTPUT);
        digitalWrite(6, HIGH);
        break;
      case 12:
      case 13:
      case 14:
      case 15:
        pinMode(7, OUTPUT);
        digitalWrite(7, HIGH);
        break;
      case 16:
      case 17:
      case 18:
      case 19:
        pinMode(8, OUTPUT);
        digitalWrite(8, HIGH);
        break;
    }
  }

  switch(currentLed) {
    case 4:
    case 8:
    case 12:
    case 16:
      pinMode(4, OUTPUT);
      digitalWrite(4, LOW);
      break;
    case 0:
    case 9:
    case 13:
    case 17:
      pinMode(5, OUTPUT);
      digitalWrite(5, LOW);
      break;
    case 1:
    case 5:
    case 14:
    case 18:
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
      break;
    case 2:
    case 6:
    case 10:
    case 19:
      pinMode(7, OUTPUT);
      digitalWrite(7, LOW);
      break;
    case 3:
    case 7:
    case 11:
    case 15:
      pinMode(8, OUTPUT);
      digitalWrite(8, LOW);
      break;
  }

  currentLed++;
}

