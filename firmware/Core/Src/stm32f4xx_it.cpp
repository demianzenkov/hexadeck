/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lvgl/src/tick/lv_tick.h"
#include "task_os.h"
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern DMA_HandleTypeDef hdma_spi4_tx;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern TIM_HandleTypeDef htim2;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1)
	{
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1)
	{
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI line 0 interrupt.
 */
void EXTI0_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI0_IRQn 0 */
	/* USER CODE END EXTI0_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC11_A_Pin);
	/* USER CODE BEGIN EXTI0_IRQn 1 */
	/* USER CODE END EXTI0_IRQn 1 */
}

/**
 * @brief This function handles EXTI line 1 interrupt.
 */
void EXTI1_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI1_IRQn 0 */

	/* USER CODE END EXTI1_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC31_A_Pin);
	/* USER CODE BEGIN EXTI1_IRQn 1 */
	/* USER CODE END EXTI1_IRQn 1 */
}

/**
 * @brief This function handles EXTI line 2 interrupt.
 */
void EXTI2_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI2_IRQn 0 */

	/* USER CODE END EXTI2_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC12_A_Pin);
	/* USER CODE BEGIN EXTI2_IRQn 1 */
	/* USER CODE END EXTI2_IRQn 1 */
}

/**
 * @brief This function handles EXTI line 3 interrupt.
 */
void EXTI3_IRQHandler(void)
{
	// encoder 32
	/* USER CODE BEGIN EXTI3_IRQn 0 */

	/* USER CODE END EXTI3_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC32_A_Pin);
	/* USER CODE BEGIN EXTI3_IRQn 1 */
	/* USER CODE END EXTI3_IRQn 1 */
}
/**
 * @brief This function handles EXTI line 4 interrupt.
 */
void EXTI4_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI4_IRQn 0 */

	/* USER CODE END EXTI4_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC13_A_Pin);
	/* USER CODE BEGIN EXTI4_IRQn 1 */
	/* USER CODE END EXTI4_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream4 global interrupt.
 */
void DMA1_Stream4_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

	/* USER CODE END DMA1_Stream4_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_spi2_tx);
	/* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

	/* USER CODE END DMA1_Stream4_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream5 global interrupt.
 */
void DMA1_Stream5_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

	/* USER CODE END DMA1_Stream5_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_spi3_tx);
	/* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

	/* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
 * @brief This function handles EXTI line[9:5] interrupts.
 */
void EXTI9_5_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI9_5_IRQn 0 */

	/* USER CODE END EXTI9_5_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC33_A_Pin); //
	HAL_GPIO_EXTI_IRQHandler(ENC14_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC34_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC21_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC41_A_Pin);

	/* USER CODE BEGIN EXTI9_5_IRQn 1 */
	
	/* USER CODE END EXTI9_5_IRQn 1 */
}

/**
 * @brief This function handles TIM2 global interrupt.
 */
void TIM2_IRQHandler(void)
{
	/* USER CODE BEGIN TIM2_IRQn 0 */

	/* USER CODE END TIM2_IRQn 0 */
	HAL_TIM_IRQHandler(&htim2);
	/* USER CODE BEGIN TIM2_IRQn 1 */
	lv_tick_inc(1);
	/* USER CODE END TIM2_IRQn 1 */
}

/**
 * @brief This function handles TIM3 global interrupt.
 */
void TIM3_IRQHandler(void)
{
	/* USER CODE BEGIN TIM3_IRQn 0 */

	/* USER CODE END TIM3_IRQn 0 */
	HAL_TIM_IRQHandler(&htim3);
	/* USER CODE BEGIN TIM3_IRQn 1 */

	/* USER CODE END TIM3_IRQn 1 */
}

/**
 * @brief This function handles SPI1 global interrupt.
 */
void SPI1_IRQHandler(void)
{
	/* USER CODE BEGIN SPI1_IRQn 0 */

	/* USER CODE END SPI1_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi1);
	/* USER CODE BEGIN SPI1_IRQn 1 */

	/* USER CODE END SPI1_IRQn 1 */
}

/**
 * @brief This function handles SPI2 global interrupt.
 */
void SPI2_IRQHandler(void)
{
	/* USER CODE BEGIN SPI2_IRQn 0 */

	/* USER CODE END SPI2_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi2);
	/* USER CODE BEGIN SPI2_IRQn 1 */

	/* USER CODE END SPI2_IRQn 1 */
}

void SPI3_IRQHandler(void)
{
	/* USER CODE BEGIN SPI3_IRQn 0 */

	/* USER CODE END SPI3_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi3);
	/* USER CODE BEGIN SPI3_IRQn 1 */
	/* USER CODE END SPI3_IRQn 1 */
}

/**
 * @brief This function handles EXTI line[15:10] interrupts.
 */
