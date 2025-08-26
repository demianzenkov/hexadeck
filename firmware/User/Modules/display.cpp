/*
 * display.c
 *
 *  Created on: Apr 13, 2024
 *      Author: demian
 */

#include "display.h"
#include "src/drivers/display/st7735/lv_st7735.h"
#include "task_lvgl.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;


//extern lv_display_t *lcd_disp;
volatile int lcd_bus_busy = 0;
volatile display_state_t * ds_active;

const display_state_t ds[16] = {
        {0, &hspi1, D11_CS_GPIO_Port, D11_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
        {1, &hspi3, D21_CS_GPIO_Port, D21_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
        {2, &hspi2, D31_CS_GPIO_Port, D31_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
        {3, &hspi4, D41_CS_GPIO_Port, D41_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
        {4, &hspi1, D12_CS_GPIO_Port, D12_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
        {5, &hspi3, D22_CS_GPIO_Port, D22_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
        {6, &hspi2, D32_CS_GPIO_Port, D32_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
        {7, &hspi4, D42_CS_GPIO_Port, D42_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
        {8, &hspi1, D13_CS_GPIO_Port, D13_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
        {9, &hspi3, D23_CS_GPIO_Port, D23_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
        {10, &hspi2, D33_CS_GPIO_Port, D33_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
        {11, &hspi4, D43_CS_GPIO_Port, D43_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
        {12, &hspi1, D14_CS_GPIO_Port, D14_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
        {13, &hspi3, D24_CS_GPIO_Port, D24_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
        {14, &hspi2, D34_CS_GPIO_Port, D34_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
        {15, &hspi4, D44_CS_GPIO_Port, D44_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin}
};

void set_active_display(uint8_t id) {
    ds_active = (display_state_t *)&ds[id];
}

static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi) {
	/* CS high */
	HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_SET);
	lcd_bus_busy = 0;
	lv_display_flush_ready(ui.lcd_disp);
}

/* Initialize LCD I/O bus, reset LCD */
int32_t lcd_io_init(void) {
	/* Register SPI Tx Complete Callback */
	HAL_SPI_RegisterCallback(ds_active->hspi, HAL_SPI_TX_COMPLETE_CB_ID,
			lcd_color_transfer_ready_cb);

	/* reset LCD */
    HAL_GPIO_WritePin(ds_active->reset_port, ds_active->reset_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
    HAL_GPIO_WritePin(ds_active->reset_port, ds_active->reset_pin, GPIO_PIN_SET);
	HAL_Delay(10);

    HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ds_active->rs_port, ds_active->rs_pin, GPIO_PIN_SET);

	return HAL_OK;
}

/* Platform-specific implementation of the LCD send command function. In general this should use polling transfer. */
void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd,
		size_t cmd_size, const uint8_t *param, size_t param_size) {
	LV_UNUSED(disp);
	while (lcd_bus_busy)
		; /* wait until previous transfer is finished */
	/* Set the SPI in 8-bit mode */
	ds_active->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(ds_active->hspi);
	/* DCX low (command) */
    HAL_GPIO_WritePin(ds_active->rs_port, ds_active->rs_pin, GPIO_PIN_RESET);
	/* CS low */
    HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_RESET);
	/* send command */
	if (HAL_SPI_Transmit(ds_active->hspi, (uint8_t *)cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
		/* DCX high (data) */
        HAL_GPIO_WritePin(ds_active->rs_port, ds_active->rs_pin, GPIO_PIN_SET);
		/* for short data blocks we use polling transfer */
		HAL_SPI_Transmit(ds_active->hspi, (uint8_t*) param, (uint16_t) param_size,
		BUS_SPI1_POLL_TIMEOUT);
		/* CS high */
        HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_SET);
	}
}

/* Platform-specific implementation of the LCD send color function. For better performance this should use DMA transfer.
 * In case of a DMA transfer a callback must be installed to notify LVGL about the end of the transfer.
 */
void lcd_send_color(lv_display_t *disp, const uint8_t *cmd,
		size_t cmd_size, uint8_t *param, size_t param_size) {
	LV_UNUSED(disp);
	while (lcd_bus_busy)
		; /* wait until previous transfer is finished */
	/* Set the SPI in 8-bit mode */
	ds_active->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(ds_active->hspi);
	/* DCX low (command) */
    HAL_GPIO_WritePin(ds_active->rs_port, ds_active->rs_pin, GPIO_PIN_RESET);
	/* CS low */
    HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_RESET);
	/* send command */
	if (HAL_SPI_Transmit(ds_active->hspi, (uint8_t *)cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
		/* DCX high (data) */
        HAL_GPIO_WritePin(ds_active->rs_port, ds_active->rs_pin, GPIO_PIN_SET);
		/* for color data use DMA transfer */
		/* Set the SPI in 16-bit mode to match endianness */
		ds_active->hspi->Init.DataSize = SPI_DATASIZE_16BIT;
		HAL_SPI_Init(ds_active->hspi);
		lcd_bus_busy = 1;
		HAL_SPI_Transmit_DMA(ds_active->hspi, param, (uint16_t) param_size / 2);
		/* NOTE: CS will be reset in the transfer ready callback */
	}
}


