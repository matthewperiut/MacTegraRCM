#ifndef USB_H
#define USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <libusb.h>

size_t usb_get_all_devices_with_vid_pid(libusb_context*, libusb_device***, uint16_t, uint16_t);
int usb_get_vendor_name(libusb_device*, char*, int);
int usb_get_product_name(libusb_device*, char*, int);

#ifdef __cplusplus
}
#endif

#endif