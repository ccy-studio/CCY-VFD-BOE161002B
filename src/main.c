/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sys.h"
#include "widget.h"
#include "ws2812.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern btn_t curr_btn;
extern u8 rgb_light;
extern u8 rgb_type;
extern u8 acg_open;
u32 rgb_exec_time;
u32 acg_exec_time;
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
    // 清除关闭rgb灯光
    rgb_clear();
    // 初始化组件
    widget_init();

    /* infinite loop */
    memset(vfd_buffer, 0x00, sizeof(vfd_buffer));
    strcpy(vfd_buffer, "Start");
    vfd_gui_set_text(vfd_buffer, 0, 1);
    for (u8 i = 0; i < 4; i++) {
        HAL_Delay(200);
        strcat(vfd_buffer, ".");
        vfd_gui_set_text(vfd_buffer, 0, 1);
    }

    HAL_Delay(300);

    while (1) {
        // 按键扫描
        if (curr_btn.falg) {
            widget_send_btn_event(&curr_btn);
            sys_btn_release(&curr_btn);
        }
        // RGB刷新逻辑
        if (rgb_type <= 2) {
            if (HAL_GetTick() - rgb_exec_time > 2) {
                rgb_frame_update(rgb_light, rgb_type);
                rgb_exec_time = HAL_GetTick();
            }
        }

        // ACG动画执行
        if (acg_open && curr_widget->name == WIDGET_NAME_VFD_TIME) {
            if (HAL_GetTick() - acg_exec_time > 120) {
                vfd_gui_acg_update();
                acg_exec_time = HAL_GetTick();
            }
        }
        // 组件刷新
        widget_refresh(NULL);
    }
}

/**
 * -----------------------------业务逻辑-----------------------------
 */

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
