/*
 * ui_lcd.h
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */

#pragma once
#include <stdint.h>

// simple level enum we'll reuse later
typedef enum { LVL_EASY = 0, LVL_MEDIUM = 1, LVL_HARD = 2, LVL_COUNT } level_t;

// draw the 20x4 menu with an arrow on the selected row
void ui_menu_draw(level_t sel);


void ui_game_screen (level_t sel, uint16_t hits, uint16_t trials, uint32_t ms_per_step);
void ui_game_summary(level_t sel, uint16_t hits, uint16_t trials);


void lcd_write_line(int row, const char *s);
