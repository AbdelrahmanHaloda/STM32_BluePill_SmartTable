#pragma once
#include "main.h"
#include <stdint.h>

// Number of channels
#define LED_N 12
#define SW_N  12

void io_init_after_mx(void);

// LED control (index: 0..11 == LED1..LED12)
void led_on(uint8_t i);
void led_off(uint8_t i);
void led_toggle(uint8_t i);

// Reads (raw, no debounce yet). Returns 0/1.
uint8_t sw_read(uint8_t i);    // limit switches 0..11
uint8_t btn_up(void);
uint8_t btn_down(void);
uint8_t btn_ok(void);
