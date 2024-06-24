#include "gui.h"

#define DIG_POINT 0x008000
#define DIG_ICON 0x004000

u8 lightLevel = 1;  // 亮度级别
static u8 data send_buf[3 * VFD_DIG_LEN] = {0};
static u32 xdata dig_buf[VFD_DIG_LEN - 1] = {0};
const u32 xdata fonts[40] = {
    0xc00000,  // ASCII:-,ASCII_N:45
    0x020000,  // ASCII:.,ASCII_N:46
    0x080400,  // ASCII:/,ASCII_N:47
    0x392700,  // ASCII:0,ASCII_N:48
    0x200600,  // ASCII:1,ASCII_N:49
    0xd12200,  // ASCII:2,ASCII_N:50
    0xe12200,  // ASCII:3,ASCII_N:51
    0xe00300,  // ASCII:4,ASCII_N:52
    0xe12100,  // ASCII:5,ASCII_N:53
    0xf12100,  // ASCII:6,ASCII_N:54
    0x202200,  // ASCII:7,ASCII_N:55
    0xf12300,  // ASCII:8,ASCII_N:56
    0xe12300,  // ASCII:9,ASCII_N:57
    0x040800,  // ASCII::,ASCII_N:58
    0xf02300,  // ASCII:A,ASCII_N:65
    0xa52a00,  // ASCII:B,ASCII_N:66
    0x112100,  // ASCII:C,ASCII_N:67
    0x252a00,  // ASCII:D,ASCII_N:68
    0x512100,  // ASCII:E,ASCII_N:69
    0xd02100,  // ASCII:F,ASCII_N:70
    0xb12100,  // ASCII:G,ASCII_N:71
    0xf00300,  // ASCII:H,ASCII_N:72
    0x052800,  // ASCII:I,ASCII_N:73
    0x310200,  // ASCII:J,ASCII_N:74
    0x520500,  // ASCII:K,ASCII_N:75
    0x110100,  // ASCII:L,ASCII_N:76
    0x301700,  // ASCII:M,ASCII_N:77
    0x321300,  // ASCII:N,ASCII_N:78
    0x312300,  // ASCII:O,ASCII_N:79
    0xd02300,  // ASCII:P,ASCII_N:80
    0x332300,  // ASCII:Q,ASCII_N:81
    0xd22300,  // ASCII:R,ASCII_N:82
    0xe12100,  // ASCII:S,ASCII_N:83
    0x042800,  // ASCII:T,ASCII_N:84
    0x310300,  // ASCII:U,ASCII_N:85
    0x180500,  // ASCII:V,ASCII_N:86
    0x3a0300,  // ASCII:W,ASCII_N:87
    0x0a1400,  // ASCII:X,ASCII_N:88
    0xc40300,  // ASCII:Y,ASCII_N:89
    0x092400,  // ASCII:Z,ASCII_N:90
};

u32 gui_get_font(char c);

void start_pwm() {
    PWMA_PS = 0x00;
    PWMA_CCER1 = 0x00;    // 写CCMRx前必须先清零CCERx关闭通道
    PWMA_CCMR2 = 0x68;    // 设置CC2为PWMA输出模式
    PWMA_CCER1 = 0x40;    // 使能CC2通道
    PWMA_CCR2 = PWM_CCR;  // 设置占空比时间
    PWMA_ARR = PWM_ARR;   // 设置周期时间
    PWMA_ENO = 0x08;      // 使能PWM2P端口输出
    PWMA_BKR = 0x80;      // 使能主输出
    PWMA_CR1 |= 0x81;     // 开始计时
}

void stop_pwm() {
    PWMA_CR1 &= 0xFE;  // 关闭定时器
    PWMA_ENO = 0x00;   // 禁止使能
}

void vfd_gui_init() {
    VFD_EN = 1;
    start_pwm();
    // VFD Setting
    setDisplayMode(0x06);
    setModeWirteDisplayMode(0);
    vfd_gui_set_blk_level(7);
    vfd_gui_clear();
}

void vfd_gui_stop() {
    VFD_EN = 0;
    stop_pwm();
    vfd_gui_clear();
}

void vfd_gui_clear() {
    memset(send_buf, 0x00, sizeof(send_buf));
    sendDigAndData(0, send_buf, sizeof(send_buf));
}

