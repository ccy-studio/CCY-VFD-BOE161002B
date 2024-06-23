#include "i2c.h"

bool i2c_send(u8 byte) {
    return true;
}
bool i2c_start(void) {
    if (I2CMSST & 0X80) {
		
    }
    return true;
}
void i2c_stop(void) {}
u8 i2c_read(bool ack) {
    return 0;
}