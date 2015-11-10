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

// Requires the following libraries:
// https://github.com/z3t0/Arduino-IRremote

#include <IRremote.h>
#include <EEPROM.h>
#include "display.h"
#include "infrared.h"
#include "persistent_data.h"
#include "animations.h"

#define BOARD_TYPE 0x01
#define VER_MAJOR 0x01
#define VER_MINOR 0x00

// Instead of every 15 seconds, this "infected" badge transmits every 5 seconds
#define BEACON_INTERVAL_MS ( 5UL * 1000UL )
#define DISPLAY_INTERVAL_US 500
#define ANIMATION_INTERVAL_MS 100

uint32_t beacon_prevMillis = 0;
uint32_t display_prevMicros = 0;
uint32_t animation_prevMillis = 0;

#undef DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x...)  Serial.print (x)
 #define DEBUG_PRINTLN(x...)  Serial.println (x)
#else
 #define DEBUG_PRINT(x...)
 #define DEBUG_PRINTLN(x...)
#endif

void setup() {
  // Set up IO pins for display
  display_init();

  // Set up IR decoding
  infrared_enable();

  // Set up persistent storage
  persist_init();
  
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // Sending the Identify Response on bootup allows us to test the IR transmission easily.
  infrared_sendIdentifyResponse(BOARD_TYPE, VER_MAJOR, VER_MINOR);
}

