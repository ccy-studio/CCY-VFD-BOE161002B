#include "dht20.h"
#include "gui.h"
#include "rx8025.h"

/**
 * 0x0n 一级页面
 * 0x1x 二级页面(设置子页面)
 */
#define PAGE_FLAG_CLOCK_TIME 0x00       // 时间显示页面
#define PAGE_FLAG_CLOCK_DATE 0x01       // 日期显示页面
#define PAGE_FLAG_SENSOR_TEMP 0x02      // 温度传感器显示页面
#define PAGE_FLAG_SENSOR_HUMIDITY 0x03  // 湿度传感器显示页面
#define PAGE_SUB_FLAG_SET_CLOCK 0x10    // 设置时间子页面

#define PAGE_COUNT PAGE_FLAG_SENSOR_HUMIDITY  // 主页面最大数量

/**
 * 屏幕保护程序间隔执行时间,单位毫秒默认2分钟一次
 */
#define SCREEN_SAVER_TIME 120000UL
// #define SCREEN_SAVER_TIME 10000UL

u8 vfd_brightness_level[3] = {1, 2, 7};  // VFD亮度等级
u8 vfd_brightness = 2;                   // VFD亮度等级Level下标
bool acg_open = true;                    // vfd动画特效开关
bool vfd_saver_open = true;              // vfd屏幕保护程序开关

u8 data page_display_flag = PAGE_FLAG_CLOCK_TIME;  // 页面显示内容
u8 last_page_display_flag;                         // 上次页面显示内容
rx8025_timeinfo data timeinfo;  // 主动态更新的时间对象

rx8025_timeinfo set_timeinfo_cache;  // 设置timeinfo时的缓存
u8 max, min;  // 设置时间的时候的临时变量，记录当前设置值的最大和最小阈值范围
bool save_timeinfo_flag = false;  // 触发保存时间的操作flag
u8 data buffer[12];               // vfd显示缓存
bool point_flag = 0;              // vfd冒号显示状态

u8* set_clock_num_p;    // 记录时间设置的指针变量
u8 set_clock_item = 1;  // 记录时间设置的时间项

// 时间等待记录变量
u32 data time_wait_count;
u32 data page_wait_count;
u32 data acg_wait_count;
u32 data saver_wait_count;
u32 data dht20_wait_count;

static dht20_val dht20;

/**
 * 按钮定义
 */
static btn_t data btn_arr[3] = {
    {BTN_P14, BTN_RELEASE, 0, 0},
    {BTN_P15, BTN_RELEASE, 0, 0},
    {BTN_P16, BTN_RELEASE, 0, 0},
};

extern u32 data _systick_ccr;  // 系统滴答定时器走时

bool interval_check(u32 select, u32 t);  // 检查时间是否满足条件
void page_home();
void page_sensor();
void page_sub_set_datetime();

