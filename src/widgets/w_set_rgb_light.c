/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 17:18:01
 * @LastEditTime: 2024-07-16 17:19:26
 */
#include "widget.h"

static void view_handler(void* params) {}

/**
 * 处理按键的事件
 */
static void btn_click_event(btn_t* event) {
    if (event->btn_type == BTN_PRESS) {
        if (event->gpio_pin == K1_GPIO_PIN) {
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
        }

        if (event->gpio_pin == K3_GPIO_PIN) {
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 退出到设置页
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    }
}

widget_t w_vfd_setting = {.name = WIDGET_NAME_SETTING_RGB_LIGHT,
                          .handler = view_handler,
                          .btn_callback = btn_click_event};