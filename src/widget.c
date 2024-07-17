/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 11:39:04
 * @LastEditTime: 2024-07-17 10:52:47
 */
#include "widget.h"

#define WIDGET_SIZE 7

rx8025_timeinfo timeinfo;
char vfd_buffer[10];  // vfd显示缓存
u32 last_time;

widget_t* curr_widget = NULL;

// 定义初始化组件
extern widget_t w_vfd_time_def;
extern widget_t w_vfd_date_def;
extern widget_t w_vfd_setting;
extern widget_t w_set_rgb;
extern widget_t w_set_rgb_light;
extern widget_t w_vfd_set_date;
extern widget_t w_vfd_set_time;
widget_t* widget_defs[WIDGET_SIZE] = {
    &w_vfd_time_def,  &w_vfd_date_def, &w_vfd_setting, &w_set_rgb,
    &w_set_rgb_light, &w_vfd_set_date, &w_vfd_set_time};

widget_t* find_widget(u8 name) {
    for (u8 i = 0; i < WIDGET_SIZE; i++) {
        if (widget_defs[i]->name == name) {
            return widget_defs[i];
        }
    }
    return NULL;
}

/**
 * 切换组件显示
 */
void replace_widget(u8 name, void* params) {
    widget_t* wd = find_widget(name);
    if (wd == NULL) {
        APP_ErrorHandler();
        return;
    }
    if (curr_widget == wd) {
        return;
    }
    if (curr_widget != NULL && curr_widget->call_hide != NULL) {
        curr_widget->call_hide(params);
    }
    if (wd->call_show != NULL) {
        wd->call_show(params);
    }
    curr_widget = wd;
    last_time = 0;
}

void widget_init() {
    replace_widget(WIDGET_NAME_VFD_TIME, NULL);
}

/**
 * 刷新
 */
void widget_refresh(void* params) {
    if (curr_widget != NULL && curr_widget->handler != NULL) {
        // 时间执行的判断
        if ((HAL_GetTick() - last_time) > curr_widget->exec_time) {
            curr_widget->handler(params);
            last_time = HAL_GetTick();
        }
    }
}

/**
 * 发送按键事件到指定的组件
 */
void widget_send_btn_event(btn_t* event) {
    if (curr_widget != NULL && curr_widget->btn_callback != NULL) {
        curr_widget->btn_callback(event);
    }
}