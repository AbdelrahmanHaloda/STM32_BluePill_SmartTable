/*
 * ui_lcd.c
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */

#include "ui_lcd.h"
#include "i2c-lcd.h"


void lcd_write_line(int row, const char *s)
{
    char buf[21];
    int n = 0;
    if (s) {
        while (n < 20 && s[n]) { buf[n] = s[n]; n++; }
    }
    while (n < 20) { buf[n++] = ' '; }
    buf[20] = '\0';
    lcd_put_cur(row, 0);
    lcd_send_string(buf);
}

static const char* LVL_NAME[LVL_COUNT] = {
  "Easy   (5 s)",
  "Medium (3 s)",
  "Hard   (1 s)"
};

void ui_menu_draw(level_t sel)
{
    if (sel >= LVL_COUNT) sel = 0;

    // Don’t blast-clear the whole DDRAM; just overwrite lines safely
    lcd_write_line(0, "Select Level:");

    for (int i = 0; i < LVL_COUNT; ++i) {
        char line[21];
        // 2 chars for arrow + space, name fits into remaining slots
        // Ensure the formatted string never exceeds 20 (lcd_write_line will trim/pad)
        snprintf(line, sizeof(line), "%c %-17s",
                 (i == sel) ? '>' : ' ', LVL_NAME[i]);
        lcd_write_line(1 + i, line);   // rows 1..3
    }
}


extern void lcd_put_cur(int row, int col);
extern void lcd_send_string(char *str);


void ui_game_screen(level_t sel, uint16_t hits, uint16_t trials, uint32_t ms_per_step)
{
  char line[21];

  lcd_write_line(0, "Game: hit lit LED");

  snprintf(line, sizeof(line),
           "Level: %s",
           sel == LVL_EASY ? "Easy" : sel == LVL_MEDIUM ? "Medium" : "Hard");
  lcd_write_line(1, line);

  snprintf(line, sizeof(line), "Score: %u/%u", (unsigned)hits, (unsigned)trials);
  lcd_write_line(2, line);

  snprintf(line, sizeof(line), "Step: %lus  OK=End",
           (unsigned)(ms_per_step/1000u));
  lcd_write_line(3, line);
}

void ui_game_summary(level_t sel, uint16_t hits, uint16_t trials)
{
  char line[21];
  lcd_write_line(0, "Session finished");
  snprintf(line, sizeof(line), "Level: %s",
           sel == LVL_EASY ? "Easy" : sel == LVL_MEDIUM ? "Medium" : "Hard");
  lcd_write_line(1, line);
  snprintf(line, sizeof(line), "Result: %u/%u", (unsigned)hits, (unsigned)trials);
  lcd_write_line(2, line);
  lcd_write_line(3, "OK: back to menu");
}


