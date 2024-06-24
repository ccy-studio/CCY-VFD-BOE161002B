#include "dht20.h"
#include "i2c.h"

static u8 read_status() {
    u8 byte;
    i2c_start();
    i2c_send(0x71);
    byte = i2c_read(0);
    i2c_stop();
    return byte;
}

static void reset_reg(u8 addr) {
    u8 b1, b2, b3;
    i2c_start();
    i2c_send(0x70);
    i2c_send(addr);
    i2c_send(0x00);
    i2c_send(0x00);
    i2c_stop();

    delay_ms(5);

    i2c_start();
    i2c_send(0x71);
    b1 = i2c_read(1);
    b2 = i2c_read(1);
    b3 = i2c_read(0);
    i2c_stop();

    delay_ms(10);
    i2c_start();
    i2c_send(0x70);
    i2c_send(0xB0 | addr);
    i2c_send(b2);
    i2c_send(b3);
    i2c_stop();
}

static void send_ac() {
    i2c_start();
    i2c_send(0x70);
    i2c_send(0xac);
    i2c_send(0x33);
    i2c_send(0x00);
    i2c_stop();
}

void dht20_init(void) {
    if ((read_status() & 0x18) != 0x18) {
        reset_reg(0x1b);
        reset_reg(0x1c);
        reset_reg(0x1e);
        delay_ms(10);
    }
}

#if DHT20_CRC

static u8 cal_crc(uint8_t* message, uint8_t num) {
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;
    for (byte = 0; byte < num; byte++) {
        crc ^= (message[byte]);
        for (i = 8; i > 0; --i) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

void dht20_read(dht20_val* val) {
    volatile u8 b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0;
    u32 ret_data = 0, t = 0, h = 0;
    size_t cnt = 0;
    u8 crc_dat[6] = {0};

    // 发测量命令
    send_ac();
    delay_ms(80);
    // 直到状态bit[7]为0，表示为空闲状态，若为1，表示忙状态
    while (((read_status() & 0x80) == 0x80)) {
        delay_us(1508);
        if (cnt++ >= 100) {
            break;
        }
    }
    i2c_start();
    i2c_send(0x71);
    crc_dat[0] = b1 = i2c_read(1);
    crc_dat[1] = b2 = i2c_read(1);
    crc_dat[2] = b3 = i2c_read(1);
    crc_dat[3] = b4 = i2c_read(1);
    crc_dat[4] = b5 = i2c_read(1);
    crc_dat[5] = b6 = i2c_read(1);
    b7 = i2c_read(0);
    i2c_stop();

    if (cal_crc(crc_dat, 6) == b7) {
        ret_data = (ret_data | b2) << 8;
        ret_data = (ret_data | b3) << 8;
        ret_data = (ret_data | b4);
        ret_data = ret_data >> 4;
        h = ret_data;  // 湿度
        ret_data = 0;
        ret_data = (ret_data | b4) << 8;
        ret_data = (ret_data | b5) << 8;
        ret_data = (ret_data | b6);
        ret_data = ret_data & 0xfffff;
        t = ret_data;  // 温度

        val->tmep = (t * 200.0 * 10 / 1024.0 / 1024.0 - 500) / 10.0;
        val->humidity = (h * 100.0 * 10 / 1024.0 / 1024.0) / 10.0;
    } else {
        val->tmep = -1;
        val->humidity = -1;
    }
}

#else

void dht20_read(dht20_val* val) {
    volatile u8 b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
    u32 ret_data = 0, t = 0, h = 0;
    size_t cnt = 0;

    // 发测量命令
    send_ac();
    delay_ms(80);
    // 直到状态bit[7]为0，表示为空闲状态，若为1，表示忙状态
    while (((read_status() & 0x80) == 0x80)) {
        delay_us(1508);
        if (cnt++ >= 100) {
            break;
        }
    }
    i2c_start();
    i2c_send(0x71);
    b1 = i2c_read(1);
    b2 = i2c_read(1);
    b3 = i2c_read(1);
    b4 = i2c_read(1);
    b5 = i2c_read(1);
    b6 = i2c_read(0);
    i2c_stop();

    ret_data = (ret_data | b2) << 8;
    ret_data = (ret_data | b3) << 8;
    ret_data = (ret_data | b4);
    ret_data = ret_data >> 4;
    h = ret_data;  // 湿度
    ret_data = 0;
    ret_data = (ret_data | b4) << 8;
    ret_data = (ret_data | b5) << 8;
    ret_data = (ret_data | b6);
    ret_data = ret_data & 0xfffff;
    t = ret_data;  // 温度

    val->tmep = (t / 1048576.00) * 200.0 - 50;
    val->humidity = (h / 1048576.00) * 100;
}

#endif