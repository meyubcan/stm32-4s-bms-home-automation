/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AKIM_OUT_STM_Pin GPIO_PIN_0
#define AKIM_OUT_STM_GPIO_Port GPIOC
#define bat1_Pin GPIO_PIN_2
#define bat1_GPIO_Port GPIOA
#define bat2_Pin GPIO_PIN_3
#define bat2_GPIO_Port GPIOA
#define bat3_Pin GPIO_PIN_4
#define bat3_GPIO_Port GPIOA
#define bat4_Pin GPIO_PIN_5
#define bat4_GPIO_Port GPIOA
#define temp1_Pin GPIO_PIN_6
#define temp1_GPIO_Port GPIOA
#define temp2_Pin GPIO_PIN_7
#define temp2_GPIO_Port GPIOA
#define sarj_Pin GPIO_PIN_0
#define sarj_GPIO_Port GPIOB
#define HOME_LED_1_Pin GPIO_PIN_8
#define HOME_LED_1_GPIO_Port GPIOD
#define HOME_LED_2_Pin GPIO_PIN_9
#define HOME_LED_2_GPIO_Port GPIOD
#define HOME_FAN_1_Pin GPIO_PIN_10
#define HOME_FAN_1_GPIO_Port GPIOD
#define HOME_FAN_2_Pin GPIO_PIN_11
#define HOME_FAN_2_GPIO_Port GPIOD
#define HOME_TYPEC_5V_Pin GPIO_PIN_12
#define HOME_TYPEC_5V_GPIO_Port GPIOD
#define batarya_buzzer_flas_tetik_Pin GPIO_PIN_14
#define batarya_buzzer_flas_tetik_GPIO_Port GPIOD
#define batarya_fan_tetik_Pin GPIO_PIN_15
#define batarya_fan_tetik_GPIO_Port GPIOD
#define BLUETOOTH_RX_STM_TX_Pin GPIO_PIN_9
#define BLUETOOTH_RX_STM_TX_GPIO_Port GPIOA
#define BLUETOOTH_TX_STM_RX_Pin GPIO_PIN_10
#define BLUETOOTH_TX_STM_RX_GPIO_Port GPIOA
#define USER_LED_1_Pin GPIO_PIN_5
#define USER_LED_1_GPIO_Port GPIOD
#define USER_LED_2_Pin GPIO_PIN_6
#define USER_LED_2_GPIO_Port GPIOD
#define balans4_Pin GPIO_PIN_6
#define balans4_GPIO_Port GPIOB
#define balans3_Pin GPIO_PIN_7
#define balans3_GPIO_Port GPIOB
#define balans2_Pin GPIO_PIN_8
#define balans2_GPIO_Port GPIOB
#define balans1_Pin GPIO_PIN_9
#define balans1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
