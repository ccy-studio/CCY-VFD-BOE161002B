/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 17:18:01
 * @LastEditTime: 2024-07-16 17:55:33
 */
#include "widget.h"
#include "ws2812.h"

static const char* types[4] = {"Type-1", "Type-2", "Type-3", "Close"};
static u8 idx = 0;

// RGB的显示效果类型设定值
u8 rgb_type = 0;

static void view_handler(void* params) {
    memset(vfd_buffer, 0, sizeof(vfd_buffer));
    memcpy(vfd_buffer, types[idx], strlen(types[idx]));
    vfd_gui_set_text(vfd_buffer, 0, 0);
}

/**
 * 处理按键的事件
 */
static void btn_click_event(btn_t* event) {
    if (event->btn_type == BTN_PRESS) {
        if (event->gpio_pin == K1_GPIO_PIN) {
            if (idx != 0) {
                idx--;
            } else {
                idx = sizeof(types) / sizeof(types[0]) - 1;
            }
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
            if (++idx >= (sizeof(types) / sizeof(types[0]))) {
                idx = 0;
            }
        }

        if (event->gpio_pin == K3_GPIO_PIN) {
            // 确认键切换RGB的显示
            rgb_type = idx;
            if (idx == (sizeof(types) / sizeof(types[0]) - 1)) {
                rgb_clear();
            }
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 退出到设置页
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    }
}

widget_t w_set_rgb = {.name = WIDGET_NAME_SETTING_RGB,
                      .exec_time = 100,
                      .handler = view_handler,
                      .btn_callback = btn_click_event};