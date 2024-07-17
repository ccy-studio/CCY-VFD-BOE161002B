/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-16 11:31:35
 * @LastEditTime: 2024-07-16 16:48:59
 */

#ifndef __WIDGET
#define __WIDGET

#ifdef __cplusplus
extern "C" {
#endif

#include "gui.h"
#include "rx8025t.h"
#include "sys.h"

typedef enum {
    WIDGET_NAME_VFD_TIME = 0,
    WIDGET_NAME_VFD_DATE,
    WIDGET_NAME_SETTING,
    WIDGET_NAME_SETTING_TIME,
    WIDGET_NAME_SETTING_DATE,
    WIDGET_NAME_SETTING_RGB,
    WIDGET_NAME_SETTING_RGB_LIGHT,
} widget_name;

typedef void (*widget_view_handler)(void* params);
typedef void (*widget_view_show)(void* params);
typedef void (*widget_view_hide)(void* params);
typedef void (*widget_btn_callback)(btn_t* event);

typedef struct {
    u8 name;
    u32 exec_time; //执行间隔ms
    widget_view_handler handler;
    widget_view_handler call_show;
    widget_view_handler call_hide;
    widget_btn_callback btn_callback;
} widget_t;

extern widget_t* curr_widget;
extern char vfd_buffer[10];

void widget_init();
void replace_widget(u8 name, void* params);
void widget_refresh(void* params);
void widget_send_btn_event(btn_t* event);

#ifdef __cplusplus
}
#endif
#endif