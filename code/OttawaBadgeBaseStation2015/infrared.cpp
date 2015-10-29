#include "Arduino.h"
#include "infrared.h"
#include <IRremote.h>
#include "crc4.h"

IRrecv irrecv(IR_IN_PIN);
IRsend irsend;

void infrared_enable() {
  irrecv.enableIRIn();
}

void infrared_sendRaw(uint32_t packet) {
  Serial.print("Sending packet: ");
  Serial.println(packet, BIN);
  irsend.sendSony(packet, 20);
  delay(15);
  irrecv.enableIRIn();
}

irReturn infrared_checkPacket(uint32_t *packet) {
  decode_results results;
  
  if(!irrecv.decode(&results)) {
    return IR_NONE;
  }
  
  // first check that the packet was a Sony format
  if(results.decode_type != SONY) {
    irrecv.resume();
    return IR_BAD_FORMAT;
  }

  // next check number of bits
  if(results.bits != 20) {
    irrecv.resume();
    return IR_BAD_LENGTH;
  }

  // next check CRC
  *packet = results.value;
  if(!crc4_checkChecksum(*packet)) {
    irrecv.resume();
    return IR_BAD_CRC;
  }

  // now check command type


  irrecv.resume();

  // ok if it passes
  return IR_OK;  
}

uint32_t infrared_buildPacket(irType packetType, uint16_t irPayload) {
  uint32_t scratch = 0;
  uint8_t crcVal;

  scratch = ((uint32_t)(packetType & 0x0F)) << 16UL;
  scratch |= (irPayload & 0x0FFF) << 4;
  crcVal = crc4_getChecksum(scratch);
  scratch |= (crcVal & 0x0F);
  return scratch;
}

void infrared_sendIdentifyCommand() {
  uint32_t packet;

  packet = infrared_buildPacket(IR_TYPE_IDENTIFY, 0);
  infrared_sendRaw(packet);
}

void infrared_sendIdentifyResponse(uint8_t boardType, uint8_t verMajor, uint8_t verMinor) {
  uint16_t payload = 0;
  uint32_t packet;
  
  payload |= (boardType & 0x0F) << 8;
  payload |= (verMajor & 0x0F) << 4;
  payload |= (verMinor & 0x0F);

  packet = infrared_buildPacket(IR_TYPE_IDENTIFY_RESPONSE, payload);
  infrared_sendRaw(packet);
}

void infrared_sendSetIDCommand(uint16_t idVal, idType type) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (idVal & 0x07FF);
  if(type == ID_TYPE_EXHIBIT) {
    payload |= 0x0800;
  }

  packet = infrared_buildPacket(IR_TYPE_SET_ID, payload);
  infrared_sendRaw(packet);
}

void infrared_sendSetIDResponse(uint16_t idVal, idType type) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (idVal & 0x07FF);
  if(type == ID_TYPE_EXHIBIT) {
    payload |= 0x0800;
  }

  packet = infrared_buildPacket(IR_TYPE_SET_ID_RESPONSE, payload);
  infrared_sendRaw(packet);
}

void infrared_sendSetNumMakersCommand(uint16_t numMakers, idType type) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (numMakers & 0x07FF);
  if(type == ID_TYPE_EXHIBIT) {
    payload |= 0x0800;
  }

  packet = infrared_buildPacket(IR_TYPE_SET_NUM_MAKERS, payload);
  infrared_sendRaw(packet);
}

void infrared_sendSetNumMakersResponse(uint16_t numMakers, idType type) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (numMakers & 0x07FF);
  if(type == ID_TYPE_EXHIBIT) {
    payload |= 0x0800;
  }

  packet = infrared_buildPacket(IR_TYPE_SET_NUM_MAKERS_RESPONSE, payload);
  infrared_sendRaw(packet);
}

void infrared_sendListMakersCommand() {
  uint16_t payload = 0;
  uint32_t packet = 0;

  packet = infrared_buildPacket(IR_TYPE_LIST_MAKERS, payload);
  infrared_sendRaw(packet);
}

void infrared_sendListMakersResponse(uint16_t makerID) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (makerID & 0x07FF);

  packet = infrared_buildPacket(IR_TYPE_LIST_MAKERS_RESPONSE, payload);
  infrared_sendRaw(packet);
}

void infrared_sendListExhibitsCommand() {
  uint16_t payload = 0;
  uint32_t packet = 0;

  packet = infrared_buildPacket(IR_TYPE_LIST_EXHIBITS, payload);
  infrared_sendRaw(packet);
}

void infrared_sendListExhibitsResponse(uint16_t exhibitID) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (exhibitID & 0x07FF);

  packet = infrared_buildPacket(IR_TYPE_LIST_EXHIBITS_RESPONSE, payload);
  infrared_sendRaw(packet);
}

void infrared_sendPlayAnimationCommand(uint8_t animationNum, uint8_t timeoutVal) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (animationNum & 0x0F) << 8;
  payload |= timeoutVal;

  packet = infrared_buildPacket(IR_TYPE_PLAY_ANIMATION, payload);
  infrared_sendRaw(packet);
}

void infrared_sendBeacon(uint16_t idVal, idType type) {
  uint16_t payload = 0;
  uint32_t packet;

  payload |= (idVal & 0x07FF);
  if(type == ID_TYPE_EXHIBIT) {
    payload |= 0x0800;
  }  

  packet = infrared_buildPacket(IR_TYPE_BEACON, payload);
  infrared_sendRaw(packet);
}

void infrared_sendResetCommand() {
  uint16_t payload = 0;
  uint32_t packet = 0;

  packet = infrared_buildPacket(IR_TYPE_RESET, payload);
  infrared_sendRaw(packet);
}

decodedPacket infrared_decodePacket(uint32_t packet) {
  decodedPacket scratch;

  scratch.type = (irType)((packet & 0x000F0000UL) >> 16);
  
  switch(scratch.type) {
    case IR_TYPE_IDENTIFY:
    case IR_TYPE_LIST_MAKERS:
    case IR_TYPE_LIST_EXHIBITS:
    case IR_TYPE_RESET:
    default:
      scratch.param1 = 0;
      scratch.param2 = 0;
      scratch.param3 = 0;
      break;
    case IR_TYPE_IDENTIFY_RESPONSE:
      scratch.param1 = ((packet & 0x0000F000UL) >> 12); // board type
      scratch.param2 = ((packet & 0x00000F00UL) >> 8); // major ver
      scratch.param3 = ((packet & 0x000000F0UL) >> 4); // minor ver
      break;
    case IR_TYPE_SET_ID:
    case IR_TYPE_SET_ID_RESPONSE:
    case IR_TYPE_SET_NUM_MAKERS:
    case IR_TYPE_SET_NUM_MAKERS_RESPONSE:
    case IR_TYPE_BEACON:
      scratch.param1 = (packet & 0x00008000UL) ? 1 : 0; // MakerID or ExhibitID
      scratch.param2 = (packet & 0x00007FF0UL) >> 4; // Actual ID value
      scratch.param3 = 0;
      break;
    case IR_TYPE_LIST_MAKERS_RESPONSE:
    case IR_TYPE_LIST_EXHIBITS_RESPONSE:
      scratch.param1 = (packet & 0x00007FF0UL) >> 4; // Actual ID value
      scratch.param2 = 0;
      scratch.param3 = 0;
      break;
    case IR_TYPE_PLAY_ANIMATION:
      scratch.param1 = (packet & 0x0000F000UL) >> 12; // Animation number
      scratch.param2 = (packet & 0x00000FF0UL) >> 4; // Animation duration
      scratch.param3 = 0;
      break;
  }
  
  return scratch;
}

