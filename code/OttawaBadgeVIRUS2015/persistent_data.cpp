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

#include <EEPROM.h>
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

#define MAKERS_ENCOUNTERED_ADDRESS 0
#define EXHIBITS_ENCOUNTERED_ADDRESS 128
#define NUM_MAKERS_ADDRESS 256
#define NUM_EXHIBITS_ADDRESS 258
#define MAKER_ID_ADDRESS 260
#define EXHIBIT_ID_ADDRESS 262
#define CHECK_VALUE_ADDRESS 264
#define CHECK_VALUE 0x4242

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

  uint16_t checkValue;
  EEPROM.get(CHECK_VALUE_ADDRESS, checkValue);
  if(checkValue != CHECK_VALUE) {
    // initialize eeprom
    checkValue = CHECK_VALUE;
    EEPROM.put(CHECK_VALUE_ADDRESS, checkValue);
    EEPROM.put(EXHIBIT_ID_ADDRESS, myExhibitID);
    EEPROM.put(MAKER_ID_ADDRESS, myMakerID);
    EEPROM.put(NUM_EXHIBITS_ADDRESS, numExhibits);
    EEPROM.put(NUM_MAKERS_ADDRESS, numMakers);
    EEPROM.put(EXHIBITS_ENCOUNTERED_ADDRESS, exhibitsEncountered);
    EEPROM.put(MAKERS_ENCOUNTERED_ADDRESS, makersEncountered);
  } else {
    // read values from eeprom
    EEPROM.get(EXHIBIT_ID_ADDRESS, myExhibitID);
    EEPROM.get(MAKER_ID_ADDRESS, myMakerID);
    EEPROM.get(NUM_EXHIBITS_ADDRESS, numExhibits);
    EEPROM.get(NUM_MAKERS_ADDRESS, numMakers);
    EEPROM.get(EXHIBITS_ENCOUNTERED_ADDRESS, exhibitsEncountered);
    EEPROM.get(MAKERS_ENCOUNTERED_ADDRESS, makersEncountered);
  }
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
  newID = (newID & 0x07FF);
  
  if(myMakerID != newID) {
    myMakerID = newID;
    // Write to EEPROM on change
    EEPROM.put(MAKER_ID_ADDRESS, myMakerID);
  }
}

uint16_t persist_getExhibitID() {
  return myExhibitID;
}

void persist_setExhibitID(uint16_t newID) {
  newID = (newID & 0x07FF);

  if(myExhibitID != newID) {
    myExhibitID = newID;
    // Write to EEPROM on change
    EEPROM.put(EXHIBIT_ID_ADDRESS, myExhibitID);
  }
}

uint16_t persist_getNumMakers() {
  return numMakers;
}

void persist_setNumMakers(uint16_t newNum) {
  newNum = (newNum & 0x07FF);

  if(numMakers != newNum) {
    numMakers = newNum;
    // Write to EEPROM on change
    EEPROM.put(NUM_MAKERS_ADDRESS, numMakers);
  }
}

uint16_t persist_getNumExhibits() {
  return numExhibits;
}

void persist_setNumExhibits(uint16_t newNum) {
  newNum = (newNum & 0x07FF);
  
  if(numExhibits != newNum) {
    numExhibits = newNum;
    // Write to EEPROM on change
    EEPROM.put(NUM_EXHIBITS_ADDRESS, numExhibits);
  }
}

void persist_encounterMaker(uint16_t makerID) {
  makerID = (makerID & 0x07FF);

  uint8_t arraySlot, bitNum;
  arraySlot = makerID / 8;
  bitNum = makerID % 8;

  uint8_t scratch = makersEncountered[arraySlot];
  scratch |= (1 << bitNum);
  if(makersEncountered[arraySlot] != scratch) {
    makersEncountered[arraySlot] = scratch;
    // Write to EEPROM on change
    EEPROM.put(MAKERS_ENCOUNTERED_ADDRESS, makersEncountered);
  }
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
  if(exhibitsEncountered[arraySlot] != scratch) {
    exhibitsEncountered[arraySlot] = scratch;
    // Write to EEPROM on change
    EEPROM.put(EXHIBITS_ENCOUNTERED_ADDRESS, exhibitsEncountered);
  }
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

void persist_erase() {
  // initialize eeprom
  uint16_t checkValue = CHECK_VALUE;
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
  
  EEPROM.put(CHECK_VALUE_ADDRESS, checkValue);
  EEPROM.put(EXHIBIT_ID_ADDRESS, myExhibitID);
  EEPROM.put(MAKER_ID_ADDRESS, myMakerID);
  EEPROM.put(NUM_EXHIBITS_ADDRESS, numExhibits);
  EEPROM.put(NUM_MAKERS_ADDRESS, numMakers);
  EEPROM.put(EXHIBITS_ENCOUNTERED_ADDRESS, exhibitsEncountered);
  EEPROM.put(MAKERS_ENCOUNTERED_ADDRESS, makersEncountered);
}

