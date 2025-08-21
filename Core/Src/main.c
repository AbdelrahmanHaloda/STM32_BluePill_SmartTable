/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
Electronics Studio
@file LCD16x2_I2c_F103
@Date 07-06-25
@url https:https: https://github.com/electronicsstudio
@url YouTube: https://www.youtube.com/@ElectronicsStudio-v7o/featured
*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"

#include "io.h"
#include "stdio.h"
#include "deb.h"
#include "ui_lcd.h"   // <--- add this
#include "game.h"
#include <string.h>  // Added for string manipulation functions
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  // ----- app state (0=MENU, 1=READY, 2=RUNNING, 3=SUMMARY)
  uint8_t  app_state = 0;
  level_t  sel = LVL_EASY;
  game_t   g;

  uint32_t last_ms = HAL_GetTick();

  // READY-state helpers
  uint32_t ready_end = 0;
  uint32_t next_sec = 0;
  uint32_t ready_blink_t = 0;   // <--- renamed (was blink_t)
  uint8_t  sec_left = 0;
  uint8_t  blink_on = 0;

  // draw initial menu
  ui_menu_draw(sel);
  io_init_after_mx();

  lcd_init();
  HAL_Delay(5);
  ui_menu_draw(LVL_EASY);     // start on Easy

  deb_init();


  /* USER CODE END 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 1) debounce at 1 ms
    uint32_t now = HAL_GetTick();
    if (now != last_ms) {
      for (; last_ms != now; last_ms++) {
        deb_poll_1ms();
      }
    }

    if (app_state == 0) {
      // ===== MENU =====
      if (deb_btn_pressed_edge(0)) {            // UP
        sel = (sel == 0) ? (LVL_COUNT - 1) : (sel - 1);
        ui_menu_draw(sel);
      }
      if (deb_btn_pressed_edge(1)) {            // DOWN
        sel = (sel + 1) % LVL_COUNT;
        ui_menu_draw(sel);
      }
      // OK -> READY (3s countdown)
      if (deb_btn_pressed_edge(2)) {
        for (int i = 0; i < 12; ++i) led_off(i);  // start clean

        sec_left       = 3;
        blink_on       = 0;
        ready_end      = now + 3000;
        next_sec       = now + 1000;
        ready_blink_t  = now;                    // <--- use ready_blink_t

        ui_ready_draw(sel, sec_left);
        app_state = 1;  // READY
      }

    } else if (app_state == 1) {
      // ===== READY (3s) =====

      // Blink all LEDs every ~200 ms
      if ((int32_t)(now - ready_blink_t) >= 400) {
        blink_on = !blink_on;
        for (int i = 0; i < 12; ++i) {
          if (blink_on) led_on(i); else led_off(i);
        }
        ready_blink_t += 400;                    // <--- advance ready_blink_t
      }

      // Update the countdown each second: 3 -> 2 -> 1
      if ((int32_t)(now - next_sec) >= 0 && sec_left > 1) {
        sec_left--;
        ui_ready_draw(sel, sec_left);
        next_sec += 1000;
      }

      // Allow OK to skip the wait
      if (deb_btn_pressed_edge(2)) {
        for (int i = 0; i < 12; ++i) led_off(i);
        game_start(&g, sel, now);
        app_state = 2;  // RUNNING
      }

      // Auto-start when the 3s is up
      if ((int32_t)(now - ready_end) >= 0) {
        for (int i = 0; i < 12; ++i) led_off(i);
        game_start(&g, sel, now);
        app_state = 2;  // RUNNING
      }

    } else if (app_state == 2) {
      // ===== RUNNING =====

      // a) handle debounced presses from 12 limit switches
      for (uint8_t i = 0; i < 12; ++i) {
        if (deb_sw_pressed_edge(i)) game_handle_press(&g, i);
      }

      // b) advance steps / finish?
      if (game_tick(&g, now)) {
        app_state = 3;   // SUMMARY (ui already drawn by game_tick)
      }

      // c) early end with OK
      if (deb_btn_pressed_edge(2)) {
        ui_game_summary(g.level, g.hits, g.trials);
        for (int i = 0; i < 12; ++i) led_off(i);
        app_state = 3;
      }

    } else {
      // ===== SUMMARY =====
      if (deb_btn_pressed_edge(2)) {
        for (int i = 0; i < 12; ++i) led_off(i);
        ui_menu_draw(sel);
        app_state = 0;
      }
    }
  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_1_Pin|LED_2_Pin|LED_3_Pin|LED_4_Pin
                          |LED_5_Pin|LED_6_Pin|LED_7_Pin|LED_8_Pin
                          |LED_9_Pin|LED_10_Pin|LED_11_Pin|LED_12_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LS_3_Pin BTN_OK_Pin */
  GPIO_InitStruct.Pin = LS_3_Pin|BTN_OK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_1_Pin LED_2_Pin LED_3_Pin LED_4_Pin
                           LED_5_Pin LED_6_Pin LED_7_Pin LED_8_Pin
                           LED_9_Pin LED_10_Pin LED_11_Pin LED_12_Pin */
  GPIO_InitStruct.Pin = LED_1_Pin|LED_2_Pin|LED_3_Pin|LED_4_Pin
                          |LED_5_Pin|LED_6_Pin|LED_7_Pin|LED_8_Pin
                          |LED_9_Pin|LED_10_Pin|LED_11_Pin|LED_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LS_1_Pin LS_2_Pin LS_9_Pin LS_10_Pin
                           LS_11_Pin LS_12_Pin BTN_UP_Pin BTN_DOWN_Pin
                           LS_4_Pin LS_5_Pin LS_6_Pin LS_7_Pin
                           LS_8_Pin */
  GPIO_InitStruct.Pin = LS_1_Pin|LS_2_Pin|LS_9_Pin|LS_10_Pin
                          |LS_11_Pin|LS_12_Pin|BTN_UP_Pin|BTN_DOWN_Pin
                          |LS_4_Pin|LS_5_Pin|LS_6_Pin|LS_7_Pin
                          |LS_8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
void assert_failed(uint8_t *file, uint32_t line)
{
  // User can add his own implementation to report the file name and line number
}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