void EXTI15_10_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI15_10_IRQn 0 */

	/* USER CODE END EXTI15_10_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ENC22_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC42_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC23_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC43_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC24_A_Pin);
	HAL_GPIO_EXTI_IRQHandler(ENC44_A_Pin);
	/* USER CODE BEGIN EXTI15_10_IRQn 1 */
	/* USER CODE END EXTI15_10_IRQn 1 */
}

void TIM6_DAC_IRQHandler(void)
{
	/* USER CODE BEGIN TIM6_DAC_IRQn 0 */

	/* USER CODE END TIM6_DAC_IRQn 0 */
	HAL_TIM_IRQHandler(&htim6);
	/* USER CODE BEGIN TIM6_DAC_IRQn 1 */

	/* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream0 global interrupt.
 */
void DMA2_Stream0_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

	/* USER CODE END DMA2_Stream0_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_adc1);
	/* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

	/* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream1_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

	/* USER CODE END DMA2_Stream1_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_spi4_tx);
	/* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

	/* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream3 global interrupt.
 */
void DMA2_Stream3_IRQHandler(void)
{
	/* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

	/* USER CODE END DMA2_Stream3_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_spi1_tx);
	/* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

	/* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
 * @brief This function handles USB On The Go FS global interrupt.
 */
void OTG_FS_IRQHandler(void)
{
	/* USER CODE BEGIN OTG_FS_IRQn 0 */

	/* USER CODE END OTG_FS_IRQn 0 */
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
	/* USER CODE BEGIN OTG_FS_IRQn 1 */

	/* USER CODE END OTG_FS_IRQn 1 */
}

/**
 * @brief This function handles SPI4 global interrupt.
 */
void SPI4_IRQHandler(void)
{
	/* USER CODE BEGIN SPI4_IRQn 0 */

	/* USER CODE END SPI4_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi4);
	/* USER CODE BEGIN SPI4_IRQn 1 */

	/* USER CODE END SPI4_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	TaskOS * task_os = TaskOS::getInstance();
	encoder_event_t enc_ev;
	switch (GPIO_Pin) {
		case ENC11_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC11_B_GPIO_Port, ENC11_B_Pin)); 
			enc_ev.encoder_id = 0;
			break;
		}
		case ENC12_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC12_B_GPIO_Port, ENC12_B_Pin)); 
			enc_ev.encoder_id = 1;
			break;
		}
		case ENC13_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC13_B_GPIO_Port, ENC13_B_Pin)); 
			enc_ev.encoder_id = 2;
			break;
		}
		case ENC14_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC14_B_GPIO_Port, ENC14_B_Pin)); 
			enc_ev.encoder_id = 3;
			break;
		}
		case ENC21_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC21_B_GPIO_Port, ENC21_B_Pin)); 
			enc_ev.encoder_id = 4;
			break;
		}
		case ENC22_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC22_B_GPIO_Port, ENC22_B_Pin)); 
			enc_ev.encoder_id = 5;
			break;
		}
		case ENC23_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC23_B_GPIO_Port, ENC23_B_Pin)); 
			enc_ev.encoder_id = 6;
			break;
		}
		case ENC24_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC24_B_GPIO_Port, ENC24_B_Pin)); 
			enc_ev.encoder_id = 7;
			break;
		}
		case ENC31_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC31_B_GPIO_Port, ENC31_B_Pin)); 
			enc_ev.encoder_id = 8;
			break;
		}
		case ENC32_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC32_B_GPIO_Port, ENC32_B_Pin)); 
			enc_ev.encoder_id = 9;
			break;
		}
		case ENC33_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC33_B_GPIO_Port, ENC33_B_Pin)); 
			enc_ev.encoder_id = 10;
			break;
		}
		case ENC34_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC34_B_GPIO_Port, ENC34_B_Pin)); 
			enc_ev.encoder_id = 11;
			break;
		}
		case ENC41_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC41_B_GPIO_Port, ENC41_B_Pin)); 
			enc_ev.encoder_id = 12;
			break;
		}
		case ENC42_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC42_B_GPIO_Port, ENC42_B_Pin)); 
			enc_ev.encoder_id = 13;
			break;
		}
		case ENC43_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC43_B_GPIO_Port, ENC43_B_Pin)); 
			enc_ev.encoder_id = 14;
			break;
		}
		case ENC44_A_Pin: {
			enc_ev.increase = !(HAL_GPIO_ReadPin(ENC44_B_GPIO_Port, ENC44_B_Pin)); 
			enc_ev.encoder_id = 15;
			break;
		}
		default:
			return;
	}
	xQueueSendFromISR(task_os->encoder_event_queue, &enc_ev, 0);

	/* NOTE: This function Should not be modified, when the callback is needed,
			 the HAL_GPIO_EXTI_Callback could be implemented in the user file
	 */
}

/* USER CODE END 1 */
