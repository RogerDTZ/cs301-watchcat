/* USER CODE BEGIN Header */
/* clang-format off */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

uint32_t get_2hz_tick();
uint32_t get_50hz_tick();

void update_2hz(uint32_t delta);
void update_50hz(uint32_t delta);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RF24_IRQ_Pin GPIO_PIN_1
#define RF24_IRQ_GPIO_Port GPIOA
#define RF24_IRQ_EXTI_IRQn EXTI1_IRQn
#define RF24_CE_Pin GPIO_PIN_4
#define RF24_CE_GPIO_Port GPIOA
#define RF24_CSN_Pin GPIO_PIN_4
#define RF24_CSN_GPIO_Port GPIOC
#define KEY0_Pin GPIO_PIN_5
#define KEY0_GPIO_Port GPIOC
#define KEY0_EXTI_IRQn EXTI9_5_IRQn
#define IIC_SDA_Pin GPIO_PIN_11
#define IIC_SDA_GPIO_Port GPIOC
#define IIC_SCL_Pin GPIO_PIN_12
#define IIC_SCL_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
