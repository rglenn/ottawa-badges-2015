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

#include "animations.h"
#include "display.h"
#include <avr/pgmspace.h>

uint32_t ledChaseUp(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;
  
  actualFrameNumber = (*animationFrameNumber) / 2;

  frame = 1;
  frame <<= actualFrameNumber;

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 40; // rollover at 20 updates

  return frame;
}

uint32_t ledChaseDown(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;
  
  actualFrameNumber = (*animationFrameNumber) / 2;

  frame = 1;
  frame <<= (19 - actualFrameNumber);

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 40; // rollover at 20 updates

  return frame;
}

uint32_t ledChaseUpDown(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;

  actualFrameNumber = (*animationFrameNumber) / 2;

  frame = 1;

  if((*animationFrameNumber) <= 40) {
    frame <<= actualFrameNumber;
  } else {
    actualFrameNumber -= 21;
    frame <<= (19 - actualFrameNumber);
  }

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 80; // rollover at 40 updates

  return frame;
}

uint32_t ledChaseAround(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint8_t ledNumber;
  uint32_t frame;

  actualFrameNumber = (*animationFrameNumber) / 2;

  frame = 1;

  if((actualFrameNumber) < 10) {
    ledNumber = actualFrameNumber * 2;
  } else {
    ledNumber = 20-(2*(actualFrameNumber - 10)) - 1;
  }

  frame <<= ledNumber;
  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 40; // rollover at 40 updates

  return frame;
}

uint32_t ledMarquee(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;

  actualFrameNumber = (*animationFrameNumber) / 2;

  if(actualFrameNumber == 0) {
    frame = 419430;
  } else {
    frame = 629145;
  }

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 4;

  return frame;
}

uint32_t ledDoubleUp(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;
  
  actualFrameNumber = (*animationFrameNumber) / 4;
  actualFrameNumber *= 2;

  frame = 3;
  frame <<= actualFrameNumber;

  (*animationFrameNumber) += 2;
  (*animationFrameNumber) %= 40; // rollover at 40 updates

  return frame;
}

uint32_t ledDoubleDown(uint8_t *animationFrameNumber) {
  uint8_t actualFrameNumber;
  uint32_t frame;
  
  actualFrameNumber = (*animationFrameNumber) / 4;
  actualFrameNumber *= 2;

  frame = 3;
  frame <<= (19 - actualFrameNumber)-1;

  (*animationFrameNumber) += 2;
  (*animationFrameNumber) %= 40; // rollover at 40 updates

  return frame;
}

const uint32_t larsonPattern[] PROGMEM = { 0b00000000000000000000000000000000UL,
                                           0b00000000000000000000000000000011UL,
                                           0b00000000000000000000000000001100UL,
                                           0b00000000000000000000000000110000UL,
                                           0b00000000000000000000000011000000UL,
                                           0b00000000000000000000001100000000UL,
                                           0b00000000000000000000110000000000UL,
                                           0b00000000000000000011000000000000UL,
                                           0b00000000000000001100000000000000UL,
                                           0b00000000000000110000000000000000UL,
                                           0b00000000000011000000000000000000UL,
                                           0b00000000000000000000000000000000UL,
                                           0b00000000000011000000000000000000UL,
                                           0b00000000000000110000000000000000UL,
                                           0b00000000000000001100000000000000UL,
                                           0b00000000000000000011000000000000UL,
                                           0b00000000000000000000110000000000UL,
                                           0b00000000000000000000001100000000UL,
                                           0b00000000000000000000000011000000UL,
                                           0b00000000000000000000000000110000UL,
                                           0b00000000000000000000000000001100UL,
                                           0b00000000000000000000000000000011UL};
#define LARSON_STEPS 22

uint32_t ledLarsonTogether(uint8_t *animationFrameNumber) {
  uint32_t frame;
  uint8_t actualFrameNumber;

  actualFrameNumber = (*animationFrameNumber);

  frame = pgm_read_dword_near(larsonPattern + actualFrameNumber);

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= (LARSON_STEPS); // rollover at 40 updates

  return frame;
}

const uint32_t oppositeLarsonPattern[] PROGMEM = { 0b00000000000000000000000000000000UL,
                                           0b00000000000001000000000000000010UL,
                                           0b00000000000000010000000000001000UL,
                                           0b00000000000000000100000000100000UL,
                                           0b00000000000000000001000010000000UL,
                                           0b00000000000000000000011000000000UL,
                                           0b00000000000000000000100100000000UL,
                                           0b00000000000000000010000001000000UL,
                                           0b00000000000000001000000000010000UL,
                                           0b00000000000000100000000000000100UL,
                                           0b00000000000010000000000000000001UL,
                                           0b00000000000000000000000000000000UL,
                                           0b00000000000010000000000000000001UL,
                                           0b00000000000000100000000000000100UL,
                                           0b00000000000000001000000000010000UL,
                                           0b00000000000000000010000001000000UL,
                                           0b00000000000000000000100100000000UL,
                                           0b00000000000000000000011000000000UL,
                                           0b00000000000000000001000010000000UL,
                                           0b00000000000000000100000000100000UL,
                                           0b00000000000000010000000000001000UL,
                                           0b00000000000001000000000000000010UL};
#define OPPOSITE_LARSON_STEPS 22

uint32_t ledLarsonOpposite(uint8_t *animationFrameNumber) {
  uint32_t frame;
  uint8_t actualFrameNumber;

  actualFrameNumber = (*animationFrameNumber);

  frame = pgm_read_dword_near(oppositeLarsonPattern + actualFrameNumber);

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= (OPPOSITE_LARSON_STEPS); // rollover at 40 updates

  return frame;
}

uint32_t ledBlink(uint8_t *animationFrameNumber) {
  uint32_t frame;

  if((*animationFrameNumber) < 10) {
    frame = 0xFFFFFFFFUL;
  } else {
    frame = 0;
  }

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 20; // rollover at 40 updates

  return frame;
}

uint32_t ledBlinkAlternate(uint8_t *animationFrameNumber) {
  uint32_t frame;

  if((*animationFrameNumber) < 10) {
    frame = 0x55555555UL;
  } else {
    frame = 0xAAAAAAAAUL;
  }

  (*animationFrameNumber)++;
  (*animationFrameNumber) %= 20; // rollover at 40 updates

  return frame;
}

uint32_t doAnimation(uint8_t animationNumber, uint8_t *animationFrameNumber) {
  switch(animationNumber) {
    case 0:
      return 0;
      break;
    case 1:
      return ledChaseUp(animationFrameNumber); // works
      break;
    case 2:
      return ledChaseDown(animationFrameNumber); // works
      break;
    case 3:
      return ledChaseUpDown(animationFrameNumber); // works
      break;
    case 4:
      return ledChaseAround(animationFrameNumber); // works
      break;
    case 5:
      return ledMarquee(animationFrameNumber); // works
      break;
    case 6:
      return ledDoubleUp(animationFrameNumber); // works
      break;
    case 7:
      return ledDoubleDown(animationFrameNumber); // works
      break;
    case 8:
      return ledLarsonTogether(animationFrameNumber); // works
      break;
    case 9:
      return ledLarsonOpposite(animationFrameNumber); // works
      break;
    case 10:
      return ledBlink(animationFrameNumber); // works
      break;
    case 11:
      return ledBlinkAlternate(animationFrameNumber); // works
      break;
    case 12:
    case 13:
    case 14:
    case 15:
    default:
      return 0;
      break;
  }
}
