#ifndef BIT_LEDS_H
#define BIT_LEDS_H

#include <stdint.h>
#include <stdbool.h>

void bit_leds_init();
void bit_leds_enable(bool enable);
void bit_leds_clear();
void bit_leds_latch();
void bit_leds_set(uint32_t value);

#endif