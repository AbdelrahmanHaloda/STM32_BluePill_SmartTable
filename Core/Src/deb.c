#include "deb.h"
#include "io.h"

#ifndef DEB_THRESHOLD_MS
#define DEB_THRESHOLD_MS 20
#endif

// ---- switches (12) ----
static uint8_t  sw_state[DEB_SW_N];
static uint8_t  sw_event_rise[DEB_SW_N];
static uint8_t  sw_cnt[DEB_SW_N];

// ---- buttons (3: 0=UP,1=DOWN,2=OK) ----
static uint8_t  btn_state[DEB_BTN_N];
static uint8_t  btn_event_rise[DEB_BTN_N];
static uint8_t  btn_cnt[DEB_BTN_N];

static inline uint8_t btn_raw(uint8_t i) {
  switch (i) {
    case 0: return btn_up();   // PB14
    case 1: return btn_down(); // PB15
    case 2: return btn_ok();   // PA15
    default: return 0;
  }
}

void deb_init(void) {
  // switches
  for (int i = 0; i < DEB_SW_N; ++i) {
    sw_state[i] = sw_read(i);
    sw_event_rise[i] = 0;
    sw_cnt[i] = 0;
  }
  // buttons
  for (int i = 0; i < DEB_BTN_N; ++i) {
    btn_state[i] = btn_raw(i);
    btn_event_rise[i] = 0;
    btn_cnt[i] = 0;
  }
}

static inline void deb_integrate(uint8_t raw, uint8_t *state, uint8_t *cnt, uint8_t *rise) {
  if (raw == *state) {
    if (*cnt) (*cnt)--;
  } else {
    if (*cnt < DEB_THRESHOLD_MS) (*cnt)++;
    if (*cnt >= DEB_THRESHOLD_MS) {
      *state = raw;
      *cnt = 0;
      if (raw) *rise = 1;
    }
  }
}

void deb_poll_1ms(void) {
  for (int i = 0; i < DEB_SW_N; ++i) {
    deb_integrate(sw_read(i), &sw_state[i], &sw_cnt[i], &sw_event_rise[i]);
  }
  for (int i = 0; i < DEB_BTN_N; ++i) {
    deb_integrate(btn_raw(i), &btn_state[i], &btn_cnt[i], &btn_event_rise[i]);
  }
}

// --- switches API ---
uint8_t deb_sw_is_pressed(uint8_t i) {
  if (i >= DEB_SW_N) return 0;
  return sw_state[i];
}
uint8_t deb_sw_pressed_edge(uint8_t i) {
  if (i >= DEB_SW_N) return 0;
  uint8_t e = sw_event_rise[i];
  sw_event_rise[i] = 0;
  return e;
}

// --- buttons API ---
uint8_t deb_btn_is_pressed(uint8_t i) {
  if (i >= DEB_BTN_N) return 0;
  return btn_state[i];
}
uint8_t deb_btn_pressed_edge(uint8_t i) {
  if (i >= DEB_BTN_N) return 0;
  uint8_t e = btn_event_rise[i];
  btn_event_rise[i] = 0;
  return e;
}
