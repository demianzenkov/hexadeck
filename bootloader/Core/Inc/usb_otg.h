#ifndef __USB_OTG_H__
#define __USB_OTG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

void MX_USB_OTG_FS_PCD_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_OTG_H__ */
