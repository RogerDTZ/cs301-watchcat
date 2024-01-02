/* USER CODE BEGIN Header */
/* clang-format off */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* clang-format on */

#include <assert.h>

#include "SYSTEM/delay/delay.h"

#include "BSP/24C02/24cxx.h"
#include "BSP/ATK_MD0280/atk_md0280_touch.h"
#include "BSP/LED/led.h"
#include "BSP/NRF24L01/24l01.h"
#include "lvgl.h"
#include "sl_ui/ui.h"

#include "app/album.h"
#include "app/calc.h"
#include "app/chat.h"
#include "app/home.h"
#include "port/input_dev.h"
#include "port/lvgl_ctrl.h"

/* clang-format off */
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

/* USER CODE BEGIN PV */

// ms, updated every 500 ms
static uint32_t tick_2hz;
// ms, updated every 20 ms
static uint32_t tick_50hz;

static uint16_t probed_touch_point_x;
static uint16_t probed_touch_point_y;
static bool probed_touch_pressed;

extern uint8_t rx_buffer[2];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* clang-format on */

void update_2hz(uint32_t delta)
{
  assert(delta == 500);
  tick_2hz += delta;

  chat_update(delta);
  update_time();
}

void update_50hz(uint32_t delta)
{
  assert(delta == 20);
  tick_50hz += delta;

  lv_tick_inc(delta);
}

uint32_t get_2hz_tick() { return tick_2hz; }

uint32_t get_50hz_tick() { return tick_50hz; }

void touch_indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
  data->point.x = (lv_coord_t)probed_touch_point_x;
  data->point.y = (lv_coord_t)probed_touch_point_y;
  data->state = probed_touch_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

static void custom_ui_init() {}

/* clang-format off */
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
  /* clang-format on */

  // Initialize the delay scale in the SYSTEM library
  const uint32_t pclk2_freq = HAL_RCC_GetPCLK2Freq();
  assert(pclk2_freq % 1000000 == 0);
  delay_init(pclk2_freq / 1000000);

  /* clang-format off */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* clang-format on */

  // Initialize LEDs
  led_init();
  LED0(1);
  LED1(1);

  // Initialize EEPROM
  AT24CXX_Init();
  while (AT24CXX_Check()) {
    // Indicate a failure upon checking the EEPROM
    LED0_TOGGLE();
    HAL_Delay(300);
    LED0_TOGGLE();
    HAL_Delay(300);
  }

  // Initialize NRF24L01 2.4 Ghz
  NRF24L01_Init();
  while (NRF24L01_Check()) {
    // Indicate a failure upon checking the NRF24L01
    LED1_TOGGLE();
    HAL_Delay(300);
    LED1_TOGGLE();
    HAL_Delay(300);
  }

  // Initialize lvgl library
  lv_init();
  // Initialize LCD & connect lvgl to LCD
  lv_port_disp_init();
  // Initialize all input devices
  init_lvgl_input_devices();
  // Init SquareLine ui
  ui_init();
  custom_ui_init();

  // Enable TIM2: 50 Hz
  HAL_TIM_Base_Start_IT(&htim2);

  // Enable TIM3: 2 Hz
  HAL_TIM_Base_Start_IT(&htim3);

  // Initialize HAL Transmit
  HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_buffer, 1);

  // open_app_calc();
  // open_app_chat();
  // open_app_album();

  /* clang-format off */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* clang-format on */

    // Probe input devices
    probed_touch_pressed =
        atk_md0280_touch_scan(&probed_touch_point_x, &probed_touch_point_y) ==
        ATK_MD0280_TOUCH_EOK;

    // Call lvgl's update handler
    // Then, delay for the required period
    HAL_Delay(lv_timer_handler());
  }
  /* clang-format off */
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

#ifdef  USE_FULL_ASSERT
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
