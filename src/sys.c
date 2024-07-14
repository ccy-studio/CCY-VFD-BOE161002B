#include "sys.h"

void sys_gpio_init() {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpio;

    // 初始化VFD电源使能引脚
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Pin = VFD_EN_GPIO_PIN;
    HAL_GPIO_Init(VFD_EN_GPIO_PORT, &gpio);

    // 初始化按键
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pin = K1_GPIO_PIN & K2_GPIO_PIN & K3_GPIO_PIN;
    HAL_GPIO_Init(KEY_GPIO_PORT, &gpio);

    // 初始化pt6315
    gpio.Pull = GPIO_NOPULL;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pin = PT_CLK_GPIO_PIN;
    HAL_GPIO_Init(PT_CLK_GPIO_PORT, &gpio);
    gpio.Pin = PT_STB_GPIO_PIN & PT_DIN_GPIO_PIN;
    HAL_GPIO_Init(PT_STB_GPIO_PORT, &gpio);

    // 初始化i2c
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pin = I2C_SCL_GPIO_PIN & I2C_SDA_GPIO_PIN;
    gpio.Alternate = GPIO_AF6_I2C1;
    HAL_GPIO_Init(I2C_GPIO_PORT, &gpio);

    // 初始化RGB

    // 初始化RX8025中断
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pin = RX8025_INT_GPIO_PIN;
    HAL_GPIO_Init(RX8025_INT_GPIO_PORT, &gpio);
}
void sys_init_i2c() {}
void sys_init_pwm() {}
void sys_open_power() {
    HAL_GPIO_WritePin(VFD_EN_GPIO_PORT, VFD_EN_GPIO_PIN, GPIO_PIN_SET);
}
void sys_close_power() {
    HAL_GPIO_WritePin(VFD_EN_GPIO_PORT, VFD_EN_GPIO_PIN, GPIO_PIN_RESET);
}