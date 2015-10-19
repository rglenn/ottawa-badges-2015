#include "Arduino.h"

#pragma once

uint8_t crc4_getChecksum(uint32_t packet);
uint8_t crc4_checkChecksum(uint32_t packet);
