/*
 * Hexadeck Bootloader - Main Header
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* MIDI port configuration (required by USB MIDI library) */
#define MIDI_IN_PORTS_NUM   0x01
#define MIDI_OUT_PORTS_NUM  0x01

/* Bootloader constants */
#define APP_ADDRESS             ((uint32_t)0x08010000)  /* Application start address (after 64KB bootloader) */
#define BOOTLOADER_UPDATE_MAGIC ((uint32_t)0xB00710AD)  /* Magic value in backup register to request update */

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
