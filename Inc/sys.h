#ifndef __SYSH
#define __SYSH

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

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


void sys_gpio_init();
void sys_init_i2c();
void sys_init_pwm();
void sys_open_power();
void sys_close_power();

#ifdef __cplusplus
}
#endif

#endif