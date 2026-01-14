/* USER CODE BEGIN Header */
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
#include "stm32h7xx_hal.h"

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
#define EEWC_Pin GPIO_PIN_13
#define EEWC_GPIO_Port GPIOC
#define CODECRESET_Pin GPIO_PIN_14
#define CODECRESET_GPIO_Port GPIOC
#define POT10_Pin GPIO_PIN_0
#define POT10_GPIO_Port GPIOC
#define POT1_Pin GPIO_PIN_1
#define POT1_GPIO_Port GPIOC
#define CODECCHIPSELECT_Pin GPIO_PIN_2
#define CODECCHIPSELECT_GPIO_Port GPIOC
#define POT2_Pin GPIO_PIN_1
#define POT2_GPIO_Port GPIOA
#define POT3_Pin GPIO_PIN_2
#define POT3_GPIO_Port GPIOA
#define POT4_Pin GPIO_PIN_3
#define POT4_GPIO_Port GPIOA
#define POT5_Pin GPIO_PIN_5
#define POT5_GPIO_Port GPIOA
#define EXP_Pin GPIO_PIN_6
#define EXP_GPIO_Port GPIOA
#define POT6_Pin GPIO_PIN_7
#define POT6_GPIO_Port GPIOA
#define POT9_Pin GPIO_PIN_4
#define POT9_GPIO_Port GPIOC
#define POT7_Pin GPIO_PIN_5
#define POT7_GPIO_Port GPIOC
#define POT8_Pin GPIO_PIN_0
#define POT8_GPIO_Port GPIOB
#define LEDBYPASS_Pin GPIO_PIN_1
#define LEDBYPASS_GPIO_Port GPIOB
#define LFOTACT3_Pin GPIO_PIN_2
#define LFOTACT3_GPIO_Port GPIOB
#define LFOTACT2_Pin GPIO_PIN_7
#define LFOTACT2_GPIO_Port GPIOE
#define LFOTACT1_Pin GPIO_PIN_8
#define LFOTACT1_GPIO_Port GPIOE
#define PRESETTACT_Pin GPIO_PIN_9
#define PRESETTACT_GPIO_Port GPIOE
#define LEDP1_Pin GPIO_PIN_10
#define LEDP1_GPIO_Port GPIOE
#define LEDP2_Pin GPIO_PIN_11
#define LEDP2_GPIO_Port GPIOE
#define LEDP3_Pin GPIO_PIN_12
#define LEDP3_GPIO_Port GPIOE
#define FS1_Pin GPIO_PIN_14
#define FS1_GPIO_Port GPIOE
#define FS2_Pin GPIO_PIN_10
#define FS2_GPIO_Port GPIOB
#define LEDEXTRA_Pin GPIO_PIN_13
#define LEDEXTRA_GPIO_Port GPIOB
#define DEEMPHASISCONTROL_Pin GPIO_PIN_11
#define DEEMPHASISCONTROL_GPIO_Port GPIOD
#define CODECFITLERSELECT_Pin GPIO_PIN_12
#define CODECFITLERSELECT_GPIO_Port GPIOD
#define DOUBLESPEEDMODE_Pin GPIO_PIN_13
#define DOUBLESPEEDMODE_GPIO_Port GPIOD
#define CODECPARALLELSERIAL_Pin GPIO_PIN_14
#define CODECPARALLELSERIAL_GPIO_Port GPIOD
#define MISORELAY1_Pin GPIO_PIN_10
#define MISORELAY1_GPIO_Port GPIOC
#define MISORELAY12_Pin GPIO_PIN_11
#define MISORELAY12_GPIO_Port GPIOC
#define RIGHTRELAY1_Pin GPIO_PIN_0
#define RIGHTRELAY1_GPIO_Port GPIOD
#define RIGHTRELAY12_Pin GPIO_PIN_1
#define RIGHTRELAY12_GPIO_Port GPIOD
#define LEFTRELAY1_Pin GPIO_PIN_3
#define LEFTRELAY1_GPIO_Port GPIOD
#define LEFTRELAY12_Pin GPIO_PIN_4
#define LEFTRELAY12_GPIO_Port GPIOD
#define LFOLED1_Pin GPIO_PIN_5
#define LFOLED1_GPIO_Port GPIOD
#define LFOLED2_Pin GPIO_PIN_6
#define LFOLED2_GPIO_Port GPIOD
#define LFOLED3_Pin GPIO_PIN_7
#define LFOLED3_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
