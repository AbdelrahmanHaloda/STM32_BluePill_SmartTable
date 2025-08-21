/*
 * io.c
 *
 *  Created on: Aug 21, 2025
 *      Author: abdelrahmanhaloda
 */


#include "io.h"

// LED cathodes (open-drain outputs, High=OFF, Low=ON)
static GPIO_TypeDef* const LED_PORT[LED_N] = {
  GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA
};
static const uint16_t LED_PIN[LED_N] = {
  GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,
  GPIO_PIN_7,GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,GPIO_PIN_11,GPIO_PIN_12
};

// Limit-switch inputs (indices 0..11 = SW1..SW12)
static GPIO_TypeDef* const SW_PORT[SW_N] = {
  GPIOB, GPIOB, GPIOA, GPIOB, GPIOB, GPIOB,
  GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB
};
static const uint16_t SW_PIN[SW_N] = {
  GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_8,
  GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_3
};


// Navigation buttons
#define BTN_UP_PORT     GPIOB
#define BTN_UP_PIN      GPIO_PIN_14
#define BTN_DOWN_PORT   GPIOB
#define BTN_DOWN_PIN    GPIO_PIN_15
#define BTN_OK_PORT     GPIOA
#define BTN_OK_PIN      GPIO_PIN_15

// ---------------------------------------------------------

void io_init_after_mx(void)
{
  // Make sure LEDs start OFF (Open-drain: HIGH = Hi-Z = OFF)
  for (int i = 0; i < LED_N; ++i) {
    HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i], GPIO_PIN_SET);
  }
}

void led_on(uint8_t i)
{
  if (i < LED_N) HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i], GPIO_PIN_RESET); // sink
}
void led_off(uint8_t i)
{
  if (i < LED_N) HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i], GPIO_PIN_SET);   // float
}
void led_toggle(uint8_t i)
{
  if (i < LED_N) HAL_GPIO_TogglePin(LED_PORT[i], LED_PIN[i]);
}

uint8_t sw_read(uint8_t i)
{
  if (i >= SW_N) return 0;
  return (HAL_GPIO_ReadPin(SW_PORT[i], SW_PIN[i]) == GPIO_PIN_SET) ? 1 : 0;
}

uint8_t btn_up(void)   { return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET; }
uint8_t btn_down(void) { return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET; }
uint8_t btn_ok(void)   { return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET; }

