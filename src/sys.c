#include "sys.h"

// 24Mhz
static u8 sys_clk_us = 24;

I2C_HandleTypeDef i2c;
TIM_HandleTypeDef pwm_tim;
SPI_HandleTypeDef spi;

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

    // 初始化RX8025中断
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pin = RX8025_INT_GPIO_PIN;
    HAL_GPIO_Init(RX8025_INT_GPIO_PORT, &gpio);

    // 初始化灯丝PWM
    gpio.Pull = GPIO_NOPULL;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Pin = PWM_GPIO_PIN;
    gpio.Alternate = GPIO_AF3_TIM1;
    HAL_GPIO_Init(PWM_GPIO_PORT, &gpio);

    // 初始化i2c
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pin = I2C_SCL_GPIO_PIN & I2C_SDA_GPIO_PIN;
    gpio.Alternate = GPIO_AF6_I2C1;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(I2C_GPIO_PORT, &gpio);

    // 初始化RGB
    gpio.Pull = GPIO_PULLUP;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pin = RGB_GPIO_PIN;
    gpio.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(RGB_GPIO_PORT, &gpio);
}
void sys_init_i2c() {
    __HAL_RCC_I2C_CLK_ENABLE();
    i2c.Instance = I2C1;
    // 设置时钟频率300Khz
    i2c.Init.ClockSpeed = 300000;
    i2c.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
    if (HAL_I2C_Init(&i2c) != HAL_OK) {
        APP_ErrorHandler();
    }

    __HAL_RCC_I2C_FORCE_RESET();
    __HAL_RCC_I2C_RELEASE_RESET();
}

void sys_init_pwm() {
    __HAL_RCC_TIM1_CLK_ENABLE();
    // PWM的频率 = 时钟频率 / （自动重装值 + 1）*（预分频值 + 1）
    //  24000000/100/24=10,000Hz.
    /* TIM1 */
    pwm_tim.Instance = TIM1;

    pwm_tim.Init.Period = 100 - 1;  // ARR

    pwm_tim.Init.Prescaler = 24 - 1;  // PSC

    /* ClockDivision = 0  */
    pwm_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    /* Counter direction = Up */
    pwm_tim.Init.CounterMode = TIM_COUNTERMODE_UP;

    /* Repetition = 0 */
    pwm_tim.Init.RepetitionCounter = 1 - 1;

    /* Auto-reload register not buffered  */
    pwm_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&pwm_tim) != HAL_OK) {
        APP_ErrorHandler();
    }

    TIM_OC_InitTypeDef sConfig;

    /* Set output compare mode: PWM1  */
    sConfig.OCMode = TIM_OCMODE_PWM1;

    /* OC channel output high level effective */
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;

    /* Disable OC FastMode */
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    /* OCN channel output high level effective */
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;

    /* Idle state OC1N output low level */
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    /* Idle state OC1 output low level*/
    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;

    // 占空比 = CCR / (ARR + 1) (50/(100+1) = 50%)
    sConfig.Pulse = 50;

    /* Channel 1 configuration */
    if (HAL_TIM_PWM_ConfigChannel(&pwm_tim, &sConfig, TIM_CHANNEL_4) !=
        HAL_OK) {
        APP_ErrorHandler();
    }
}

void sys_init_rgb() {
    __HAL_RCC_SPI1_CLK_ENABLE();
    spi.Instance = SPI1;
    spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    spi.Init.Direction = SPI_DIRECTION_2LINES;
    spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi.Init.CLKPhase = SPI_PHASE_1EDGE;
    spi.Init.DataSize = SPI_DATASIZE_8BIT;
    spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi.Init.NSS = SPI_NSS_HARD_INPUT;
    spi.Init.Mode = SPI_MODE_MASTER;

    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();

    /* SPI initialization */
    if (HAL_SPI_Init(&spi) != HAL_OK) {
        APP_ErrorHandler();
    }
}

void sys_open_power() {
    HAL_GPIO_WritePin(VFD_EN_GPIO_PORT, VFD_EN_GPIO_PIN, GPIO_PIN_SET);
    HAL_TIM_PWM_Start(&pwm_tim, TIM_CHANNEL_4);
}

void sys_close_power() {
    HAL_GPIO_WritePin(VFD_EN_GPIO_PORT, VFD_EN_GPIO_PIN, GPIO_PIN_RESET);
    HAL_TIM_PWM_Stop(&pwm_tim, TIM_CHANNEL_4);
}

/**
 * Hz代表每秒周期震动1次， 60Hz代表每秒周期震动60次。
 * 假如滴答时钟的频率是72MHZ，72MHz表示每秒钟有72,000,000个时钟周期。
 * 那让滴答时钟计1次，时间过去了1/72μs，也就是一个时钟周期为1/72000000 s =1/72
 us。 ●定时1us，就需要72个时钟周期。 ●定时1s，就需要72000个时钟周期。
 */
void delay_us(u32 us) {
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;  // LOAD的值
    ticks = us * sys_clk_us;     // 需要的节拍数
    told = SysTick->VAL;         // 刚进入时的计数器值
    while (1) {
        tnow = SysTick->VAL;
        // SysTick是24位向下计数器
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break;  // 时间超过/等于要延迟的时间,则退出
            }
        }
    }
}
void delay_ms(u32 ms) {
    u32 i;
    for (i = 0; i < ms; i++) {
        delay_us(1000);
    }
}