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

void JumpToBootloader(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* BOOTLOADER_H_ */