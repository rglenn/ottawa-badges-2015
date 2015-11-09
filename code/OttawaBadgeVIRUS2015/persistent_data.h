#include "Arduino.h"

#pragma once

typedef enum {
  ID_TYPE_MAKER = 0x00,
  ID_TYPE_EXHIBIT = 0x01
} idType;

void persist_init();
uint16_t persist_getMaxMakers();
uint16_t persist_getMaxExhibits();
uint16_t persist_getMakerID();
void persist_setMakerID(uint16_t newID);
uint16_t persist_getExhibitID();
void persist_setExhibitID(uint16_t newID);
uint16_t persist_getNumMakers();
void persist_setNumMakers(uint16_t newNum);
uint16_t persist_getNumExhibits();
void persist_setNumExhibits(uint16_t newNum);
void persist_encounterMaker(uint16_t makerID);
uint8_t persist_haveEncounteredMaker(uint16_t makerID);
void persist_encounterExhibit(uint16_t exhibitID);
uint8_t persist_haveEncounteredExhibit(uint16_t exhibitID);
uint16_t persist_getNumMakersEncountered();
uint16_t persist_getNumExhibitsEncountered();
idType persist_getIDtype(uint16_t id);
void persist_erase();