void loop() {
  static uint16_t i = 0, j = 0;
  uint32_t frame;
  uint32_t rawPacket;
  decodedPacket packet;

  irReturn retVal = infrared_checkPacket(&rawPacket);

  static uint8_t animationNumber = 0;
  static uint8_t animationCountdown = 0;
  static uint8_t animationFrameNumber = 0;
  
  if(retVal == IR_OK) {
    // we have a valid packet!
    packet = infrared_decodePacket(rawPacket);
    
    switch(packet.type) {
        case IR_TYPE_IDENTIFY:
          DEBUG_PRINTLN("Received Identify");
          infrared_sendIdentifyResponse(BOARD_TYPE, VER_MAJOR, VER_MINOR);
          delay(20);
          infrared_sendSetIDResponse(persist_getMakerID(), ID_TYPE_MAKER);
          delay(20);
          infrared_sendSetIDResponse(persist_getExhibitID(), ID_TYPE_EXHIBIT);
          delay(10);
          break;
        case IR_TYPE_SET_ID:
          // check if MakerID or ExhibitID
          // update MakerID or ExhibitID depending
          // send response
          if(packet.param1 == 1) {
            DEBUG_PRINT("Setting ExhibitID to ");
            DEBUG_PRINTLN(packet.param2);
            persist_setExhibitID(packet.param2);
            infrared_sendSetIDResponse(persist_getExhibitID(), ID_TYPE_EXHIBIT);
          } else {
            DEBUG_PRINT("Setting MakerID to ");
            DEBUG_PRINTLN(packet.param2);
            persist_setMakerID(packet.param2);
            infrared_sendSetIDResponse(persist_getMakerID(), ID_TYPE_MAKER);
          }
          break;
        case IR_TYPE_SET_NUM_MAKERS:
          // check if MakerID or ExhibitID
          // update MakerCount or ExhibitCount depending
          // send response
          if(packet.param1 == 1) {
            DEBUG_PRINT("Setting Number of Exhibits to ");
            DEBUG_PRINTLN(packet.param2);
            persist_setNumExhibits(packet.param2);
            infrared_sendSetNumMakersResponse(persist_getNumExhibits(), ID_TYPE_EXHIBIT);
          } else {
            DEBUG_PRINT("Setting Number of Makers to ");
            DEBUG_PRINTLN(packet.param2);
            persist_setNumMakers(packet.param2);
            infrared_sendSetNumMakersResponse(persist_getNumMakers(), ID_TYPE_MAKER);
          }
          break;
        case IR_TYPE_LIST_MAKERS:
          // find number of makers encountered
          // send response for each one
          DEBUG_PRINTLN("Received List Makers");
          j = persist_getMaxMakers();
          for(i=1; i<(j+1); i++) {
            if(persist_haveEncounteredMaker(i)) {
              DEBUG_PRINT("Sending maker encountered for maker #");
              DEBUG_PRINTLN(i);
              infrared_sendListMakersResponse(i);
              delay(20);
            }
          }
          break;
        case IR_TYPE_LIST_EXHIBITS:
          // find number of exhibits encountered
          // send response for each one
          DEBUG_PRINTLN("Received List Exhibits");
          j = persist_getMaxExhibits();
          for(i=1; i<(j+1); i++) {
            if(persist_haveEncounteredExhibit(i)) {
              DEBUG_PRINT("Sending exhibit encountered for maker #");
              DEBUG_PRINTLN(i);
              infrared_sendListExhibitsResponse(i);
              delay(20);
            }
          }
          break;
        case IR_TYPE_PLAY_ANIMATION:
          // start playing indicated animation
          // packet.param1 contains the animation number
          // packet.param2 contains the animation duration, which is in 10ms units
          // so multiply it by 10 to get duration in ms
          DEBUG_PRINT("Received Play Animation for animation #");
          DEBUG_PRINT(packet.param1);
          DEBUG_PRINT(" with duration ");
          DEBUG_PRINTLN(packet.param2);
          animationNumber = (uint8_t) packet.param1;
          animationCountdown = (uint8_t) packet.param2;
          if(animationCountdown == 0) animationNumber = 0;
          animationFrameNumber = 0;
          break;
        case IR_TYPE_BEACON:
          // check if MakerID or ExhibitID
          // mark that maker / exhibit as encountered

          // MakerID / ExhibitID 0 doesn't count - that's an unprogrammed badge
          if(packet.param2 != 0) {
            if(packet.param1 == 1) {
              DEBUG_PRINT("Encountering exhibit ");
              DEBUG_PRINTLN(packet.param2);
              persist_encounterExhibit(packet.param2);
            } else {
              DEBUG_PRINT("Encountering maker ");
              DEBUG_PRINTLN(packet.param2);
              persist_encounterMaker(packet.param2);
            }
          }
          break;
        case IR_TYPE_RESET:
          DEBUG_PRINTLN("Erase received!");
          persist_erase();
          break;
        default:
          // unknown packet - just drop it
          break;
    }
  } else if(retVal == IR_BAD_FORMAT) {
    DEBUG_PRINTLN(F("PACKET: INVALID, BAD FORMAT"));
  } else if(retVal == IR_BAD_LENGTH) {
    DEBUG_PRINTLN(F("PACKET: INVALID, BAD LENGTH"));
  } else if(retVal == IR_BAD_CRC) {
    DEBUG_PRINTLN(F("PACKET: INVALID, BAD CRC"));
  }

  uint32_t currentMillis = millis();
  uint32_t currentMicros = micros();

  // Send out beacons periodically
  if(currentMillis - beacon_prevMillis > BEACON_INTERVAL_MS) {
    DEBUG_PRINTLN("Beacon time!");
    beacon_prevMillis = currentMillis;

    // If the MakerID hasn't been set, we don't send a beacon for it
    if(persist_getMakerID() != 0) {
      infrared_sendBeacon(persist_getMakerID(), ID_TYPE_MAKER);
      delay(20);
    }

    // If the ExhibitID hasn't been set, we don't send a beacon for it
    if(persist_getExhibitID() != 0) {
      infrared_sendBeacon(persist_getExhibitID(), ID_TYPE_EXHIBIT);
      delay(20);
    }
    // Infect other badges by setting the number of Makers and Exhibits to 50 each, rather than 246 and 120, respectively.
    // This will make it easier to fill the progress bars.
    infrared_sendSetNumMakersCommand(50, ID_TYPE_EXHIBIT);
    delay(20);
    infrared_sendSetNumMakersCommand(50, ID_TYPE_MAKER);
    delay(20);
    // Send out 5 beacon packets to try and add 5 makers to everyone
    infrared_sendBeacon(42, ID_TYPE_MAKER);
    delay(20);
    infrared_sendBeacon(43, ID_TYPE_MAKER);
    delay(20);
    infrared_sendBeacon(44, ID_TYPE_MAKER);
    delay(20);
    infrared_sendBeacon(45, ID_TYPE_MAKER);
    delay(20);
    infrared_sendBeacon(46, ID_TYPE_MAKER);
    delay(20);
    // Play the Larson Scanner animation to show that the process is done.
    infrared_sendPlayAnimationCommand(9, 40);
    delay(20);
  }

  // We update the display every 500 microseconds.
  // 20 updates are required to draw the entire display - only one LED is lit at a time.
  // This ensures constant brightness no matter how many LEDs are on, and improves
  // battery life - CR2032s don't like providing lots of current.
  
  // 500 microseconds times 20 LEDs = 10 milliseconds to draw the entire display
  // This gives us a 100 Hz frame rate on the display, which is fast enough for humans
  // not to notice.
  
  // Update display periodically
  if(currentMicros - display_prevMicros > DISPLAY_INTERVAL_US) {
    display_prevMicros = currentMicros;
    display_update();
  }

  if(currentMillis - animation_prevMillis > ANIMATION_INTERVAL_MS) {
    animation_prevMillis = currentMillis;

    if(animationNumber == 0) {
      display_setFrame(doBargraph());
    } else {
      // run an animation
      display_setFrame(doAnimation(animationNumber, &animationFrameNumber));

      // timeout the animation
      animationCountdown--;
      if(animationCountdown == 0) {
        animationNumber = 0;
      }
    }
  }
}

