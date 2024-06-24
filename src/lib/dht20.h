/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-06-24 11:08:46
 * @LastEditTime: 2024-06-24 14:43:16
 */
#include "sys.h"

#define DHT20_CRC 0  // CRC是否检查

typedef struct {
    float tmep;      // 温度
    float humidity;  // 湿度
} dht20_val;

/**
 * 初始化
 */
void dht20_init(void);

/**
 * 获取温湿度信息
 */
void dht20_read(dht20_val* val);