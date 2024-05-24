#include "fusee.h"

#include <stdlib.h>
#include <stdio.h>

#include <libusb.h>

#include "fs.h"
#include "payload.h"
#include "tegra.h"
#include "util.h"
#include "usb.h"

int fuseec(char* payload_path, std::string& message)
{
    int r = 0;

    if (libusb_init(NULL) < 0)
    {
        message = "Failed to init libusb";
        fprintf(stderr, "Failed to init libusb\n");
        return -1;
    }

    libusb_device* device;
    libusb_device** devices = tegra_get_rcm_devices(NULL);
    if (!devices || !devices[0])
    {
        message = "No Devices Found";
        fprintf(stderr, "No Devices Found\n");
        r = -1;
        return -1;
    }

    if (devices[1])
    {
        message = "Multiple Devices Found";
        fprintf(stderr, "Multiple Devices Found\n");

        size_t device_index = 0;
        while ((device = devices[device_index++]) != NULL)
        {
            struct libusb_device_descriptor desc;
            if (libusb_get_device_descriptor(device, &desc) < 0)
                continue;

            char vendor[0xFF] = "";
            char product[0xFF] = "";

            usb_get_vendor_name(device, vendor, ARRAY_LEN(vendor));
            usb_get_product_name(device, product, ARRAY_LEN(product));

            fprintf(stderr, "%zi - %04x:%04x %s %s\n", device_index, desc.idVendor, desc.idProduct, vendor, product);
        }
        size_t device_count = device_index-1;

        while (1)
        {
            fprintf(stderr, "> ");
            scanf("%zu", &device_index);

            device_index -=1;

            if (device_index < device_count)
                break;

            fprintf(stderr, "Invalid Index\n");
        }

        device = devices[device_index];
    }
    else
    {
        fprintf(stderr, "Found a device\n");
        device = devices[0];
    }

    tegra_handle* handle;

    if (tegra_open(device, &handle))
    {
        message = "Failed to open Tegra Handle";
        fprintf(stderr, "Failed to open Tegra Handle\n");
        r = -1;
        return -1;
    }

    uint8_t device_id[16];
    if (tegra_read_device_id(handle, device_id))
    {
        message = "Failed to read Device ID\nStack may already be smashed\n";
        fprintf(stderr, message.c_str());
        r = -1;
        return -1;
    }

    uint8_t* payload_buffer;
    size_t payload_size = load_file(payload_path, &payload_buffer);
    if (!payload_buffer)
    {
        message = "Failed to read Payload\nIt either doesn't exist or isn't accessible\n";
        fprintf(stderr, message.c_str());
        r = -1;
        return -1;
    }

    fprintf(stderr, "Device ID:");
    for (size_t i = 0; i < ARRAY_LEN(device_id); ++i)
    {
        fprintf(stderr, " %02X", device_id[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Uploading Payload\n");
    upload_payload(handle, payload_buffer, payload_size);
    free(payload_buffer);

    tegra_switch_to_high_buffer(handle);

    fprintf(stderr, "Running payload\n");

    if (run_payload(handle) < 0)
    {
        fprintf(stderr, "Something went wrong while running payload\n");
    }

handle_exit:
    tegra_close(handle);
device_exit:
    libusb_free_device_list(devices, 1);
exit:
    libusb_exit(NULL);
    return r;
}
