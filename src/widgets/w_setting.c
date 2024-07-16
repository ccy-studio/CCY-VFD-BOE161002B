#include "widget.h"

typedef struct {
    const char title[7];
    u8 open_widget_name;
    u8 active;
} menu_item_t;

static const menu_item_t menu_items[4] = {
    {.title = "set time", .open_widget_name = WIDGET_NAME_SETTING_TIME},
    {.title = "set date", .open_widget_name = WIDGET_NAME_SETTING_DATE},
    {.title = "rgb type", .open_widget_name = WIDGET_NAME_SETTING_RGB},
    {.title = "rgb bl", .open_widget_name = WIDGET_NAME_SETTING_RGB_LIGHT},
};

static u8 idx = 0;

static void menu_handler(void* params) {
    memset(vfd_buffer, 0, size_t(vfd_buffer));
    memcpy(vfd_buffer, menu_items[idx].title, sizeof(menu_items[idx].title));
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
                idx = sizeof(menu_items) / sizeof(menu_items[0]) - 1;
            }
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
            if (++idx >= (sizeof(menu_items) / sizeof(menu_items[0]))) {
                idx = 0;
            }
        }

        if (event->gpio_pin == K3_GPIO_PIN) {
            // 进入设置页
            replace_widget(menu_items[idx].open_widget_name, NULL);
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 退出设置页
            replace_widget(WIDGET_NAME_VFD_TIME, NULL);
        }
    }
}

widget_t w_vfd_setting = {.name = WIDGET_NAME_SETTING,
                          .handler = menu_handler,
                          .btn_callback = btn_click_event};