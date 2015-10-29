// Requires the following libraries:
// https://github.com/z3t0/Arduino-IRremote

#include <IRremote.h>
#include "infrared.h"

typedef enum {
  STATE_WAIT_COMMAND = 0,
  STATE_WAIT_PARAM_1,
  STATE_WAIT_PARAM_2,
  STATE_WAIT_PARAM_3
} badgeCommandState;

void setup() {
  // Set up IR decoding
  infrared_enable();

  Serial.begin(9600);
}

badgeCommandState currentState = STATE_WAIT_COMMAND;

void loop() {
  static uint16_t i = 0, j = 0;
  uint32_t frame;
  uint32_t rawPacket;
  decodedPacket packet;

  irReturn retVal = infrared_checkPacket(&rawPacket);

  if(retVal == IR_OK) {
    // we have a valid packet!
    packet = infrared_decodePacket(rawPacket);
    Serial.print(F("PACKET: "));
    Serial.print(packet.type, DEC);
    Serial.print(F(","));
    Serial.print(packet.param1, HEX);
    Serial.print(F(","));
    Serial.print(packet.param2, HEX);
    Serial.print(F(","));
    Serial.println(packet.param3, HEX);
  } else if(retVal == IR_BAD_FORMAT) {
    Serial.println(F("PACKET: INVALID, BAD FORMAT"));
  } else if(retVal == IR_BAD_LENGTH) {
    Serial.println(F("PACKET: INVALID, BAD LENGTH"));
  } else if(retVal == IR_BAD_CRC) {
    Serial.println(F("PACKET: INVALID, BAD CRC"));
  }

  char ch;
  static irType packetType;
  static uint16_t param1, param2, param3;
  while(Serial.available() > 0) {
    ch = Serial.read();

    switch(currentState) {
      case STATE_WAIT_COMMAND:
        param1 = 0;
        param2 = 0;
        param3 = 0;
        if(ch == 'H') {
          printInfo();
        } else {
          packetType = getTypeFromChar(ch);
          if(packetType != IR_TYPE_INVALID) {
            currentState = STATE_WAIT_PARAM_1;
          }
        }
        break;
      case STATE_WAIT_PARAM_1:
        if ((ch <= '9') && (ch >= '0')) {
          param1 *= 10;
          param1 += (ch - '0');
          currentState = STATE_WAIT_PARAM_1;
        } else if(ch == ',') {
          currentState = STATE_WAIT_PARAM_2;
        } else {
          currentState = STATE_WAIT_COMMAND;
        }
        break;
      case STATE_WAIT_PARAM_2:
        if ((ch <= '9') && (ch >= '0')) {
          param2 *= 10;
          param2 += (ch - '0');
          currentState = STATE_WAIT_PARAM_2;
        } else if(ch == ',') {
          currentState = STATE_WAIT_PARAM_3;
        } else {
          currentState = STATE_WAIT_COMMAND;
        }
        break;
      case STATE_WAIT_PARAM_3:
        if ((ch <= '9') && (ch >= '0')) {
          param3 *= 10;
          param3 += (ch - '0');
          currentState = STATE_WAIT_PARAM_3;
        } else if((ch == '\r') || (ch == '\n')) {
          sendIRPacket(packetType, param1, param2, param3);
          currentState = STATE_WAIT_COMMAND;
        } else {
          currentState = STATE_WAIT_COMMAND;
        }
        break;

    }
  }
}

void printInfo() {
  Serial.println("Ottawa Maker Faire Badge Base Station v1.0");
}

irType getTypeFromChar(char ch) {
  switch(ch) {
    case 'I':
      return IR_TYPE_IDENTIFY;
      break;
    case 'J':
      return IR_TYPE_IDENTIFY_RESPONSE;
      break;
    case 'S':
      return IR_TYPE_SET_ID;
      break;
    case 'T':
      return IR_TYPE_SET_ID_RESPONSE;
      break;
    case 'N':
      return IR_TYPE_SET_NUM_MAKERS;
      break;
    case 'O':
      return IR_TYPE_SET_NUM_MAKERS_RESPONSE;
      break;
    case 'L':
      return IR_TYPE_LIST_MAKERS;
      break;
    case 'M':
      return IR_TYPE_LIST_MAKERS_RESPONSE;
      break;
    case 'E':
      return IR_TYPE_LIST_EXHIBITS;
      break;
    case 'F':
      return IR_TYPE_LIST_EXHIBITS_RESPONSE;
      break;
    case 'P':
      return IR_TYPE_PLAY_ANIMATION;
      break;
    case 'B':
      return IR_TYPE_BEACON;
      break;
    case 'Z':
     return IR_TYPE_RESET;
     break;
    default:
      return IR_TYPE_INVALID;
      break;
  }
}

void sendIRPacket(irType packetType, uint16_t param1, uint16_t param2, uint16_t param3) {
  Serial.println("Sending packet");
  switch(packetType) {
    case IR_TYPE_IDENTIFY:
      Serial.println("Sending Identify packet");
      infrared_sendIdentifyCommand();
      break;
    case IR_TYPE_IDENTIFY_RESPONSE:
      infrared_sendIdentifyResponse((uint8_t) param1, (uint8_t) param2, (uint8_t) param3);
      break;
    case IR_TYPE_SET_ID:
      infrared_sendSetIDCommand(param1, (idType) param2);
      break;
    case IR_TYPE_SET_ID_RESPONSE:
      infrared_sendSetIDResponse(param1, (idType) param2);
      break;
    case IR_TYPE_SET_NUM_MAKERS:
      infrared_sendSetNumMakersCommand(param1, (idType) param2);
      break;
    case IR_TYPE_SET_NUM_MAKERS_RESPONSE:
      infrared_sendSetNumMakersResponse(param1, (idType) param2);
      break;
    case IR_TYPE_LIST_MAKERS:
      infrared_sendListMakersCommand();
      break;
    case IR_TYPE_LIST_MAKERS_RESPONSE:
      infrared_sendListMakersResponse(param1);
      break;
    case IR_TYPE_LIST_EXHIBITS:
      infrared_sendListExhibitsCommand();
      break;
    case IR_TYPE_LIST_EXHIBITS_RESPONSE:
      infrared_sendListExhibitsResponse(param1);
      break;
    case IR_TYPE_PLAY_ANIMATION:
      infrared_sendPlayAnimationCommand((uint8_t) param1, (uint8_t) param2);
      break;
    case IR_TYPE_BEACON:
      infrared_sendBeacon(param1, (idType) param2);
      break;
    case IR_TYPE_RESET:
      infrared_sendResetCommand();
      break;
  }
}

