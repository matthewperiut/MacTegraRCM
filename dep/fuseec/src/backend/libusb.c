#include <libusb.h>

#include "backend.h"

int libusb_trigger_vulnerability(tegra_handle* handle, uint16_t length)
{
    return libusb_control_transfer(
        handle->usb_handle,
        LIBUSB_ENDPOINT_DIR_MASK | LIBUSB_RECIPIENT_ENDPOINT,
        LIBUSB_REQUEST_GET_STATUS,
        0, 0,
        NULL, length,
        1000
    );
}
