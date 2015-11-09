#include "Arduino.h"
#include <IRremote.h>
#include "persistent_data.h"

#pragma once

const int IR_IN_PIN = 9;
const int IR_OUT_PIN = 3;

typedef enum {IR_NONE, IR_OK, IR_BAD_FORMAT, IR_BAD_LENGTH, IR_BAD_CRC, IR_BAD_TYPE} irReturn;
typedef enum {
  IR_TYPE_IDENTIFY = 0x0C,
  IR_TYPE_IDENTIFY_RESPONSE = 0x01,
  IR_TYPE_SET_ID = 0x02,
  IR_TYPE_SET_ID_RESPONSE = 0x03,
  IR_TYPE_SET_NUM_MAKERS = 0x04,
  IR_TYPE_SET_NUM_MAKERS_RESPONSE = 0x05,
  IR_TYPE_LIST_MAKERS = 0x06,
  IR_TYPE_LIST_MAKERS_RESPONSE = 0x07,
  IR_TYPE_LIST_EXHIBITS = 0x08,
  IR_TYPE_LIST_EXHIBITS_RESPONSE = 0x09,
  IR_TYPE_PLAY_ANIMATION = 0x0A,
  IR_TYPE_BEACON = 0x0B,
  IR_TYPE_RESET = 0x0D,
  IR_TYPE_INVALID = 0xFF
} irType;

typedef struct {
  irType type;
  uint16_t param1;
  uint16_t param2;
  uint16_t param3;
} decodedPacket;

void infrared_enable();
void infrared_sendRaw(uint32_t packet);
irReturn infrared_checkPacket(uint32_t *packet);
uint32_t infrared_buildPacket(irType packetType, uint16_t irPayload);
void infrared_sendIdentifyCommand();
void infrared_sendIdentifyResponse(uint8_t boardType, uint8_t verMajor, uint8_t verMinor);
void infrared_sendSetIDCommand(uint16_t idVal, idType type);
void infrared_sendSetIDResponse(uint16_t idVal, idType type);
void infrared_sendSetNumMakersCommand(uint16_t numMakers, idType type);
void infrared_sendSetNumMakersResponse(uint16_t numMakers, idType type);
void infrared_sendListMakersCommand();
void infrared_sendListMakersResponse(uint16_t makerID);
void infrared_sendListExhibitsCommand();
void infrared_sendListExhibitsResponse(uint16_t exhibitID);
void infrared_sendPlayAnimationCommand(uint8_t animationNum, uint8_t timeoutVal);
void infrared_sendBeacon(uint16_t idVal, idType type);
void infrared_sendResetCommand();
decodedPacket infrared_decodePacket(uint32_t packet);

