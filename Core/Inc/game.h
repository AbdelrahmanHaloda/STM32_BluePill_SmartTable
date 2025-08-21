/*
 * game.h
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ui_lcd.h"   // for level_t

typedef struct {
  level_t   level;
  uint32_t  interval_ms;    // per-level step time (Easy=5000, Med=3000, Hard=1000)
  uint32_t  next_ms;        // next step time
  int8_t    current;        // 0..11 currently lit, -1 = none
  uint16_t  hits;           // successful presses
  uint16_t  trials;         // total steps shown
  uint16_t  max_trials;     // stop after N trials (test = 20)
  uint8_t   awaiting_press; // 1 while waiting for correct hit
  uint32_t  rng;            // PRNG state
} game_t;

void  game_start(game_t* g, level_t lvl, uint32_t now_ms);
bool  game_tick (game_t* g, uint32_t now_ms);        // returns true when session finished
void  game_handle_press(game_t* g, uint8_t sw_idx);  // call on debounced rising edge
