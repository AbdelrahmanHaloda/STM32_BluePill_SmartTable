/*
 * game.c
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */


#include "game.h"
#include "io.h"
#include "ui_lcd.h"

static uint32_t xorshift32(uint32_t* s) {
  uint32_t x = *s;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  return *s = x;
}

static uint8_t rand12(game_t* g) {
  return (uint8_t)(xorshift32(&g->rng) % 12u);
}

static void all_off(void) {
  for (int i = 0; i < 12; ++i) led_off(i);
}

static uint32_t level_interval_ms(level_t lvl) {
  switch (lvl) {
    case LVL_EASY:   return 5000;
    case LVL_MEDIUM: return 3000;
    default:         return 1000;  // LVL_HARD
  }
}

void game_start(game_t* g, level_t lvl, uint32_t now_ms) {
  g->level          = lvl;
  g->interval_ms    = level_interval_ms(lvl);
  g->hits           = 0;
  g->trials         = 0;
  g->max_trials     = 20;         // short session for testing
  g->rng            = (now_ms ^ (0x9E3779B9u * (1u + (uint32_t)lvl)));

  all_off();
  g->current        = -1;
  g->awaiting_press = 0;

  // pick first target
  int8_t n = (int8_t)rand12(g);
  g->current = n;
  led_on(n);
  g->awaiting_press = 1;
  g->next_ms = now_ms + g->interval_ms;

  ui_game_screen(g->level, g->hits, g->trials, g->interval_ms);
}

bool game_tick(game_t* g, uint32_t now_ms) {
  if ((int32_t)(now_ms - g->next_ms) < 0) return false;

  // step window ended
  g->trials++;

  // if still awaiting a press, it was a miss (we just advance)
  // light next random LED (different from previous if possible)
  int8_t prev = g->current;
  int8_t n = prev;
  if (g->trials < g->max_trials) {
    for (int k = 0; k < 5 && n == prev; ++k) n = (int8_t)rand12(g);
  }
  all_off();
  if (g->trials >= g->max_trials) {
    g->current = -1;
    g->awaiting_press = 0;
    ui_game_summary(g->level, g->hits, g->trials);
    return true; // finished
  }

  g->current = n;
  led_on(n);
  g->awaiting_press = 1;
  g->next_ms += g->interval_ms;   // fixed cadence
  ui_game_screen(g->level, g->hits, g->trials, g->interval_ms);
  return false;
}

void game_handle_press(game_t* g, uint8_t sw_idx) {
  if (!g->awaiting_press) return;
  if (sw_idx == (uint8_t)g->current) {
    g->hits++;
    g->awaiting_press = 0;
    // give immediate visual feedback: turn LED off until next step
    led_off(g->current);
    ui_game_screen(g->level, g->hits, g->trials, g->interval_ms);
  }
}
