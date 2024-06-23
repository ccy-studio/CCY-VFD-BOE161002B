#include "sys.h"

bool i2c_send(u8 byte);
bool i2c_start(void);
void i2c_stop(void);
u8 i2c_read(bool ack);