void main() {
    P_SW2 |= 0x80;  // 使能EAXFR寄存器 XFR
    hal_init_all_gpio();
    hal_init_uart();
    hal_init_systick();
    hal_init_i2c();
    vfd_gui_init();

    // 开启按键扫描定时器 1毫秒@24MHz
    AUXR |= 0x40;  // 定时器时钟1T模式
    TMOD &= 0x0F;  // 设置定时器模式
    TL1 = 0x40;    // 设置定时初始值
    TH1 = 0xA2;    // 设置定时初始值
    TF1 = 0;       // 清除TF1标志
    TR1 = 1;       // 定时器1开始计时
    ET1 = 1;       // 使能定时器1中断

    vfd_gui_set_text("Starting", 0, 1);
    vfd_gui_set_dig(9, 0xffffff);
    delay_ms(500);
    dht20_init();
    dht20_read(&dht20);
    delay_ms(1000);
    vfd_gui_clear();
    while (true) {
#ifdef DEV_PLATFROM
        if (bUsbOutReady) {
            usb_OUT_done();
        }
#endif
        //     //====>>>>>>按钮的触发扫描 -> Start
        if (btn_arr[0].falg) {
            // P33
            if (btn_arr[0].btn_type == BTN_PRESS) {
                if (page_display_flag != PAGE_SUB_FLAG_SET_CLOCK) {
                    // 当页面不是时间设置页的时候，切换页面显示内容
                    if (page_display_flag + 1 > PAGE_COUNT) {
                        page_display_flag = 0;
                    } else {
                        page_display_flag++;
                    }
                } else {
                    if (*set_clock_num_p == 0 || (*set_clock_num_p - 1) < min) {
                        *set_clock_num_p = max;
                    } else {
                        *set_clock_num_p -= 1;
                    }
                }
            } else if (btn_arr[0].btn_type == BTN_LONG) {
            }
            btn_arr[0].falg = 0;
        }
        if (btn_arr[1].falg) {
            // P34
            if (btn_arr[1].btn_type == BTN_PRESS) {
                if (page_display_flag == PAGE_SUB_FLAG_SET_CLOCK) {
                    if ((*set_clock_num_p + 1) > max) {
                        *set_clock_num_p = min;
                    } else {
                        *set_clock_num_p += 1;
                    }
                } else {
                    // 亮度调整
                    vfd_brightness = (vfd_brightness + 1) % 3;
                }
            } else if (btn_arr[1].btn_type == BTN_LONG) {
                IAP_CONTR = 0x60;  // 软复位到ISP进行下载
            }
            btn_arr[1].falg = 0;
        }
        if (btn_arr[2].falg) {
            // P35
            if (btn_arr[2].btn_type == BTN_PRESS) {
                if (page_display_flag == PAGE_SUB_FLAG_SET_CLOCK) {
                    set_clock_item++;
                    if (set_clock_item > 3) {
                        set_clock_item = 1;
                    }
                } else {
                    acg_open = !acg_open;
                }
            } else if (btn_arr[2].btn_type == BTN_LONG) {
                if (page_display_flag == PAGE_SUB_FLAG_SET_CLOCK) {
                    page_display_flag = PAGE_FLAG_CLOCK_TIME;
                    save_timeinfo_flag = true;
                    memset(buffer, 0, sizeof(buffer));
                } else {
                    last_page_display_flag = page_display_flag;
                    page_display_flag = PAGE_SUB_FLAG_SET_CLOCK;
                    set_clock_item = 1;
                    memcpy(&set_timeinfo_cache, &timeinfo, sizeof(timeinfo));
                }
            }
            btn_arr[2].falg = 0;
        }
        //====>>>>>>按钮的触发扫描 -> End

        // 动态亮度调整
        vfd_gui_set_blk_level(vfd_brightness_level[vfd_brightness]);

        if (page_display_flag == PAGE_FLAG_CLOCK_TIME ||
            page_display_flag == PAGE_FLAG_CLOCK_DATE) {
            if (interval_check(time_wait_count, 500)) {
                page_home();
                time_wait_count = _systick_ccr;
            }
        } else if (page_display_flag == PAGE_SUB_FLAG_SET_CLOCK) {
            // 时间设置
            if (interval_check(page_wait_count, 300)) {
                page_sub_set_datetime();
                page_wait_count = _systick_ccr;
            }
        } else if (page_display_flag == PAGE_FLAG_SENSOR_TEMP ||
                   page_display_flag == PAGE_FLAG_SENSOR_HUMIDITY) {
            // 显示温湿度
            if (interval_check(time_wait_count, 500)) {
                page_sensor();
                time_wait_count = _systick_ccr;
            }
        }

        // 时间保存设定
        if (save_timeinfo_flag) {
            if (last_page_display_flag == PAGE_FLAG_CLOCK_DATE) {
                // 设置日期保留时间
                set_timeinfo_cache.hour = timeinfo.hour;
                set_timeinfo_cache.min = timeinfo.min;
                set_timeinfo_cache.sec = timeinfo.sec;
            } else {
                // 设置时间保留日期
                set_timeinfo_cache.year = timeinfo.year;
                set_timeinfo_cache.month = timeinfo.month;
                set_timeinfo_cache.day = timeinfo.day;
                set_timeinfo_cache.week = timeinfo.week;
            }
            rx8025_set_time(set_timeinfo_cache.year, set_timeinfo_cache.month,
                            set_timeinfo_cache.day, set_timeinfo_cache.week - 1,
                            set_timeinfo_cache.hour, set_timeinfo_cache.min,
                            set_timeinfo_cache.sec);
            memset(&set_timeinfo_cache, 0x00, sizeof(set_timeinfo_cache));
            save_timeinfo_flag = false;
        }

        // vfd特效动画
        if (acg_open && page_display_flag != PAGE_SUB_FLAG_SET_CLOCK) {
            if (interval_check(acg_wait_count, 150)) {
                vfd_gui_acg_update();
                acg_wait_count = _systick_ccr;
            }
        }

        // 屏幕保护程序
        if (vfd_saver_open && page_display_flag != PAGE_SUB_FLAG_SET_CLOCK) {
            if (interval_check(saver_wait_count, SCREEN_SAVER_TIME)) {
                vfd_gui_display_protect_exec();
                saver_wait_count = _systick_ccr;
            }
        }

        if (interval_check(dht20_wait_count, 1000)) {
            dht20_read(&dht20);
            dht20_wait_count = _systick_ccr;
        }
    }
}

