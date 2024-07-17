/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 11:43:37
 * @LastEditTime: 2024-07-17 10:53:31
 */
#include "widget.h"

#define SCREENSAVERS_MS 300000  // 屏保执行时间 5*60*1000

extern rx8025_timeinfo timeinfo;

static u8 point = 0;
static const u8 light_level[3] = {1, 3, 7};
static u8 idx = 0;

static u32 last_screensavers_ms;  // 屏保
u8 acg_open = 1;                  // ACG动画开关

static void init() {
    rx8025_time_get(&timeinfo);
    memset(vfd_buffer, 0, sizeof(vfd_buffer));
    // 执行屏保程序
    if ((HAL_GetTick() - last_screensavers_ms) > SCREENSAVERS_MS) {
        vfd_gui_display_protect_exec();
        last_screensavers_ms = HAL_GetTick();
    }
}

static void time_handler(void* params) {
    init();
    formart_time(&timeinfo, vfd_buffer);
    point = !point;
    vfd_gui_set_text(vfd_buffer, point, 0);
}
static void date_handler(void* params) {
    init();
    formart_date(&timeinfo, vfd_buffer);
    vfd_gui_set_text(vfd_buffer, 0, 0);
}

static void hide_call(void* params) {
    vfd_gui_clear();
}

/**
 * 处理按键的事件
 */
static void btn_click_event(btn_t* event) {
    if (event->btn_type == BTN_PRESS) {
        if (event->gpio_pin == K1_GPIO_PIN) {
            // 调节显示日期和时间的切换
            if (curr_widget->name == WIDGET_NAME_VFD_DATE) {
                replace_widget(WIDGET_NAME_VFD_TIME, NULL);
            } else if (curr_widget->name == WIDGET_NAME_VFD_TIME) {
                replace_widget(WIDGET_NAME_VFD_DATE, NULL);
            }
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
            // 调节vfd亮度
            if (++idx >= sizeof(light_level)) {
                idx = 0;
            }
            vfd_gui_set_blk_level(light_level[idx]);
        }
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 开关acg动画
            acg_open = !acg_open;
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    }
}

widget_t w_vfd_time_def = {.name = WIDGET_NAME_VFD_TIME,
                           .exec_time = 500,
                           .handler = time_handler,
                           .call_hide = hide_call,
                           .btn_callback = btn_click_event};

widget_t w_vfd_date_def = {.name = WIDGET_NAME_VFD_DATE,
                           .exec_time = 500,
                           .handler = date_handler,
                           .call_hide = hide_call,
                           .btn_callback = btn_click_event};