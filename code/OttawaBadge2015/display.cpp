/*
    This file is part of OttawaBadges2015.

    OttawaBadges2015 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OttawaBadges2015 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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

  DDRD &= 0x0F; // Digital 4-7 as input
  PORTD &= 0x0F;
  DDRB &= 0xFE; // Digital 8  as input
  PORTB &= 0xFE;
    
  // this switch statement sets the anode
  if(framebuffer & (uint32_t)(1UL << currentLed)) {
    switch(currentLed) {
      case 0:
      case 1:
      case 2:
      case 3:
        DDRD |= 0x10;
        PORTD |= 0x10;
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        DDRD |= 0x20;
        PORTD |= 0x20;
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        DDRD |= 0x40;
        PORTD |= 0x40;
        break;
      case 12:
      case 13:
      case 14:
      case 15:
        DDRD |= 0x80;
        PORTD |= 0x80;
        break;
      case 16:
      case 17:
      case 18:
      case 19:
        DDRB |= 0x01;
        PORTB |= 0x01;
        break;
    }
  }

  switch(currentLed) {
    case 4:
    case 8:
    case 12:
    case 16:
        DDRD |= 0x10;
        PORTD &= ~0x10;
      break;
    case 0:
    case 9:
    case 13:
    case 17:
        DDRD |= 0x20;
        PORTD &= ~0x20;
      break;
    case 1:
    case 5:
    case 14:
    case 18:
        DDRD |= 0x40;
        PORTD &= ~0x40;
      break;
    case 2:
    case 6:
    case 10:
    case 19:
        DDRD |= 0x80;
        PORTD &= ~0x80;
      break;
    case 3:
    case 7:
    case 11:
    case 15:
        DDRB |= 0x01;
        PORTB &= ~0x01;
      break;
  }

  currentLed++;
}

