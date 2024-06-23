#include "sys.h"

u32 data _systick_ccr = 0;

#ifdef DEV_PLATFROM

char* USER_STCISPCMD = "@STCISP#";
char* USER_PRODUCTDESC = NULL;
char* USER_DEVICEDESC = NULL;

void hal_init_uart(void) {
    // CDC USB虚拟串口
    P3M0 &= 0x03;
    P3M1 |= 0x03;
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01))
        ;
    USBCLK = 0x00;
    USBCON = 0x90;
    usb_init();
    IE2 |= 0x80;
    EA = 1;
}
#else
void hal_init_uart(void) {}
#endif

void hal_init_systick() {
    // 1毫秒@24Mhz
    AUXR |= 0x80;  // 定时器时钟1T模式
    TMOD = 0xF3;   // 设置定时器模式 不可屏蔽模式
    TL0 = 0x40;    // 设置定时初始值
    TH0 = 0xA2;    // 设置定时初始值
    TF0 = 0;       // 清除TF0标志
    TR0 = 1;       // 定时器0开始计时
    ET0 = 1;       // 使能定时器0中断
}

u32 hal_systick_get() {
    return _systick_ccr;
}

void timer0_Isr(void) interrupt 1 {
    _systick_ccr++;
    TF0 = 0;  // 清除TF0标志
}

void hal_init_all_gpio(void) {
    P0M0 = 0x0e;
    P0M1 = 0x00;
    P2M0 = 0x70;
    P2M1 = 0x70;
    P1M0 = 0x88;
    P1M1 = 0x70;

    // I2C内部上拉
    P2PU = 0x70;
    // Key内部上拉
    P1PU = 0x70;

    // 开启硬件I2C
    P_SW2 = (P_SW2 & ~0x30) | 0x10;  // I2C: I2CSCL(P2.5), I2CSDA(P2.4)

    EA = 1;  // 开总中断
}

void delay_ms(u32 ms) {
    unsigned char data i, j;

    do {
        _nop_();
        i = 32;
        j = 40;
        do {
            while (--j)
                ;
        } while (--i);
    } while (--ms);
}

void delay_us(u32 us) {
    unsigned char data i;

    do {
        i = 6;
        while (--i)
            ;
    } while (--us);
}

u8 btn_gpio_read(btn_gpio_t gpio) {
    if (gpio == BTN_P16) {
        return P16;
    } else if (gpio == BTN_P15) {
        return P15;
    } else if (gpio == BTN_P14) {
        return P14;
    }
    return 0;
}

void hal_init_i2c(void){
    //I2C总线速度=Fosc/2/(MSSPEED*2+4)
    I2CCFG = 0xCD; //MSSPEED = 13; 24000000/2/(13*2+4) = 400Khz
}