/*
 * Hexadeck Bootloader - USB Composite Configuration
 * MIDI-only mode (no CDC ACM, no HID)
 */

#ifndef __AL94__I_CUBE_USBD_COMPOSITE_CONF__H__
#define __AL94__I_CUBE_USBD_COMPOSITE_CONF__H__

#ifdef __cplusplus
 extern "C" {
#endif

/*---------- _USBD_USE_HS  -----------*/
#define _USBD_USE_HS      false

/*---------- _USBD_USE_CDC_ACM  -----------*/
#define _USBD_USE_CDC_ACM      false

/*---------- _USBD_CDC_ACM_COUNT  -----------*/
#define _USBD_CDC_ACM_COUNT      0

/*---------- _USBD_USE_CDC_RNDIS  -----------*/
#define _USBD_USE_CDC_RNDIS      false

/*---------- _USBD_USE_CDC_ECM  -----------*/
#define _USBD_USE_CDC_ECM      false

/*---------- _USBD_USE_HID_MOUSE  -----------*/
#define _USBD_USE_HID_MOUSE      false

/*---------- _USBD_USE_HID_KEYBOARD  -----------*/
#define _USBD_USE_HID_KEYBOARD      false

/*---------- _USBD_USE_HID_CUSTOM  -----------*/
#define _USBD_USE_HID_CUSTOM      false

/*---------- _USBD_USE_UAC_MIC  -----------*/
#define _USBD_USE_UAC_MIC      false

/*---------- _USBD_USE_UAC_SPKR  -----------*/
#define _USBD_USE_UAC_SPKR      false

/*---------- _USBD_USE_UVC  -----------*/
#define _USBD_USE_UVC      false

/*---------- _USBD_USE_MSC  -----------*/
#define _USBD_USE_MSC      false

/*---------- _USBD_USE_DFU  -----------*/
#define _USBD_USE_DFU      false

/*---------- _USBD_USE_PRNTR  -----------*/
#define _USBD_USE_PRNTR      false

/*---------- _USBD_USE_MIDI  -----------*/
#define _USBD_USE_MIDI      true

/*---------- _STM32F1_DEVICE  -----------*/
#define _STM32F1_DEVICE      false

#ifdef __cplusplus
}
#endif

#endif /*__ AL94__I_CUBE_USBD_COMPOSITE_CONF__H_H */
