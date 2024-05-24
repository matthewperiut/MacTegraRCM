#ifndef TEGRA_H
#define TEGRA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <libusb.h>

typedef struct {
	libusb_device_handle* usb_handle;
    uint8_t buffer_index;
} tegra_handle;

uint64_t tegra_get_current_buffer_address(tegra_handle*);
int tegra_switch_to_high_buffer(tegra_handle*);
int tegra_switch_to_low_buffer(tegra_handle*);
int tegra_switch_buffer(tegra_handle*);
int tegra_read_device_id(tegra_handle*, uint8_t*);
int tegra_rcm_write(tegra_handle*, uint8_t*, int);
libusb_device** tegra_get_rcm_devices(libusb_context*);
int tegra_open(libusb_device*, tegra_handle**);
void tegra_close(tegra_handle*);

#ifdef __cplusplus
}
#endif

#endif