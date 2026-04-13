/*
 * bootloader.h
 *
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "main.h"
#include "cmsis_os.h"
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Jump to STM32 system bootloader (ROM DFU at 0x1FFF0000) */
void JumpToBootloader(void);

/* Enter custom MIDI bootloader (resets into bootloader update mode) */
void EnterCustomBootloader(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* BOOTLOADER_H_ */