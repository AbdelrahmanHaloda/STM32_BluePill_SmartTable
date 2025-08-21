/*
 * deb.h
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */

#pragma once
#include <stdint.h>

// 12 limit switches
#define DEB_SW_N  12
// 3 nav buttons: 0=UP, 1=DOWN, 2=OK
#define DEB_BTN_N 3

void deb_init(void);          // call once
void deb_poll_1ms(void);      // call every 1 ms

// Limit switches
uint8_t deb_sw_is_pressed(uint8_t i);     // 0..11
uint8_t deb_sw_pressed_edge(uint8_t i);   // latched rising edge, auto-clears

// Nav buttons
uint8_t deb_btn_is_pressed(uint8_t i);    // 0=UP,1=DOWN,2=OK
uint8_t deb_btn_pressed_edge(uint8_t i);  // latched rising edge, auto-clears