void vfd_gui_set_dig(u8 dig, u32 buf) {
    uint8_t arr[3] = {0};
    memset(arr, 0x00, sizeof(arr));
    if (buf) {
        arr[0] = (buf >> 16) & 0xFF;
        arr[1] = (buf >> 8) & 0xFF;
        arr[2] = buf & 0xFF;
    }
    sendDigAndData(dig * 3, arr, 3);
}

void vfd_gui_set_text(char* string, const u8 point, const u8 icon) {
    size_t str_len = strlen(string);
    size_t index = 0;
    short i = 0;
    size_t len = str_len > (VFD_DIG_LEN - 1) ? (VFD_DIG_LEN - 1) : str_len;
    memset(send_buf, 0x00, sizeof(send_buf));
    memset(dig_buf, 0, sizeof(dig_buf));
    if (len != 0) {
        for (i = (len - 1); i >= 0; i--) {
            if (string[i] != '\0') {
                dig_buf[index] = gui_get_font(string[i]);
                if (point) {
                    dig_buf[index] |= DIG_POINT;
                }
                if (icon) {
                    dig_buf[index] |= DIG_ICON;
                }
                index++;
            }
        }
    }
    index = 0;
    for (i = 0; i < (VFD_DIG_LEN - 1); i++) {
        send_buf[index++] = (dig_buf[i] >> 16) & 0xFF;
        send_buf[index++] = (dig_buf[i] >> 8) & 0xFF;
        send_buf[index++] = dig_buf[i] & 0xFF;
    }

    sendDigAndData(0, send_buf, sizeof(send_buf));
}

/**
 * 亮度调节 1~7
 */
void vfd_gui_set_blk_level(size_t level) {
    if (level == lightLevel) {
        return;
    }
    lightLevel = level;
    ptSetDisplayLight(1, lightLevel);
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    const long delta = x - in_min;
    return (delta * dividend + (divisor / 2)) / divisor + out_min;
}

u32 gui_get_font(char c) {
    if (c == ' ') {
        return 0;
    }
    if (c >= 45 && c <= 58) {
        return fonts[map(c, 45, 58, 0, 13)];
    } else if (c >= 65 && c <= 90) {
        return fonts[map(c, 65, 90, 14, 39)];
    } else if (c >= 97 && c <= 122) {
        return gui_get_font(c - 32);
    } else {
        return 0;
    }
}

u8 bit_reversed(u8 old) {
    // 颠倒所有比特位
    uint8_t reversedValue = 0, i;
    for (i = 0; i < 8; ++i) {
        reversedValue |= ((old >> i) & 1) << (7 - i);
    }
    return reversedValue;
}

/**
 * acg动画
 */
void vfd_gui_acg_update() {
    static u8 pos = 0;
    static short direction = 1;
    static u32 acg1_u32s[8] = {0};
    if (!acg1_u32s[7]) {
        for (pos = 0; pos < 8; pos++) {
            u32 b;
            u8 u1, u2, u3;
            if (pos >= 8) {
                pos = 0;
            }
            b = 0x800000 >> pos;
            u1 = bit_reversed((b >> 16) & 0xFF);
            u2 = bit_reversed((b >> 8) & 0xFF);
            u3 = bit_reversed(b & 0xFF);
            b = (uint32_t)u1 << 16 | u2 << 8 | u3;
            acg1_u32s[pos] = b;
        }
        pos = 0;
    }
    vfd_gui_set_dig(9, acg1_u32s[pos]);
    pos += direction;
    if (pos == 7) {
        direction = -1;
    } else if (pos == 0) {
        direction = 1;
    }
}

/**
 * 屏幕保护程序
 */
void vfd_gui_display_protect_exec() {
    u8 i, j;
    u8 buf[12];
    for (i = 1; i <= 10; i++) {
        memset(buf, 0x00, sizeof(buf));
        for (j = 0; j < 10; j++) {
            u8 rn = (hal_systick_get() << i) % 10;
            sprintf((&buf) + j, "%bd", rn);
            delay_ms(1);
        }
        vfd_gui_set_text(buf, 0, 0);
        delay_ms(10);
    }
    vfd_gui_clear();
}
