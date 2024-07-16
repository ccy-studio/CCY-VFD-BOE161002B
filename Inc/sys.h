/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-15 10:16:01
 * @LastEditTime: 2024-07-16 10:35:03
 */
#ifndef __SYSH
#define __SYSH

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "main.h"

typedef enum {
    BTN_PRESS = 0,
    BTN_RELEASE,
    BTN_LONG,
} btn_state_t;

typedef struct {
    uint16_t gpio_pin;
    btn_state_t btn_type;
    u32 last_press_time;
    u8 lock;  // 锁定状态
    u8 falg;  // 是否被处理
} btn_t;

/* 按键扫描配置 */
#define BTN_LONG_PRESS_MS 2000  // 长按间隔时间
#define BTN_SORT_PRESS_MS 500   // 短按间隔时间

void sys_gpio_init();
void sys_init_i2c();
void sys_init_pwm();
void sys_init_rgb();
void sys_open_power();
void sys_close_power();
void sys_btn_release(btn_t* btn);
void delay_us(u32 us);
void delay_ms(u32 ms);

#ifdef __cplusplus
}
#endif

#endif