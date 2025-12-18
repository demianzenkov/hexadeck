/*
 * display.cpp
 *
 */

#include "display.h"
#include "task_lvgl.h"
#include "lvgl/src/drivers/display/st7735/lv_st7735.h"
#include "lv_lcd_custom_mipi.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;


volatile display_state_t * Display::ds_active;
volatile int Display::lcd_bus_busy = 0;


const display_state_t Display::ds[] = {
        {0, &hspi1, D11_CS_GPIO_Port, D11_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
		{1, &hspi1, D12_CS_GPIO_Port, D12_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
		{2, &hspi1, D13_CS_GPIO_Port, D13_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
		{3, &hspi1, D14_CS_GPIO_Port, D14_CS_Pin, D1_RESET_GPIO_Port, D1_RESET_Pin, D1_RS_GPIO_Port, D1_RS_Pin},
		{4, &hspi3, D21_CS_GPIO_Port, D21_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
		{5, &hspi3, D22_CS_GPIO_Port, D22_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
		{6, &hspi3, D23_CS_GPIO_Port, D23_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
		{7, &hspi3, D24_CS_GPIO_Port, D24_CS_Pin, D2_RESET_GPIO_Port, D2_RESET_Pin, D2_RS_GPIO_Port, D2_RS_Pin},
		{8, &hspi2, D31_CS_GPIO_Port, D31_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
		{9, &hspi2, D32_CS_GPIO_Port, D32_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
		{10,&hspi2, D33_CS_GPIO_Port, D33_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
		{11,&hspi2, D34_CS_GPIO_Port, D34_CS_Pin, D3_RESET_GPIO_Port, D3_RESET_Pin, D3_RS_GPIO_Port, D3_RS_Pin},
		{12,&hspi4, D41_CS_GPIO_Port, D41_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
		{13,&hspi4, D42_CS_GPIO_Port, D42_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
		{14,&hspi4, D43_CS_GPIO_Port, D43_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin},
		{15,&hspi4, D44_CS_GPIO_Port, D44_CS_Pin, D4_RESET_GPIO_Port, D4_RESET_Pin, D4_RS_GPIO_Port, D4_RS_Pin}
};


lv_display_t * Display::createDisplay() {
	return lv_lcd_custom_mipi_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_BGR, lcd_send_cmd, lcd_send_color);
}


void Display::set_active_display(uint8_t id) {
	while (Display::lcd_bus_busy)
		; /* wait until previous transfer is finished */
    ds_active = (display_state_t *)&Display::ds[id];
}


/* Initialize LCD I/O bus, reset LCD */
int32_t Display::lcd_io_init(void) {
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


void Display::lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi) {
	/* CS high */
	HAL_GPIO_WritePin(ds_active->cs_port, ds_active->cs_pin, GPIO_PIN_SET);
	lcd_bus_busy = 0;
	lv_display_flush_ready(UI::getInstance()->lcd_disp);
}


/* Platform-specific implementation of the LCD send command function. In general this should use polling transfer. */
void Display::lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd,
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
void Display::lcd_send_color(lv_display_t *disp, const uint8_t *cmd,
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


