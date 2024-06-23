/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-11-02 11:23:32
 * @LastEditTime: 2024-06-03 10:21:37
 */
#ifndef __MSYS_H
#define __MSYS_H

#include <intrins.h>
#include "STC/STC8H.H"
#include "STDIO.H"
#include "STDLIB.H"
#include "STRING.H"

#define DEV_PLATFROM  // CDC虚拟串口开关

// #define SYS_FOSC 24000000UL  // 24Mhz

#ifndef DEV_PLATFROM
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
#include "stc32_stc8_usb.h"
#endif

typedef uint8_t bool;
#define false 0
#define true 1

#define I2C_SDA P24
#define I2C_SCL P25
#define RX8025_INT P26
#define VFD_EN P17
#define P_STB P01
#define P_CLK P02
#define P_DIN P03

typedef enum {
    BTN_P16 = 0,
    BTN_P15,
    BTN_P14,
} btn_gpio_t;

typedef enum {
    BTN_PRESS = 0,
    BTN_RELEASE,
    BTN_LONG,
} btn_state_t;

typedef struct {
    btn_gpio_t gpio;
    btn_state_t btn_type;
    u32 last_press_time;
    u8 falg;  // 是否被处理
} btn_t;
#define BTN_SCAN_MS 1
#define BTN_LONG_PRESS_MS 1000
#define BTN_LONG_DOUBLE_MS 500

void hal_init_systick();
u32 hal_systick_get();
void hal_init_uart(void);
void hal_init_all_gpio(void);
void hal_init_i2c(void);
void delay_ms(u32 ms);
void delay_us(u32 us);
u8 btn_gpio_read(btn_gpio_t gpio);

#endif