/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 16:52:49
 * @LastEditTime: 2024-07-16 17:16:35
 */
#include "widget.h"

extern rx8025_timeinfo timeinfo;

static rx8025_timeinfo set_timeinfo_cache;  // 设置timeinfo时的缓存
// 设置时间的时候的临时变量，记录当前设置值的最大和最小阈值范围
static u8 max, min;
static u8* set_clock_num_p;    // 记录时间设置的指针变量
static u8 set_clock_item = 1;  // 记录时间设置的时间项
static u8 flicker = 0;         // 闪烁标记

static void init() {
    memset(vfd_buffer, 0, size_t(vfd_buffer));
    flicker = !flicker;
}

static void view_time_handler(void* params) {
    init();
    sprintf(vfd_buffer, " %02d %02d %02d", set_timeinfo_cache.hour,
            set_timeinfo_cache.min, set_timeinfo_cache.sec);
    // 设置时间的
    if (set_clock_item == 1) {
        set_clock_num_p = &set_timeinfo_cache.hour;
        max = 23;
        min = 0;
        if (flicker) {
            memcpy(vfd_buffer + 1, "  ", 2);
        }
    } else if (set_clock_item == 2) {
        set_clock_num_p = &set_timeinfo_cache.min;
        max = 59;
        min = 0;
        if (flicker) {
            memcpy(vfd_buffer + 4, "  ", 2);
        }
    } else if (set_clock_item == 3) {
        set_clock_num_p = &set_timeinfo_cache.sec;
        max = 59;
        min = 0;
        if (flicker) {
            memcpy(vfd_buffer + 7, "  ", 2);
        }
    }
    vfd_gui_set_text(vfd_buffer, 0, 0);
}

static void view_date_handler(void* params) {
    init();
    // 设置日期的
    sprintf(vfd_buffer, "20%d%02d-%02d", set_timeinfo_cache.year,
            set_timeinfo_cache.month, set_timeinfo_cache.day);
    if (set_clock_item == 1) {
        set_clock_num_p = &set_timeinfo_cache.year;
        max = 99;
        min = 23;
        if (flicker) {
            memcpy(vfd_buffer + 2, "  ", 2);
        }
    } else if (set_clock_item == 2) {
        set_clock_num_p = &set_timeinfo_cache.month;
        max = 12;
        min = 1;
        if (flicker) {
            memcpy(vfd_buffer + 4, "  ", 2);
        }
    } else if (set_clock_item == 3) {
        set_clock_num_p = &set_timeinfo_cache.day;
        max = 31;
        min = 1;
        if (flicker) {
            memcpy(vfd_buffer + 7, "  ", 2);
        }
    }
    vfd_gui_set_text(vfd_buffer, 0, 0);
}

/**
 * 处理按键的事件
 */
static void btn_click_event(btn_t* event) {
    if (event->btn_type == BTN_PRESS) {
        if (event->gpio_pin == K1_GPIO_PIN) {
            if (*set_clock_num_p == 0 || (*set_clock_num_p - 1) < min) {
                *set_clock_num_p = max;
            } else {
                *set_clock_num_p -= 1;
            }
        }
        if (event->gpio_pin == K2_GPIO_PIN) {
            if ((*set_clock_num_p + 1) > max) {
                *set_clock_num_p = min;
            } else {
                *set_clock_num_p += 1;
            }
        }

        if (event->gpio_pin == K3_GPIO_PIN) {
            // 切换设置项
            set_clock_item++;
            if (set_clock_item > 3) {
                set_clock_item = 1;
            }
        }
    } else if (event->btn_type == BTN_LONG) {
        if (event->gpio_pin == K3_GPIO_PIN) {
            // 保存时间的设置
            rx8025_time_get(&timeinfo);
            if (curr_widget->name == WIDGET_NAME_SETTING_DATE) {
                // 设置日期保留时间
                set_timeinfo_cache.hour = timeinfo.hour;
                set_timeinfo_cache.min = timeinfo.min;
                set_timeinfo_cache.sec = timeinfo.sec;
            } else if (curr_widget->name == WIDGET_NAME_SETTING_TIME) {
                // 设置时间保留日期
                set_timeinfo_cache.year = timeinfo.year;
                set_timeinfo_cache.month = timeinfo.month;
                set_timeinfo_cache.day = timeinfo.day;
                set_timeinfo_cache.week = timeinfo.week;
            }
            // 保存日期到寄存器
            rx8025_set_time(set_timeinfo_cache.year, set_timeinfo_cache.month,
                            set_timeinfo_cache.day, set_timeinfo_cache.week - 1,
                            set_timeinfo_cache.hour, set_timeinfo_cache.min,
                            set_timeinfo_cache.sec);
            // 退出到设置页
            replace_widget(WIDGET_NAME_SETTING, NULL);
        }
    }
}

static void on_show(void* params) {
    rx8025_time_get(&timeinfo);
    memcpy(&set_timeinfo_cache, &timeinfo, sizeof(timeinfo));
    set_clock_item = 1;
}

widget_t w_vfd_set_date = {.name = WIDGET_NAME_SETTING_DATE,
                           .handler = view_date_handler,
                           .btn_callback = btn_click_event,
                           .call_show = on_show};

widget_t w_vfd_set_time = {.name = WIDGET_NAME_SETTING_TIME,
                           .handler = view_time_handler,
                           .btn_callback = btn_click_event,
                           .call_show = on_show};