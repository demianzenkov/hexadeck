/*
 * lv_lcd_generic_mipi.h
 *
 * Generic driver for controllers adhering to the MIPI DBI/DCS specification
 *
 * Works with:
 *
 * ST7735
 * ST7789
 * ST7796
 * ILI9341
 * ILI9488 (NOTE: in SPI mode ILI9488 only supports RGB666 mode, which is currently not supported)
 *
 * any probably many more
 *
 */

#ifndef LV_LCD_GENERIC_CUSTOM__MIPI_H
#define LV_LCD_GENERIC_CUSTOM__MIPI_H

#include "lv_lcd_custom_mipi.h"
#include "src/drivers/display/lcd/lv_lcd_generic_mipi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "src/display/lv_display.h"

#if LV_USE_GENERIC_MIPI

lv_display_t * lv_lcd_custom_mipi_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                          lv_lcd_send_cmd_cb_t send_cmd_cb, lv_lcd_send_color_cb_t send_color_cb);
void lv_lcd_custom_init_controller(lv_display_t * disp, lv_lcd_flag_t flags);
void lv_lcd_custom_mipi_set_callbacks(lv_display_t * disp);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GENERIC_MIPI*/

#endif /* LV_LCD_GENERIC_CUSTOM__MIPI_H */
