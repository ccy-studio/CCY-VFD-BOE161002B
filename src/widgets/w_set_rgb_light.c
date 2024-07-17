/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 17:18:01
 * @LastEditTime: 2024-07-17 09:46:12
 */
#include "widget.h"

// RGB显示的亮度值
u8 rgb_light = 0xff;

static void view_handler(void* params) {
    memset(vfd_buffer, 0, sizeof(vfd_buffer));
    sprintf(vfd_buffer, "BL: %d", rgb_light);
    vfd_gui_set_text(vfd_buffer, 0, 0);
}

/**
 * 处理按键的事件
 */
static void btn_click_event(btn_t* event) {
    if (event->btn_type == BTN_PRESS) {
        if (event->gpio_pin == K1_GPIO_PIN) {
            rgb_light += 51;
            if (rgb_light >= 204) {
                rgb_light = 255;
            }
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
            rgb_light -= 51;
            if (rgb_light == 0) {
                rgb_light = 1;
            }
        }

        if (event->gpio_pin == K3_GPIO_PIN) {
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 退出到设置页
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    }
}

widget_t w_set_rgb_light = {.name = WIDGET_NAME_SETTING_RGB_LIGHT,
                            .exec_time = 100,
                            .handler = view_handler,
                            .btn_callback = btn_click_event};