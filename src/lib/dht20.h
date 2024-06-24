/*
 * @Description:
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-06-24 11:08:46
 * @LastEditTime: 2024-06-24 14:43:16
 */
#include "sys.h"

#define DHT20_CRC 0  // CRC�Ƿ���

typedef struct {
    float tmep;      // �¶�
    float humidity;  // ʪ��
} dht20_val;

/**
 * ��ʼ��
 */
void dht20_init(void);

/**
 * ��ȡ��ʪ����Ϣ
 */
void dht20_read(dht20_val* val);