/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-06-24 09:14:58
 * @LastEditTime: 2024-06-24 14:22:52
 */
#include "i2c.h"

#if I2C_HARDWARE

/**
 * 等待命令执行成功
 */
static void wait_exec() {
    u8 cnt = 0;
    while (!(I2CMSST & 0x40)) {
        if (cnt >= 100) {
            break;
        }
        cnt++;
        delay_ms(1);
    }
    // 中断位,置零
    I2CMSST &= ~0x40;
    if (cnt >= 100) {
        I2CMSST = 0x00;
    }
}

bool i2c_send(u8 byte) {
    I2CTXD = byte;
    // Send Data And Rece Ack
    I2CMSCR = 0x0A;
    // 等待数据发送完成
    wait_exec();
    // 读取ACK并返回
    return I2CMSST & 0x02;
}

bool i2c_start(void) {
    // 发送Start信号
    I2CMSCR = 0x01;
    wait_exec();
    return true;
}

void i2c_stop(void) {
    // 发送STOP信号
    I2CMSCR = 0x06;
    wait_exec();
}

u8 i2c_read(bool ack) {
    u8 rev = 0;
    I2CMSCR = 0x04;
    wait_exec();
    rev = I2CRXD;
    // 发ACK命令
    I2CMSST = ack ? 0x00 : 0x01;
    I2CMSCR = 0x05;
    wait_exec();
    return rev;
}

#else

#define SDA_1 I2C_SDA = 1
#define SDA_0 I2C_SDA = 0

#define SCL_1 I2C_SCL = 1
#define SCL_0 I2C_SCL = 0

static void i2c_sda_out() {
    P2M0 |= 0x10;
    P2M1 |= 0x10;
}

static void i2c_sda_in() {
    P2M1 |= 0x10;
    P2M0 &= ~0x10;
}

void i2c_init() {
    // 设置 SDA、SCL开漏输出
    P2M0 |= 0x30;
    P2M1 |= 0x30;
    i2c_sda_out();
}

bool i2c_start(void) {
    i2c_sda_out();
    SDA_1;
    SCL_1;
    delay_us(4);
    SDA_0;
    delay_us(4);
    SCL_0;
    return true;
}
void i2c_stop(void) {
    i2c_sda_out();
    SCL_0;
    SDA_0;
    SCL_1;
    delay_us(4);
    SDA_1;
    delay_us(4);
}

void i2c_nack() {
    i2c_sda_out();
    SDA_1;
    SCL_0;
    delay_us(4);
    SCL_1;
    delay_us(4);
    SCL_0;
}

void i2c_ack() {
    i2c_sda_out();
    SDA_0;
    SCL_0;
    delay_us(4);
    SCL_1;
    delay_us(4);
    SCL_0;
}

u8 i2c_check_ack() {
    uint8_t ack = 0, errorRetry = 3;
    SDA_1;
    delay_us(1);
    i2c_sda_in();
    delay_us(1);
    SCL_1;
    delay_us(1);
    while (I2C_SDA) {
        if (errorRetry <= 0) {
            break;
        }
        delay_us(1);
        errorRetry--;
    }
    if (errorRetry) {
        ack = 1;
    }
    if (!ack) {
        i2c_stop();
    }
    SCL_0;
    return ack;
}
u8 i2c_read(bool ack) {
    uint8_t receiveData = 0, i;
    i2c_sda_in();
    for (i = 0; i < 8; i++) {
        SCL_0;
        delay_us(4);
        SCL_1;
        receiveData <<= 1;
        if (I2C_SDA) {
            receiveData |= 0x01;
        }
        delay_us(1);
    }
    SCL_0;
    ack ? i2c_ack() : i2c_nack();
    return receiveData;
}

bool i2c_send(u8 buf) {
    u8 i;
    i2c_sda_out();
    SCL_0;
    delay_us(4);
    for (i = 0; i < 8; i++) {
        if (buf & 0x80) {
            SDA_1;
        } else {
            SDA_0;
        }
        delay_us(4);
        SCL_1;
        delay_us(4);
        SCL_0;
        buf <<= 1;
    }
    return i2c_check_ack();
}

#endif