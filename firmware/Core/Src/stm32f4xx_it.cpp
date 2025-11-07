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
#include "./src/tick/lv_tick.h"
#include "task_encoder.h"
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
extern QueueHandle_t encoder_queue;
extern encoder_ev_t encoder_ev[16];
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
	while (1) {
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
  	encoder_ev[0].prev_state_a = encoder_ev[0].state_a;
	encoder_ev[0].state_a = HAL_GPIO_ReadPin(ENC11_A_GPIO_Port, ENC11_A_Pin);
	encoder_ev[0].prev_state_b = encoder_ev[0].state_b;
	encoder_ev[0].state_b = HAL_GPIO_ReadPin(ENC11_B_GPIO_Port, ENC11_B_Pin);
	encoder_ev[0].prev_ts = encoder_ev[0].ts;
  	encoder_ev[0].ts = HAL_GetTick();
	xQueueSendFromISR(encoder_queue, &encoder_ev[0], NULL);

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
  	encoder_ev[8].prev_state_a = encoder_ev[8].state_a;
  	encoder_ev[8].state_a = HAL_GPIO_ReadPin(ENC31_A_GPIO_Port, ENC31_A_Pin);
  	encoder_ev[8].prev_state_b = encoder_ev[8].state_b;
  	encoder_ev[8].state_b = HAL_GPIO_ReadPin(ENC31_B_GPIO_Port, ENC31_B_Pin);
  	encoder_ev[8].prev_ts = encoder_ev[8].ts;
	encoder_ev[8].ts = HAL_GetTick();
	xQueueSendFromISR(encoder_queue, &encoder_ev[8], NULL);
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
	encoder_ev[1].prev_state_a = encoder_ev[1].state_a;
	encoder_ev[1].state_a = HAL_GPIO_ReadPin(ENC12_A_GPIO_Port, ENC12_A_Pin);
	encoder_ev[1].prev_state_b = encoder_ev[1].state_b;
	encoder_ev[1].state_b = HAL_GPIO_ReadPin(ENC12_B_GPIO_Port, ENC12_B_Pin);
	encoder_ev[1].prev_ts = encoder_ev[1].ts;
	encoder_ev[1].ts = HAL_GetTick();
	xQueueSendFromISR(encoder_queue, &encoder_ev[1], NULL);
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
	encoder_ev[9].prev_state_a = encoder_ev[9].state_a;
	encoder_ev[9].state_a = HAL_GPIO_ReadPin(ENC32_A_GPIO_Port, ENC32_A_Pin);
	encoder_ev[9].prev_state_b = encoder_ev[9].state_b;
	encoder_ev[9].state_b = HAL_GPIO_ReadPin(ENC32_B_GPIO_Port, ENC32_B_Pin);
	encoder_ev[9].prev_ts = encoder_ev[9].ts;
	encoder_ev[9].ts = HAL_GetTick();
	xQueueSendFromISR(encoder_queue, &encoder_ev[9], NULL);
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
	encoder_ev[2].prev_state_a = encoder_ev[2].state_a;
	encoder_ev[2].state_a = HAL_GPIO_ReadPin(ENC13_A_GPIO_Port, ENC13_A_Pin);
	encoder_ev[2].prev_state_b = encoder_ev[2].state_b;
	encoder_ev[2].state_b = HAL_GPIO_ReadPin(ENC13_B_GPIO_Port, ENC13_B_Pin);
	encoder_ev[2].prev_ts = encoder_ev[2].ts;
	encoder_ev[2].ts = HAL_GetTick();
	xQueueSendFromISR(encoder_queue, &encoder_ev[2], NULL);

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
	if (encoder_ev[10].prev_state_a != encoder_ev[10].state_a) {
		encoder_ev[10].prev_state_a = encoder_ev[10].state_a;
		encoder_ev[10].state_a = HAL_GPIO_ReadPin(ENC33_A_GPIO_Port,
				ENC33_A_Pin);
		encoder_ev[10].prev_state_b = encoder_ev[10].state_b;
		encoder_ev[10].state_b = HAL_GPIO_ReadPin(ENC33_B_GPIO_Port,
				ENC33_B_Pin);
		encoder_ev[10].prev_ts = encoder_ev[10].ts;
		encoder_ev[10].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[10], NULL);
	} else {
		encoder_ev[10].state_a = HAL_GPIO_ReadPin(ENC33_A_GPIO_Port,
				ENC33_A_Pin);
	}

	if (encoder_ev[3].prev_state_b != encoder_ev[3].state_b) {
		encoder_ev[3].prev_state_a = encoder_ev[3].state_a;
		encoder_ev[3].state_a = HAL_GPIO_ReadPin(ENC14_A_GPIO_Port,
				ENC14_A_Pin);
		encoder_ev[3].prev_state_b = encoder_ev[3].state_b;
		encoder_ev[3].state_b = HAL_GPIO_ReadPin(ENC14_B_GPIO_Port,
				ENC14_B_Pin);
		encoder_ev[3].prev_ts = encoder_ev[3].ts;
		encoder_ev[3].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[3], NULL);
	} else {
		encoder_ev[3].state_b = HAL_GPIO_ReadPin(ENC14_B_GPIO_Port,
				ENC14_B_Pin);
	}

	if (encoder_ev[11].prev_state_a != encoder_ev[11].state_a) {
		encoder_ev[11].prev_state_a = encoder_ev[11].state_a;
		encoder_ev[11].state_a = HAL_GPIO_ReadPin(ENC34_A_GPIO_Port,
				ENC34_A_Pin);
		encoder_ev[11].prev_state_b = encoder_ev[11].state_b;
		encoder_ev[11].state_b = HAL_GPIO_ReadPin(ENC34_B_GPIO_Port,
				ENC34_B_Pin);
		encoder_ev[11].prev_ts = encoder_ev[11].ts;
		encoder_ev[11].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[11], NULL);
	} else {
		encoder_ev[11].state_a = HAL_GPIO_ReadPin(ENC34_A_GPIO_Port,
				ENC34_A_Pin);
	}

	if (encoder_ev[4].prev_state_a != encoder_ev[4].state_a) {
		encoder_ev[4].prev_state_a = encoder_ev[4].state_a;
		encoder_ev[4].state_a = HAL_GPIO_ReadPin(ENC21_A_GPIO_Port,
				ENC21_A_Pin);
		encoder_ev[4].prev_state_b = encoder_ev[4].state_b;
		encoder_ev[4].state_b = HAL_GPIO_ReadPin(ENC21_B_GPIO_Port,
				ENC21_B_Pin);
		encoder_ev[4].prev_ts = encoder_ev[4].ts;
		encoder_ev[4].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[4], NULL);
	} else {
		encoder_ev[4].state_a = HAL_GPIO_ReadPin(ENC21_A_GPIO_Port,
				ENC21_A_Pin);
	}

	if (encoder_ev[12].prev_state_a != encoder_ev[12].state_a) {
		encoder_ev[12].prev_state_a = encoder_ev[12].state_a;
		encoder_ev[12].state_a = HAL_GPIO_ReadPin(ENC41_A_GPIO_Port,
				ENC41_A_Pin);
		encoder_ev[12].prev_state_b = encoder_ev[12].state_b;
		encoder_ev[12].state_b = HAL_GPIO_ReadPin(ENC41_B_GPIO_Port,
				ENC41_B_Pin);
		encoder_ev[12].prev_ts = encoder_ev[12].ts;
		encoder_ev[12].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[12], NULL);
	} else {
		encoder_ev[12].state_a = HAL_GPIO_ReadPin(ENC41_A_GPIO_Port,
				ENC41_A_Pin);;
	}

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
	if (encoder_ev[5].prev_state_a != encoder_ev[5].state_a) {
		encoder_ev[5].prev_state_a = encoder_ev[5].state_a;
		encoder_ev[5].state_a = HAL_GPIO_ReadPin(ENC22_A_GPIO_Port,
				ENC22_A_Pin);
		encoder_ev[5].prev_state_b = encoder_ev[5].state_b;
		encoder_ev[5].state_b = HAL_GPIO_ReadPin(ENC22_B_GPIO_Port,
				ENC22_B_Pin);
		encoder_ev[5].prev_ts = encoder_ev[5].ts;
		encoder_ev[5].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[5], NULL);
	} else {
		encoder_ev[5].state_a = HAL_GPIO_ReadPin(ENC22_A_GPIO_Port,
				ENC22_A_Pin);
	}

	if (encoder_ev[13].prev_state_a != encoder_ev[13].state_a) {
		encoder_ev[13].prev_state_a = encoder_ev[13].state_a;
		encoder_ev[13].state_a = HAL_GPIO_ReadPin(ENC42_A_GPIO_Port,
				ENC42_A_Pin);
		encoder_ev[13].prev_state_b = encoder_ev[13].state_b;
		encoder_ev[13].state_b = HAL_GPIO_ReadPin(ENC42_B_GPIO_Port,
				ENC42_B_Pin);
		encoder_ev[13].prev_ts = encoder_ev[13].ts;
		encoder_ev[13].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[13], NULL);
	} else {
		encoder_ev[13].state_a = HAL_GPIO_ReadPin(ENC42_A_GPIO_Port,
				ENC42_A_Pin);
	}
	
	if (encoder_ev[6].prev_state_a != encoder_ev[6].state_a) {
		encoder_ev[6].prev_state_a = encoder_ev[6].state_a;
		encoder_ev[6].state_a = HAL_GPIO_ReadPin(ENC23_A_GPIO_Port,
				ENC23_A_Pin);
		encoder_ev[6].prev_state_b = encoder_ev[6].state_b;
		encoder_ev[6].state_b = HAL_GPIO_ReadPin(ENC23_B_GPIO_Port,
				ENC23_B_Pin);
		encoder_ev[6].prev_ts = encoder_ev[6].ts;
		encoder_ev[6].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[6], NULL);
	} else {
		encoder_ev[6].state_a = HAL_GPIO_ReadPin(ENC23_A_GPIO_Port,
				ENC23_A_Pin);
	}

	if (encoder_ev[14].prev_state_a != encoder_ev[14].state_a) {
		encoder_ev[14].prev_state_a = encoder_ev[14].state_a;
		encoder_ev[14].state_a = HAL_GPIO_ReadPin(ENC43_A_GPIO_Port,
				ENC43_A_Pin);
		encoder_ev[14].prev_state_b = encoder_ev[14].state_b;
		encoder_ev[14].state_b = HAL_GPIO_ReadPin(ENC43_B_GPIO_Port,
				ENC43_B_Pin);
		encoder_ev[14].prev_ts = encoder_ev[14].ts;
		encoder_ev[14].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[14], NULL);
	} else {
		encoder_ev[14].state_a = HAL_GPIO_ReadPin(ENC43_A_GPIO_Port,
				ENC43_A_Pin);
	}
	if (encoder_ev[7].prev_state_a != encoder_ev[7].state_a) {
		encoder_ev[7].prev_state_a = encoder_ev[7].state_a;
		encoder_ev[7].state_a = HAL_GPIO_ReadPin(ENC24_A_GPIO_Port,
				ENC24_A_Pin);
		encoder_ev[7].prev_state_b = encoder_ev[7].state_b;
		encoder_ev[7].state_b = HAL_GPIO_ReadPin(ENC24_B_GPIO_Port,
				ENC24_B_Pin);
		encoder_ev[7].prev_ts = encoder_ev[7].ts;
		encoder_ev[7].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[7], NULL);
	} else {
		encoder_ev[7].state_a = HAL_GPIO_ReadPin(ENC24_A_GPIO_Port,
				ENC24_A_Pin);
	}

	if (encoder_ev[15].prev_state_a != encoder_ev[15].state_a) {
		encoder_ev[15].prev_state_a = encoder_ev[15].state_a;
		encoder_ev[15].state_a = HAL_GPIO_ReadPin(ENC44_A_GPIO_Port,
				ENC44_A_Pin);
		encoder_ev[15].prev_state_b = encoder_ev[15].state_b;
		encoder_ev[15].state_b = HAL_GPIO_ReadPin(ENC44_B_GPIO_Port,
				ENC44_B_Pin);
		encoder_ev[15].prev_ts = encoder_ev[15].ts;
		encoder_ev[15].ts = HAL_GetTick();
		xQueueSendFromISR(encoder_queue, &encoder_ev[15], NULL);
	} else {
		encoder_ev[15].state_a = HAL_GPIO_ReadPin(ENC44_A_GPIO_Port,
				ENC44_A_Pin);
	}
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles SPI3 global interrupt.
  */
void SPI3_IRQHandler(void)
{
  /* USER CODE BEGIN SPI3_IRQn 0 */

  /* USER CODE END SPI3_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi3);
  /* USER CODE BEGIN SPI3_IRQn 1 */

  /* USER CODE END SPI3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt and DAC1, DAC2 underrun error interrupts.
  */
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

/* USER CODE END 1 */