bool interval_check(u32 select, u32 t) {
    return select > _systick_ccr || (_systick_ccr - select) >= t;
}

void page_home() {
    rx8025_time_get(&timeinfo);
    memset(buffer, 0, sizeof(buffer));
    if (page_display_flag == PAGE_FLAG_CLOCK_TIME) {
        point_flag = !point_flag;
        formart_time(&timeinfo, &buffer);
    } else if (page_display_flag == PAGE_FLAG_CLOCK_DATE) {
        point_flag = 1;
        formart_date(&timeinfo, &buffer);
    }
    vfd_gui_set_text(buffer, point_flag, 0);
}

/**
 * 显示传感器内容
 */
void page_sensor() {
    memset(buffer, 0, sizeof(buffer));
    if (page_display_flag == PAGE_FLAG_SENSOR_TEMP) {
        sprintf(buffer, "T-%.4f", dht20.tmep);
    } else {
        sprintf(buffer, "H-%.3f%%", dht20.humidity);
    }
    vfd_gui_set_text(buffer, 0, 0);
}

/**
 * 子页面-时间设定
 */
void page_sub_set_datetime() {
    static u8 flicker = 0;  // 闪烁标记
    flicker = !flicker;
    memset(buffer, 0x00, sizeof(buffer));
    if (last_page_display_flag == PAGE_FLAG_CLOCK_TIME) {
        sprintf(buffer, " %02bd %02bd %02bd", set_timeinfo_cache.hour,
                set_timeinfo_cache.min, set_timeinfo_cache.sec);
        // 设置时间的
        if (set_clock_item == 1) {
            set_clock_num_p = &set_timeinfo_cache.hour;
            max = 23;
            min = 0;
            if (flicker) {
                memcpy(buffer + 1, "  ", 2);
            }
        } else if (set_clock_item == 2) {
            set_clock_num_p = &set_timeinfo_cache.min;
            max = 59;
            min = 0;
            if (flicker) {
                memcpy(buffer + 4, "  ", 2);
            }
        } else if (set_clock_item == 3) {
            set_clock_num_p = &set_timeinfo_cache.sec;
            max = 59;
            min = 0;
            if (flicker) {
                memcpy(buffer + 7, "  ", 2);
            }
        }
    } else if (last_page_display_flag == PAGE_FLAG_CLOCK_DATE) {
        // 设置日期的
        sprintf(buffer, "20%bd%02bd-%02bd", set_timeinfo_cache.year,
                set_timeinfo_cache.month, set_timeinfo_cache.day);
        if (set_clock_item == 1) {
            set_clock_num_p = &set_timeinfo_cache.year;
            max = 99;
            min = 23;
            if (flicker) {
                memcpy(buffer + 2, "  ", 2);
            }
        } else if (set_clock_item == 2) {
            set_clock_num_p = &set_timeinfo_cache.month;
            max = 12;
            min = 1;
            if (flicker) {
                memcpy(buffer + 4, "  ", 2);
            }
        } else if (set_clock_item == 3) {
            set_clock_num_p = &set_timeinfo_cache.day;
            max = 31;
            min = 1;
            if (flicker) {
                memcpy(buffer + 7, "  ", 2);
            }
        }
    }
    acg_open = false;
    vfd_gui_set_text(buffer,
                     last_page_display_flag == PAGE_FLAG_CLOCK_DATE ? 0 : 1, 0);
}

void btn_handler(btn_t* btn) {
    if (btn->last_press_time >= BTN_LONG_PRESS_MS &&
        btn->btn_type == BTN_PRESS) {
        btn->last_press_time += BTN_SCAN_MS;
        return;
    }
    if (!btn_gpio_read(btn->gpio)) {
        // 按下触发
        if (btn->btn_type == BTN_PRESS) {
            btn->last_press_time += BTN_SCAN_MS;
        } else if (btn->btn_type == BTN_RELEASE) {
            btn->last_press_time = 0;
            btn->btn_type = BTN_PRESS;
            // send message
            btn->falg = 1;
            return;
        }
        if (btn->btn_type == BTN_PRESS &&
            btn->last_press_time >= BTN_LONG_PRESS_MS) {
            // long press event
            btn->btn_type = BTN_LONG;
            btn->last_press_time += BTN_SCAN_MS;
            // send message
            btn->falg = 1;
            return;
        }
    } else {
        // release
        btn->btn_type = BTN_RELEASE;
        btn->last_press_time = 0;
    }
}

void btn_scan_isr(void) interrupt 3 {
    // P33- P34+ P35M
    btn_handler(&btn_arr[0]);
    btn_handler(&btn_arr[1]);
    btn_handler(&btn_arr[2]);
    TF1 = 0;  // 清除TF1标志
}