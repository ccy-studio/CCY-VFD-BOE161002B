#include "i2c.h"

bool i2c_send(u8 byte) {}
bool i2c_start(void) {
    if (I2CMSST & 0X80) {
		
    }
}
void i2c_stop(void) {}
u8 i2c_read(bool ack) {}