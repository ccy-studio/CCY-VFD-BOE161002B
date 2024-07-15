/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-07-15 15:32:11
 * @LastEditTime: 2024-07-15 15:32:55
 */
#ifndef __WS2812
#define __WS2812

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"

#define WS2812_LED_COUNT 2  // WS2812B 
#define WS2812_RESET_PULSE 60
#define WS2812_BUFFER_SIZE (WS2812_LED_COUNT * 24 + WS2812_RESET_PULSE)

void rgb_set_color(u8 index, u8 r, u8 g, u8 b);
void rgb_update(u8 brightness);
void rgb_clear();
void rgb_frame_update(u8 brightness,u8 type);

#ifdef __cplusplus
}
#endif
#endif