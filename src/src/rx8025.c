/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-09-04 10:53:37
 * @LastEditTime: 2024-06-03 15:49:54
 */
#include <rx8025.h>

#define RX8025T_ADDR_W 0x64
#define RX8025T_ADDR_R 0x65

void i2c_start(){
    if(I2CMSST)
    I2CMSCR = 0x01;
}

u8 i2c_write(u8 buf) {
   
}

u8 i2c_read(u8 ack) {
    uint8_t receiveData = 0, i;
   
    return receiveData;
}

void rx8025_read(u8 address, u8* buf, u8 len) {
    u8 i;
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_W)) {
        return;
    }
    if (!i2c_write(address)) {
        return;
    }
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_R)) {
        return;
    }
    for (i = 0; i < len; i++) {
        buf[i] = i2c_read((i == len - 1) ? 0 : 1);
    }
    i2c_stop();
}

void rx8025_write(u8 address, u8* buf, u8 len) {
    u8 i;
    i2c_start();
    if (!i2c_write(RX8025T_ADDR_W)) {
        return;
    }
    if (!i2c_write(address)) {
        return;
    }
    for (i = 0; i < len; i++) {
        if (!i2c_write(buf[i])) {
            return;
        }
    }
    i2c_stop();
}

void rx8025t_init() {
    i2c_init();
}

u8 toBcd(u8 val) {
    return ((val / 10) << 4) | (val % 10);
}

u8 toDec(u8 bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void rx8025_set_time(u8 year,
                     u8 month,
                     u8 day,
                     u8 week,
                     u8 hour,
                     u8 min,
                     u8 sec) {
    u8 command[7];
    i2c_init();
    command[0] = toBcd(sec);
    command[1] = toBcd(min);
    command[2] = toBcd(hour);
    command[3] = (0x00 >> week) | 0x01;
    command[4] = toBcd(day);
    command[5] = toBcd(month);
    command[6] = toBcd(year);
    rx8025_write(0x00, command, 7);
}

void rx8025_time_get(rx8025_timeinfo* timeinfo) {
    u8 buf[7];
    i2c_init();
    rx8025_read(0x00, buf, 7);
    timeinfo->sec = toDec(buf[0]);
    timeinfo->min = toDec(buf[1]);
    timeinfo->hour = toDec(buf[2]);
    timeinfo->day = toDec(buf[4]);
    timeinfo->month = toDec(buf[5]);
    timeinfo->year = toDec(buf[6]);
    if (timeinfo->year > 99) {
        timeinfo->year = 23;
    }
    if (timeinfo->month > 12) {
        timeinfo->month = 12;
    }
    timeinfo->week = (-35 + timeinfo->year + (timeinfo->year / 4) +
                      (13 * (timeinfo->month + 1) / 5) + timeinfo->day - 1) %
                     7;
    if (timeinfo->week > 7) {
        timeinfo->week = 7;
    }
    if (timeinfo->week == 0) {
        timeinfo->week = 7;
    }
    if (timeinfo->hour >= 24) {
        timeinfo->hour = 0;
    }
    if (timeinfo->min >= 60) {
        timeinfo->min = 0;
    }
    if (timeinfo->sec >= 60) {
        timeinfo->sec = 0;
    }
}

void formart_time(rx8025_timeinfo* timeinfo, char* buf) {
    sprintf(buf, "W%bd %02bd%02bd%02bd", timeinfo->week, timeinfo->hour,
            timeinfo->min, timeinfo->sec);
}

/**
 *  YYMMdd
 */
void formart_date(rx8025_timeinfo* timeinfo, char* buf) {
    sprintf(buf, "20%bd %02bd%02bd", timeinfo->year, timeinfo->month,
            timeinfo->day);
}