uint32_t doBargraph() {
  uint32_t frame = 0;
  uint8_t numLEDs;
  numLEDs = map(persist_getNumMakersEncountered(), 0, persist_getNumMakers(), 0, 10);

  // To allow easier testing of the LED array, we turn on all the LEDs if the number of makers hasn't been set yet
  if(persist_getNumMakers() == 0) numLEDs = 10;
  
  switch(numLEDs) {
    case 10:
      frame |= 0b01010101010101010101UL;
      break;
    case 9:
      frame |= 0b010101010101010101UL;
      break;
    case 8:
      frame |= 0b0101010101010101UL;
      break;
    case 7:
      frame |= 0b01010101010101UL;
      break;
    case 6:
      frame |= 0b010101010101UL;
      break;
    case 5:
      frame |= 0b0101010101UL;
      break;
    case 4:
      frame |= 0b01010101UL;
      break;
    case 3:
      frame |= 0b010101UL;
      break;
    case 2:
      frame |= 0b0101UL;
      break;
    case 1:
      frame |= 0b01UL;
      break;
    default:
      break;
  }

  numLEDs = map(persist_getNumExhibitsEncountered(), 0, persist_getNumExhibits(), 0, 10);

  // To allow easier testing of the LED array, we turn on all the LEDs if the number of exhibits hasn't been set yet
  if(persist_getNumExhibits() == 0) numLEDs = 10;
  
  switch(numLEDs) {
    case 10:
      frame |= 0b010101010101010101010UL;
      break;
    case 9:
      frame |= 0b0101010101010101010UL;
      break;
    case 8:
      frame |= 0b01010101010101010UL;
      break;
    case 7:
      frame |= 0b010101010101010UL;
      break;
    case 6:
      frame |= 0b0101010101010UL;
      break;
    case 5:
      frame |= 0b01010101010UL;
      break;
    case 4:
      frame |= 0b010101010UL;
      break;
    case 3:
      frame |= 0b0101010UL;
      break;
    case 2:
      frame |= 0b01010UL;
      break;
    case 1:
      frame |= 0b010UL;
      break;
    default:
      break;
  }

  return frame;
}

