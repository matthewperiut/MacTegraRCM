#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "tegra.h"
#include "usb.h"
#include "util.h"


static const uint64_t COPY_BUFFER_ADDRESSES[] = {0x40005000, 0x40009000};

#define NVIDIA_VID 0x0955
static const uint16_t known_pids[] = {
    0x7820, // T20
    0x7030, 0x7130, 0x7330, // T30
    0x7335, 0x7535, // T114
    0x7140, 0x7f40, // T124
    0x7F13, // T132
    0x7321, 0x7721 // T210
};

static void toggle_buffer(tegra_handle* device_handle)
{
    device_handle->buffer_index = 1 - device_handle->buffer_index;
}

uint64_t tegra_get_current_buffer_address(tegra_handle* device_handle)
{
    return COPY_BUFFER_ADDRESSES[device_handle->buffer_index];
}

int tegra_switch_to_high_buffer(tegra_handle* device_handle)
{
    int r = 0;
    if (tegra_get_current_buffer_address(device_handle) != COPY_BUFFER_ADDRESSES[1])
        r = tegra_switch_buffer(device_handle);
    return r;
}

int tegra_switch_to_low_buffer(tegra_handle* device_handle)
{
    int r = 0;
    if (tegra_get_current_buffer_address(device_handle) != COPY_BUFFER_ADDRESSES[0])
        r = tegra_switch_buffer(device_handle);
    return r;
}

int tegra_switch_buffer(tegra_handle* device_handle)
{
    uint8_t data[0x1000] = { 0 };
    return tegra_rcm_write(device_handle, data, ARRAY_LEN(data));
}

int tegra_read_device_id(tegra_handle* device_handle, uint8_t* buffer)
{
    assert(device_handle && device_handle->usb_handle);
    return libusb_bulk_transfer(device_handle->usb_handle, 0x81, buffer, 16, NULL, 1000);
}

int tegra_rcm_write(tegra_handle* device_handle, uint8_t* data, int length)
{
    assert(device_handle && device_handle->usb_handle);

    const int packet_size = 0x1000;
    uint8_t* chunk;
    int chunk_size;
    int r;

    while (length)
    {
        chunk = data;
        chunk_size = MIN(length, packet_size);

        length -= chunk_size;
        data += chunk_size;

        toggle_buffer(device_handle);

        r = libusb_bulk_transfer(device_handle->usb_handle, 0x01, chunk, chunk_size, NULL, 1000);
        if (r < 0)
            return r;
    }

    return 0;
}

/**
 * Get all connected Nvidia Tegra Recovery Systems
 */
libusb_device** tegra_get_rcm_devices(libusb_context* ctx)
{
    libusb_device** all_devices = NULL;
    libusb_device** new_devices;
    size_t device_count = 0;

    for (size_t i = 0; i < ARRAY_LEN(known_pids); ++i)
    {
        size_t new_count = usb_get_all_devices_with_vid_pid(ctx, &new_devices, NVIDIA_VID, known_pids[i]);
        if (new_devices)
        {
            // Reference the device so its not cleaned up
            for (size_t j = 0; j < new_count; ++j)
            {
                libusb_ref_device(new_devices[j]);
            }

            device_count += new_count;
            all_devices = realloc(all_devices, device_count * sizeof(*all_devices));

            memcpy(all_devices + (device_count-new_count), new_devices, new_count * sizeof(*all_devices));
        }

        libusb_free_device_list(new_devices, 1);
    }

    if (all_devices)
    {
        all_devices = realloc(all_devices, sizeof(*all_devices) * (device_count+1));
        all_devices[device_count] = NULL;
    }

    return all_devices;
}

int tegra_open(libusb_device* device, tegra_handle** device_handle)
{
    libusb_device_handle* handle = NULL;

    int r = libusb_open(device, &handle);

    if (!r)
    {
        r = libusb_claim_interface(handle, 0);
        if (r < 0 && handle)
        {
            libusb_close(handle);
            handle = NULL;
        }
        tegra_handle* pdevice_handle = malloc(sizeof(tegra_handle));
        pdevice_handle->buffer_index = 0;
        pdevice_handle->usb_handle = handle;

        *device_handle = pdevice_handle;    
    }

    return r;
}

void tegra_close(tegra_handle* device_handle)
{
    libusb_release_interface(device_handle->usb_handle, 0);
    libusb_close(device_handle->usb_handle);
    device_handle->usb_handle = NULL;
    free(device_handle);
}