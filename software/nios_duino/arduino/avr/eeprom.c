//! NIOSDuino custom file, (c) Dmitry Grigoryev, 2018

#include "eeprom.h"

static uint8_t eedata[E2END+1];

uint8_t eeprom_read_byte (const uint8_t *p) { return eedata[(unsigned int)p]; }

void eeprom_write_byte (uint8_t *p, uint8_t value) { eedata[(unsigned int)p] = value; }
