#include "widget.h"

#define WIDGET_SIZE 3

rx8025_timeinfo timeinfo;
u8 vfd_buffer[10];  // vfd显示缓存

widget_t widget_defs[WIDGET_SIZE] = {};
widget_t* curr_widget = NULL;

widget_t* find_widget(u8 name) {
    for (u8 i = 0; i < WIDGET_SIZE; i++) {
        if (widget_defs[i].name == name) {
            return &widget_defs[i];
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
}

void widget_init() {
    replace_widget(WIDGET_NAME_VFD_TIME, NULL);
}

/**
 * 刷新
 */
void widget_refresh(void* params) {
    if (curr_widget != NULL && curr_widget->handler != NULL) {
        curr_widget->handler(params);
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