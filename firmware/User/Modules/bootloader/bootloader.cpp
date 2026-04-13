#include "bootloader.h"
#include "main.h"
#include "usbd_composite.h"
#include "usbd_desc.h"

#define BOOTLOADER_UPDATE_MAGIC ((uint32_t)0xB00710AD)

void JumpToBootloader(void)
{

	extern USBD_HandleTypeDef hUsbDevice;
	USBD_Stop(&hUsbDevice);
	USBD_DeInit(&hUsbDevice);

	/* Disable all interrupts */
	__disable_irq();
	/* Set the clock to the default state */
	HAL_RCC_DeInit();
	/* Disable Systick timer */
	SysTick->CTRL = 0;
	/* Clear Interrupt Enable Register & Interrupt Pending Register */
	for (uint8_t i = 0; i < (70 + 31u) / 32; i++)
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}
	/* Re-enable all interrupts */
	__enable_irq();
	/* Set up the jump to boot loader address + 4 */
	uint32_t jump_address = *(__IO uint32_t *)(0x1FFF0000 + 4);
	/* Set the main stack pointer to the boot loader stack */
	__set_MSP(*(uint32_t *)0x1FFF0000);
	/* Call the function to jump to boot loader location */
	void (*boot_load)(void) = (void (*)(void))(jump_address);
	// remap memory
	SYSCFG->MEMRMP = 0x01;
	__enable_irq();
	boot_load();
}

void EnterCustomBootloader(void)
{
	extern USBD_HandleTypeDef hUsbDevice;
	USBD_Stop(&hUsbDevice);
	USBD_DeInit(&hUsbDevice);

	/* Write update magic to RTC backup register and reset.
	 * The custom bootloader will read this on startup and enter update mode. */
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();
	RTC->BKP0R = BOOTLOADER_UPDATE_MAGIC;

	/* System reset - custom bootloader will handle the rest */
	NVIC_SystemReset();
}
