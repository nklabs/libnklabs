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
#include "stm32f7xx_hal.h"

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
#define MAIN_I2C_SCL_Pin GPIO_PIN_8
#define MAIN_I2C_SCL_GPIO_Port GPIOB
#define MAIN_SPI_CS_L_1_Pin GPIO_PIN_15
#define MAIN_SPI_CS_L_1_GPIO_Port GPIOA
#define MAIN_I2C_SDA_Pin GPIO_PIN_9
#define MAIN_I2C_SDA_GPIO_Port GPIOB
#define CONSOLE_RX_Pin GPIO_PIN_7
#define CONSOLE_RX_GPIO_Port GPIOB
#define USER_LED_Pin GPIO_PIN_1
#define USER_LED_GPIO_Port GPIOI
#define CONSOLE_TX_Pin GPIO_PIN_9
#define CONSOLE_TX_GPIO_Port GPIOA
#define USER_BUTTON_Pin GPIO_PIN_11
#define USER_BUTTON_GPIO_Port GPIOI
#define USER_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define MAIN_SPI_CS_L_Pin GPIO_PIN_8
#define MAIN_SPI_CS_L_GPIO_Port GPIOA
#define MAIN_SPI_CS_L_2_Pin GPIO_PIN_6
#define MAIN_SPI_CS_L_2_GPIO_Port GPIOG
#define MAIN_SPI_MISO_Pin GPIO_PIN_14
#define MAIN_SPI_MISO_GPIO_Port GPIOB
#define MAIN_SPI_MOSI_Pin GPIO_PIN_15
#define MAIN_SPI_MOSI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

// Name the devices we use

#define MAIN_CONSOLE_UART huart1
extern UART_HandleTypeDef huart1;

#define ARD_SPI hspi2
extern SPI_HandleTypeDef hspi2;

#define ARD_I2C hi2c1
extern I2C_HandleTypeDef hi2c1;

#define MAIN_WDT hiwdg
extern IWDG_HandleTypeDef hiwdg;

// Demo app functions called by main.h

void user_main(void);

// Called by stm32f0xx_it.c

void nk_uart_irq_handler(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
