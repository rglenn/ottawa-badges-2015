// Requires the following libraries:
// https://github.com/z3t0/Arduino-IRremote
// https://github.com/Chris--A/Keypad

#include <IRremote.h>
#include <IRremoteInt.h>

#include <Key.h>
#include <Keypad.h>

#include "crc4.h"

IRsend irsend;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'*','0','#','D'},
  {'7','8','9','C'},
  {'4','5','6','B'},
  {'1','2','3','A'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {14, 15, 16, 17}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// What follows is a lot of code copied and pasted from the badge code.
// My excuse for doing it this way: this was built on the floor of the Maker Faire.
// I mean literally, the floor: I was sitting on concrete when I wrote this.

void infrared_sendRaw(uint32_t packet) {
  Serial.print("Sending packet: ");
  Serial.println(packet, BIN);
  irsend.sendSony(packet, 20);
  delay(15);
}

uint32_t infrared_buildPacket(uint8_t packetType, uint16_t irPayload) {
  uint32_t scratch = 0;
  uint8_t crcVal;

  scratch = ((uint32_t)(packetType & 0x0F)) << 16UL;
  scratch |= (irPayload & 0x0FFF) << 4;
  crcVal = crc4_getChecksum(scratch);
  scratch |= (crcVal & 0x0F);
  return scratch;
}

void infrared_sendPlayAnimationCommand(uint8_t animationNum, uint8_t timeoutVal) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (animationNum & 0x0F) << 8;
  payload |= timeoutVal;

  packet = infrared_buildPacket(0x0a, payload);
  infrared_sendRaw(packet);
}

// Copied and pasted code ends here.

void setup() {
  Serial.begin(9600);
}

void loop() {
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
    switch(customKey) {
      case '0':
        infrared_sendPlayAnimationCommand(0, 0);
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        // customKey minus '0' gives us the numeric value of the ASCII character. So the character '9' becomes the actual value 9.
        infrared_sendPlayAnimationCommand(customKey - '0', 255);
        break;
      case '*':
        infrared_sendPlayAnimationCommand(10, 255);
        break;
      case '#':
        infrared_sendPlayAnimationCommand(11, 255);
        break;
    }
  }
}
