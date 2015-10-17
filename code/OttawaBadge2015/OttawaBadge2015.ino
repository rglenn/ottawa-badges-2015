// Requires the following libraries:
// https://github.com/PaulStoffregen/TimerOne
// https://github.com/z3t0/Arduino-IRremote (not implemented yet)

#include <TimerOne.h>
#include <IRremote.h>
#include "display.h"
#include "infrared.h"
#include "persistent_data.h"

#define BOARD_TYPE 0x01
#define VER_MAJOR 0x01
#define VER_MINOR 0x00

uint16_t MakerID;
uint16_t ExhibitID;

void setup() {
  // Set up IO pins for display
  display_init();

  // Set up IR decoding
  infrared_enable();

  // Set up persistent storage
  persist_init();
  
  // We update the display every 500 microseconds.
  // 20 updates are required to draw the entire display - only one LED is lit at a time.
  // This ensures constant brightness no matter how many LEDs are on, and improves
  // battery life - CR2032s don't like providing lots of current.
  
  // 500 microseconds times 20 LEDs = 10 milliseconds to draw the entire display
  // This gives us a 100 Hz frame rate on the display, which is fast enough for humans
  // not to notice.
  Timer1.initialize(500);
  Timer1.attachInterrupt(display_update);
  Timer1.start();
}

void loop() {
  static uint16_t i = 0;
  uint32_t frame;
  uint32_t rawPacket;
  decodedPacket packet;

  if(infrared_checkPacket(&rawPacket) == IR_OK) {
    // we have a valid packet!
    packet = infrared_decodePacket(rawPacket);

    switch(packet.type) {
        case IR_TYPE_IDENTIFY:
          infrared_sendIdentifyResponse(BOARD_TYPE, VER_MAJOR, VER_MINOR);
          delay(20);
          infrared_sendSetIDResponse(persist_getMakerID(), ID_TYPE_MAKER);
          delay(20);
          infrared_sendSetIDResponse(persist_getExhibitID(), ID_TYPE_EXHIBIT);
          delay(20);
          break;
        case IR_TYPE_SET_ID:
          // check if MakerID or ExhibitID
          // update MakerID or ExhibitID depending
          // send response
          if(packet.param1 == 1) {
            persist_setExhibitID(packet.param2);
            infrared_sendSetIDResponse(persist_getExhibitID(), ID_TYPE_EXHIBIT);
          } else {
            persist_setMakerID(packet.param2);
            infrared_sendSetIDResponse(persist_getMakerID(), ID_TYPE_MAKER);
          }
          break;
        case IR_TYPE_SET_NUM_MAKERS:
          // check if MakerID or ExhibitID
          // update MakerCount or ExhibitCount depending
          // send response
          if(packet.param1 == 1) {
            persist_setNumExhibits(packet.param2);
            infrared_sendSetNumMakersResponse(persist_getNumExhibits(), ID_TYPE_EXHIBIT);
          } else {
            persist_setNumMakers(packet.param2);
            infrared_sendSetNumMakersResponse(persist_getNumMakers(), ID_TYPE_MAKER);
          }
          break;
        case IR_TYPE_LIST_MAKERS:
          // find number of makers encountered
          // send response for each one
          break;
        case IR_TYPE_LIST_EXHIBITS:
          // find number of exhibits encountered
          // send response for each one
          break;
        case IR_TYPE_PLAY_ANIMATION:
          // start playing indicated animation
          break;
        case IR_TYPE_BEACON:
          // check if MakerID or ExhibitID
          // mark that maker / exhibit as encountered
          if(packet.param1 == 1) {
            persist_encounterExhibit(packet.param2);
          } else {
            persist_encounterMaker(packet.param2);
          }
          break;
        default:
          // unknown packet - just drop it
          break;
    }
  }


  // Super basic LED chase. We get what's on the display currently.
  // If nothing is lit, we light the first LED. Otherwise, we just shift it left.
  // Left shifts continue, until we shift off the end - at which point everything
  // is off, and we start with the first LED again.
  
  frame = display_getFrame();

  // The frame variable now contains the status of all 20 LEDs in one 32-bit variable
  // - bit 0 is LED1 on the PCB, bit 1 is LED2, and so on.
  // You can also use display_setPixel(ledNumber, state) to set a given LED to 
  // on (state=1) or off (state=0)
  // display_getPixel(ledNumber) will return 0 if that LED is off, and 1 if it is on.
  // Note that the ledNumber parameters for those functions are 0-based, so 0 is LED1, 1 is LED2, etc.
  if(frame == 0) {
    frame = 1;
  } else {
    frame <<= 1;
  }
  display_setFrame(frame);
  i = 0;

  delay(250);
}
