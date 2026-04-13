/*
 * Hexadeck Bootloader - Flash Programming Implementation
 */

#include "bl_flash.h"
#include <string.h>

/* STM32F446 sector layout */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* 16 KB */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* 16 KB */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* 16 KB */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* 16 KB */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* 64 KB */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* 128 KB */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* 128 KB */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* 128 KB */

int bl_flash_erase_app(void)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error = 0;

    /* Erase sectors 4 through 7 (application area) */
    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase_init.Sector = FLASH_SECTOR_4;
    erase_init.NbSectors = 4; /* Sectors 4, 5, 6, 7 */

    if (HAL_FLASHEx_Erase(&erase_init, &sector_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;
}

int bl_flash_write(uint32_t addr, const uint8_t *data, uint32_t data_len)
{
    if (addr < APP_FLASH_START || (addr + data_len) > APP_FLASH_END) {
        return -1;
    }

    HAL_FLASH_Unlock();

    /* Program data word by word (32-bit) */
    for (uint32_t i = 0; i < data_len; i += 4) {
        uint32_t word = 0xFFFFFFFF;
        uint32_t remain = data_len - i;
        if (remain >= 4) {
            memcpy(&word, &data[i], 4);
        } else {
            /* Handle last partial word (pad with 0xFF) */
            memcpy(&word, &data[i], remain);
        }
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return -2;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

/* Simple CRC32 (same polynomial as standard CRC32) */
uint32_t bl_flash_crc32(uint32_t addr, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *p = (const uint8_t *)addr;

    for (uint32_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }

    return crc ^ 0xFFFFFFFF;
}
