/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gui.h"
#include "rx8025t.h"
#include "sys.h"
#include "ws2812.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 last_ms_rgb, last_ms_vfd_main, last_ms_date_setting;
static u8 buffer[10];  // vfd显示缓存
static rx8025_timeinfo timeinfo;
extern btn_t curr_btn;
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

    // 开启电源
    sys_open_power();
    // 初始化GUI
    vfd_gui_init();

    rgb_clear();

    /* infinite loop */
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, "Start");
    vfd_gui_set_text(buffer, 0, 1);
    for (u8 i = 0; i < 4; i++) {
        HAL_Delay(200);
        strcat(buffer, ".");
        vfd_gui_set_text(buffer, 0, 1);
    }

    HAL_Delay(300);

    while (1) {
        // HAL_Delay(500);
        // memset(buffer, 0x00, sizeof(buffer));
        // rx8025_time_get(&timeinfo);
        // formart_time(&timeinfo, &buffer);
        // vfd_gui_set_text(buffer, 1, 0);
        // 按键扫描
        if (curr_btn.falg) {
            if (curr_btn.gpio_pin == K1_GPIO_PIN) {
            } else if (curr_btn.gpio_pin == K2_GPIO_PIN) {
            } else if (curr_btn.gpio_pin == K3_GPIO_PIN) {
            }
            sys_btn_release(&curr_btn);
        }
    }
}

/**
 * -----------------------------业务逻辑-----------------------------
 */

static void logic_vfd_refresh() {}

static void logic_rgb_refresh() {
    if ((HAL_GetTick() - last_ms_rgb) >= 2) {
        rgb_frame_update(255, 1);
        last_ms_rgb = HAL_GetTick();
    }
}

/**
 * -----------------------------系统初始化-----------------------------
 */

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
    vfd_gui_clear();
    while (1) {
        HAL_Delay(1000);
        vfd_gui_set_text("Error", 0, 0);
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
