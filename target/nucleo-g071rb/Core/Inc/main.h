/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32g0xx_hal.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define STPM1_CSN_Pin GPIO_PIN_0
#define STPM1_CSN_GPIO_Port GPIOC
#define STPM_SYN_Pin GPIO_PIN_1
#define STPM_SYN_GPIO_Port GPIOC
#define S2LP_GPIO0_Pin GPIO_PIN_0
#define S2LP_GPIO0_GPIO_Port GPIOA
#define S2LP_CSN_Pin GPIO_PIN_1
#define S2LP_CSN_GPIO_Port GPIOA
#define S2LP_GPIO1_Pin GPIO_PIN_4
#define S2LP_GPIO1_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_5
#define USER_LED_GPIO_Port GPIOA
#define S2LP_GPIO2_Pin GPIO_PIN_1
#define S2LP_GPIO2_GPIO_Port GPIOB
#define S2LP_GPIO3_Pin GPIO_PIN_12
#define S2LP_GPIO3_GPIO_Port GPIOB
#define S2LP_GPIO3_EXTI_IRQn EXTI4_15_IRQn
#define S2LP_SDN_Pin GPIO_PIN_8
#define S2LP_SDN_GPIO_Port GPIOA
#define EEPROM_CSN_Pin GPIO_PIN_9
#define EEPROM_CSN_GPIO_Port GPIOA
#define RADIO_LED_Pin GPIO_PIN_7
#define RADIO_LED_GPIO_Port GPIOC
#define STPM1_EN_Pin GPIO_PIN_11
#define STPM1_EN_GPIO_Port GPIOA
void   MX_TIM3_Init(void);
void   MX_TIM15_Init(void);
/* USER CODE BEGIN Private defines */

// Only dev. board has the "User Button", so use it to determine that we are on the dev. board
#ifdef USER_LED_GPIO_Port
#define DEV_BOARD 1
#else
#define PROTO_BOARD 1
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
