#include "Arduino.h"

#pragma once

void display_init();
void display_setPixel(uint8_t ledNumber, uint8_t val);
uint8_t display_getPixel(uint8_t ledNumber);
void display_setFrame(uint32_t frame);
uint32_t display_getFrame();
void display_update();

