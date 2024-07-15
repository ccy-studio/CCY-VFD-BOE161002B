/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-15 10:21:14
 * @LastEditTime: 2024-07-15 15:57:03
 */
#ifndef __PT6315__
#define __PT6315__

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

#define CLK_1 HAL_GPIO_WritePin(PT_CLK_GPIO_PORT, PT_CLK_GPIO_PIN, 1)
#define CLK_0 HAL_GPIO_WritePin(PT_CLK_GPIO_PORT, PT_CLK_GPIO_PIN, 0)
#define DIN_1 HAL_GPIO_WritePin(PT_DIN_GPIO_PORT, PT_DIN_GPIO_PIN, 1)
#define DIN_0 HAL_GPIO_WritePin(PT_DIN_GPIO_PORT, PT_DIN_GPIO_PIN, 0)
#define STB_1 HAL_GPIO_WritePin(PT_STB_GPIO_PORT, PT_STB_GPIO_PIN, 1)
#define STB_0 HAL_GPIO_WritePin(PT_STB_GPIO_PORT, PT_STB_GPIO_PIN, 0)

void ptSetDisplayLight(uint8_t onOff, uint8_t brightnessVal);
void setModeWirteDisplayMode(uint8_t addressMode);
void setDisplayMode(uint8_t digit);
void sendDigAndData(uint8_t dig, const uint8_t* dat, size_t len);

#ifdef __cplusplus
}
#endif
#endif