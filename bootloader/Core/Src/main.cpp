/*
 * Hexadeck Custom Bootloader
 *
 * Memory layout:
 *   0x08000000 - 0x0800BFFF : Bootloader (48KB, sectors 0-2)
 *   0x0800C000 - 0x0800FFFF : NVS data (16KB, sector 3)
 *   0x08010000 - 0x0807FFFF : Application firmware (448KB, sectors 4-7)
 *
 * Boot flow:
 *   1. Check backup register for update magic value
 *   2. If magic found → clear register, init USB MIDI, enter update mode
 *   3. If no magic → check if valid application exists at APP_ADDRESS
 *   4. If valid app → jump to application
 *   5. If no valid app → enter update mode
 */

#include "main.h"
#include "usb_otg.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "bl_protocol.h"
#include "bl_flash.h"

void SystemClock_Config(void);

static bool check_update_requested(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    uint32_t flag = RTC->BKP0R;
    if (flag == BOOTLOADER_UPDATE_MAGIC) {
        RTC->BKP0R = 0;
        return true;
    }
    return false;
}

static bool is_valid_app(void)
{
    uint32_t app_msp = *(__IO uint32_t *)APP_ADDRESS;
    /* Check if MSP value points to valid RAM range (0x20000000 - 0x20020000) */
    return ((app_msp & 0xFFF00000) == 0x20000000);
}

static void jump_to_app(void)
{
    uint32_t app_msp = *(__IO uint32_t *)APP_ADDRESS;
    uint32_t app_reset = *(__IO uint32_t *)(APP_ADDRESS + 4);

    /* Disable all interrupts */
    __disable_irq();

    /* Disable SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Clear all NVIC interrupt enables and pending flags */
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* Set vector table to application */
    SCB->VTOR = APP_ADDRESS;

    /* Set MSP to application's stack pointer */
    __set_MSP(app_msp);

    /* Re-enable interrupts */
    __enable_irq();

    /* Jump to application reset handler */
    void (*app_entry)(void) = (void (*)(void))(app_reset);
    app_entry();
}

extern "C" int main(void)
{
    HAL_Init();
    SystemClock_Config();

    bool enter_update = check_update_requested();

    if (!enter_update && is_valid_app()) {
        /* Valid application exists and no update requested → jump */
        jump_to_app();
    }

    /* Enter update mode: init USB MIDI and wait for firmware */
    bl_protocol_init();
    MX_USB_OTG_FS_PCD_Init();
    MX_USB_DEVICE_Init();

    while (1) {
        bl_protocol_process();
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        HAL_IncTick();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}
