/*
 * display.h
 *
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "main.h"
#include "cmsis_os.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define LCD_H_RES       132
//#define LCD_V_RES       162
#define BUS_SPI1_POLL_TIMEOUT 0x1000U


typedef struct {
	uint8_t id;
	SPI_HandleTypeDef * hspi;
	GPIO_TypeDef * cs_port;
	uint16_t cs_pin;
	GPIO_TypeDef * reset_port;
	uint16_t reset_pin;
	GPIO_TypeDef * rs_port;
	uint16_t rs_pin;
} display_state_t;

int32_t lcd_io_init(void);
void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

void set_active_display(uint8_t id);

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /* DISPLAY_H_ */
