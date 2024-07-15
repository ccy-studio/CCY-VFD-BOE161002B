/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gui.h"
#include "rx8025t.h"
#include "sys.h"
#include "ws2812.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 buffer[10];  // vfd显示缓存
static rx8025_timeinfo timeinfo;
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);

/**
 * @brief  Application Entry Function.
 * @retval int
 */
int main(void) {
    /* Reset of all peripherals, Initializes the Systick. */
    HAL_Init();

    /* System clock configuration */
    APP_SystemClockConfig();
    sys_gpio_init();
    sys_init_i2c();
    sys_init_pwm();
    sys_init_rgb();

    // 初始化GUI
    vfd_gui_init();
    // 开启电源
    sys_open_power();

    /* infinite loop */
    rx8025_set_time(24, 7, 15, 1, 20, 1, 5);
    vfd_gui_set_text("Start", 1, 1);
    HAL_Delay(500);

    rx8025_time_get(&timeinfo);
    memset(buffer, 0x00, sizeof(buffer));
    formart_time(&timeinfo, &buffer);
    vfd_gui_set_text(buffer, 1, 0);
    HAL_Delay(1000);
    memset(buffer, 0x00, sizeof(buffer));
    formart_time(&timeinfo, &buffer);
    vfd_gui_set_text(buffer, 1, 0);

    while (1) {
        HAL_Delay(2);
        rgb_frame_update(255, 1);
    }
}

/**
 * @brief  System clock configuration function
 * @param  None
 * @retval None
 */
static void APP_SystemClockConfig(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Oscillator configuration */
    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI |
        RCC_OSCILLATORTYPE_LSI |
        RCC_OSCILLATORTYPE_LSE; /* Select oscillator HSE, HSI, LSI, LSE */
    RCC_OscInitStruct.HSIState = RCC_HSI_ON; /* Enable HSI */
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1; /* HSI 1 frequency division */
    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_24MHz; /* Configure HSI clock 24MHz */
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DISABLE; /* Close HSE bypass */
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;            /* Close LSI */
    /*RCC_OscInitStruct.LSICalibrationValue = RCC_LSICALIBRATION_32768Hz;*/
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF; /* Close LSE */
    /*RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;*/
    /* Configure oscillator */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        APP_ErrorHandler();
    }

    /* Clock source configuration */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1; /* Choose to configure clock HCLK, SYSCLK, PCLK1 */
    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSISYS; /* Select HSISYS as the system clock */
    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;                              /* AHB clock 1 division */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; /* APB clock 1 division */
    /* Configure clock source */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        APP_ErrorHandler();
    }
}

/**
 * @brief  Error executing function.
 * @param  None
 * @retval None
 */
void APP_ErrorHandler(void) {
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* Users can add their own printing information as needed,
       for example: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* Infinite loop */
    while (1) {
    }
}
#endif /* USE_FULL_ASSERT */
