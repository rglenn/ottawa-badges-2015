#include "Arduino.h"
#include "crc4.h"

static const uint8_t crc4lookup[] = {0, 0b00000011, 0b00000110, 0b00000101, 0b00001100, 0b00001111, 0b00001010, 0b00001001, 0b00001011, 0b00001000, 0b00001101, 0b00001110, 0b00000111, 0b00000100, 0b00000001, 0b00000010};

static uint8_t crcResult = 0;

static void crc4reset() {
  crcResult = 0;
}

static uint8_t crc4step(uint8_t input) {
  input &= 0x0F; // LSBs only
  input ^= crcResult;
  crcResult = crc4lookup[input];
  return crcResult;
}

uint8_t crc4_getChecksum(uint32_t packet) {
  uint8_t scratch;

  crc4reset();
  scratch = (uint8_t) ((packet & 0x000F0000UL) >> 16);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x0000F000UL) >> 12);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x00000F00UL) >> 8);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x000000F0UL) >> 4);
  return crc4step(scratch);
}

uint8_t crc4_checkChecksum(uint32_t packet) {
  uint8_t scratch;

  crc4reset();
  scratch = (uint8_t) ((packet & 0x000F0000UL) >> 16);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x0000F000UL) >> 12);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x00000F00UL) >> 8);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x000000F0UL) >> 4);
  crc4step(scratch);
  scratch = (uint8_t) ((packet & 0x0000000FUL));
  return (crc4step(scratch) == 0);
}

