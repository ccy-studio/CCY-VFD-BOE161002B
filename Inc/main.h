/**
  ******************************************************************************
  * @file    main.h
  * @author  MCU Application Team
  * @brief   Header for main.c file.
  *          This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t



#define VFD_EN_GPIO_PORT GPIOC
#define VFD_EN_GPIO_PIN GPIO_PIN_1

#define RGB_GPIO_PORT GPIOB
#define RGB_GPIO_PIN GPIO_PIN_7

#define KEY_GPIO_PORT GPIOA
#define K1_GPIO_PIN GPIO_PIN_5
#define K2_GPIO_PIN GPIO_PIN_6
#define K3_GPIO_PIN GPIO_PIN_7

#define PT_STB_GPIO_PORT GPIOA
#define PT_STB_GPIO_PIN GPIO_PIN_1
#define PT_DIN_GPIO_PORT GPIOA
#define PT_DIN_GPIO_PIN GPIO_PIN_0
#define PT_CLK_GPIO_PORT GPIOB
#define PT_CLK_GPIO_PIN GPIO_PIN_0

#define PWM_GPIO_PORT GPIOB
#define PWM_GPIO_PIN GPIO_PIN_1

#define RX8025_INT_GPIO_PORT GPIOB
#define RX8025_INT_GPIO_PIN GPIO_PIN_2

#define I2C_GPIO_PORT GPIOB
#define I2C_SCL_GPIO_PIN GPIO_PIN_3
#define I2C_SDA_GPIO_PIN GPIO_PIN_4
/* Exported variables prototypes ---------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void APP_ErrorHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
