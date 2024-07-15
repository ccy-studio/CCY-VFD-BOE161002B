#include "pt6315.h"

void writeData(uint8_t dat) {
    size_t i = 0;
    CLK_0;
    for (i = 0; i < 8; i++) {
        delay_us(10);
        if (dat & 0x01) {
            DIN_1;
        } else {
            DIN_0;
        }
        delay_us(10);
        CLK_1;
        delay_us(10);
        CLK_0;
        dat >>= 1;
    }
}

void setModeWirteDisplayMode(uint8_t addressMode) {
    uint8_t command = 0x40;
    if (addressMode) {
        command |= 0x4;
    }
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(command);
    delay_us(10);
    STB_1;
}


void setDisplayMode(uint8_t digit) {
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(digit);
    delay_us(10);
    STB_1;
}

void ptSetDisplayLight(uint8_t onOff, uint8_t brightnessVal) {
    uint8_t command = 0x80 | brightnessVal;
    if (onOff) {
        command |= 0x8;
    }
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    // 0x8f
    writeData(command);
    delay_us(10);
    STB_1;
}

void sendDigAndData(uint8_t dig, const uint8_t* dat, size_t len) {
    size_t i = 0;
    STB_1;
    delay_us(10);
    STB_0;
    delay_us(10);
    writeData(0xc0 | dig);
    delay_us(10);
    for (i = 0; i < len; i++) {
        writeData(dat[i]);
    }
    delay_us(10);
    STB_1;
}