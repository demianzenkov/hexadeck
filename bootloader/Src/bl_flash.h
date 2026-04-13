/*
 * Hexadeck Bootloader - Flash Programming
 */

#ifndef BL_FLASH_H
#define BL_FLASH_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Application flash area: sectors 4-7 (0x08010000 - 0x0807FFFF = 448KB) */
#define APP_FLASH_START  0x08010000U
#define APP_FLASH_END    0x08080000U  /* one past last byte */
#define APP_FLASH_SIZE   (APP_FLASH_END - APP_FLASH_START)

/* Erase all application sectors (4-7) */
int bl_flash_erase_app(void);

/* Write data to application flash area.
 * addr must be within APP_FLASH_START..APP_FLASH_END, word-aligned.
 * data_len should be a multiple of 4 (padded internally if not). */
int bl_flash_write(uint32_t addr, const uint8_t *data, uint32_t data_len);

/* Verify application CRC32 */
uint32_t bl_flash_crc32(uint32_t addr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* BL_FLASH_H */
