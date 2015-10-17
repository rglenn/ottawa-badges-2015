#include "Arduino.h"
#include "persistent_data.h"

#define NUM_MAKERS 1024
#define numMakersArray (NUM_MAKERS/8)

static uint8_t makersEncountered[numMakersArray];

#define NUM_EXHIBITS 1024
#define numExhibitsArray (NUM_EXHIBITS/8)

static uint8_t exhibitsEncountered[numExhibitsArray];

static uint16_t numMakers;
static uint16_t numExhibits;
static uint16_t myMakerID;
static uint16_t myExhibitID;

void persist_init() {
  // MAJOR TODO: Read everything from EEPROM if a check value is found in EEPROM, otherwise default to 0
  uint8_t i;
  for(i=0; i<numMakersArray; i++) {
    makersEncountered[i] = 0;
  }

  for(i=0; i<numExhibitsArray; i++) {
    exhibitsEncountered[i] = 0;
  }

  numMakers = 0;
  numExhibits = 0;
  myMakerID = 0;
  myExhibitID = 0;
}

uint16_t persist_getMaxMakers() {
  return numMakers;
}

uint16_t persist_getMaxExhibits() {
  return numExhibits;
}

uint16_t persist_getMakerID() {
  return myMakerID;
}

void persist_setMakerID(uint16_t newID) {
  myMakerID = (newID & 0x07FF);
  // MAJOR TODO: Write to EEPROM on change
}

uint16_t persist_getExhibitID() {
  return myExhibitID;
}

void persist_setExhibitID(uint16_t newID) {
  myExhibitID = (newID & 0x07FF);
  // MAJOR TODO: Write to EEPROM on change
}

uint16_t persist_getNumMakers() {
  return numMakers;
}

void persist_setNumMakers(uint16_t newNum) {
  numMakers = (newNum & 0x07FF);
  // MAJOR TODO: Write to EEPROM on change
}

uint16_t persist_getNumExhibits() {
  return numExhibits;
}

void persist_setNumExhibits(uint16_t newNum) {
  numExhibits = (newNum & 0x07FF);
  // MAJOR TODO: Write to EEPROM on change
}

void persist_encounterMaker(uint16_t makerID) {
  makerID = (makerID & 0x07FF);

  uint8_t arraySlot, bitNum;
  arraySlot = makerID / 8;
  bitNum = makerID % 8;

  uint8_t scratch = makersEncountered[arraySlot];
  scratch |= (1 << bitNum);
  makersEncountered[arraySlot] = scratch;
  // MAJOR TODO: Write to EEPROM on change
}

uint8_t persist_haveEncounteredMaker(uint16_t makerID) {
  makerID = (makerID & 0x07FF);

  uint8_t arraySlot, bitNum;
  arraySlot = makerID / 8;
  bitNum = makerID % 8;

  uint8_t scratch = makersEncountered[arraySlot];
  if (scratch & (1 << bitNum)) {
    return 1;
  }
  return 0;
}

void persist_encounterExhibit(uint16_t exhibitID) {
  exhibitID = (exhibitID & 0x07FF);

  uint8_t arraySlot, bitNum;
  arraySlot = exhibitID / 8;
  bitNum = exhibitID % 8;

  uint8_t scratch = exhibitsEncountered[arraySlot];
  scratch |= (1 << bitNum);
  exhibitsEncountered[arraySlot] = scratch;
  // MAJOR TODO: Write to EEPROM on change
}

uint8_t persist_haveEncounteredExhibit(uint16_t exhibitID) {
  exhibitID = (exhibitID & 0x07FF);

  uint8_t arraySlot, bitNum;
  arraySlot = exhibitID / 8;
  bitNum = exhibitID % 8;

  uint8_t scratch = exhibitsEncountered[arraySlot];
  if (scratch & (1 << bitNum)) {
    return 1;
  }
  return 0;
}

static const uint8_t oneBits[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

static uint8_t countOnes(uint8_t x) {
  uint8_t results;
  results = oneBits[x&0x0f];
  results += oneBits[x>>4];
  return results;
}

uint16_t persist_getNumMakersEncountered() {
  uint16_t scratch = 0;
  
  for(uint8_t i = 0; i < numMakersArray; i++) {
    scratch += countOnes(makersEncountered[i]);
  }

  return scratch;
}

uint16_t persist_getNumExhibitsEncountered() {
  uint16_t scratch = 0;
  
  for(uint8_t i = 0; i < numExhibitsArray; i++) {
    scratch += countOnes(exhibitsEncountered[i]);
  }

  return scratch;
}

idType persist_getIDtype(uint16_t id) {
  if(id & 0x0800) {
    return ID_TYPE_EXHIBIT;
  } else {
    return ID_TYPE_MAKER;
  }
}

