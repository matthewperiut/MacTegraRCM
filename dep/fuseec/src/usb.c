#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libusb.h>
#ifdef HAVE_UDEV
#include <libudev.h>
#endif

#include "usb.h"
#include "util.h"

#ifdef HAVE_USB_DATABASE
extern uint8_t usb_ids[];
extern size_t usb_ids_size;
#endif

/**
 * Get a list of all devices with a specific Vendor and Product ID
 *
 * Behaves the same as libusb_get_device_list()
 * Freed by libusb_free_device_list
 */
size_t usb_get_all_devices_with_vid_pid(libusb_context* ctx, libusb_device*** list, uint16_t vid, uint16_t pid)
{
    libusb_device** all_devices;
    libusb_device** found_devices = NULL;
    libusb_device* device;
    size_t device_count = 0;

    if (libusb_get_device_list(ctx, &all_devices) < 0)
        goto out;

    size_t index = 0;

    while ((device = all_devices[index++]) != NULL)
    {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(device, &desc) < 0)
            continue;

        if (desc.idVendor == vid && desc.idProduct == pid)
        {
            device_count += 1;
            found_devices = realloc(found_devices, sizeof(libusb_device*) * device_count);
            found_devices[device_count-1] = libusb_ref_device(device);
        }
    }

    if (found_devices)
    {
        found_devices = realloc(found_devices, sizeof(*found_devices) * (device_count+1));
        found_devices[device_count] = NULL;
    }

    libusb_free_device_list(all_devices, 1);

out:
    *list = found_devices;
    return device_count;
}

enum lookup_type
{
    LOOKUP_VENDOR,
    LOOKUP_PRODUCT,
};

static char* usb_device_lookup(uint16_t vid, uint16_t pid, const enum lookup_type type)
{
    char* retval = NULL;

#ifdef HAVE_UDEV

    const char* entry_key = NULL;
    switch (type)
    {
        case LOOKUP_VENDOR:
            entry_key = "ID_VENDOR_FROM_DATABASE";
            break;

        case LOOKUP_PRODUCT:
            entry_key = "ID_MODEL_FROM_DATABASE";
            break;

        default:
            break;
    }

    if (entry_key)
    {
        char modalias[64];
        sprintf(modalias, "usb:v%04Xp%04X*", vid, pid);

        struct udev_list_entry *entry;

        struct udev *udev = udev_new();

        if (udev)
        {
            struct udev_hwdb *hwdb = udev_hwdb_new(udev);

            if (hwdb)
            {
                udev_list_entry_foreach(entry, udev_hwdb_get_properties_list_entry(hwdb, modalias, 0))
                {

                    if (strcmp(udev_list_entry_get_name(entry), entry_key) == 0)
                    {
                        retval = strdup(udev_list_entry_get_value(entry));
                        break;
                    }
                }

                hwdb = udev_hwdb_unref(hwdb);
            }
            udev = udev_unref(udev);
        }
    }
#endif

#ifdef HAVE_USB_DATABASE
    if (!retval)
    {
        char vendor_str[4+2+1];
        sprintf(vendor_str, "%04x  ", vid);
        char product_str[1+4+2+1];
        sprintf(product_str, "\t%04x  ", pid);

        char* database_vendor = NULL;
        char* database_product = NULL;

        char* out = NULL;

        uint8_t* head = usb_ids;
        uint8_t* tail;

        switch (type)
        {
            case LOOKUP_PRODUCT:
                database_vendor = usb_device_lookup(vid, pid, LOOKUP_VENDOR);

                // No Vendor = No Product
                if (database_vendor)
                {
                    while (*head && strncmp((char*)head, product_str, ARRAY_LEN(product_str)-1))
                    {
                        while (*head && *head != '\n') ++head;
                        while (*head && *head == '\n') ++head;
                    }

                    if (*head)
                    {
                        // We found the product
                        head += ARRAY_LEN(product_str)-1;
                        tail = head;
                        while (*head && *head != '\n') ++head;

                        {
                            size_t product_size = (size_t)(head-tail);
                            database_product = malloc(product_size+1);
                            strncpy(database_product, (char*)tail, product_size);
                            database_product[product_size] = '\0';
                        }
                    }

                }

                break;

            case LOOKUP_VENDOR:
                while (*head && strncmp((char*)head, vendor_str, ARRAY_LEN(vendor_str)-1))
                {
                    while (*head && *head != '\n') ++head;
                    while (*head && *head == '\n') ++head;
                }

                if (*head)
                {
                    // We found the vendor
                    head += ARRAY_LEN(vendor_str)-1;
                    tail = head;
                    while (*head && *head != '\n') ++head;

                    {
                        size_t vendor_size = (size_t)(head-tail);
                        database_vendor = malloc(vendor_size+1);
                        strncpy(database_vendor, (char*)tail, vendor_size);
                        database_vendor[vendor_size] = '\0';
                    }
                }

                break;

            default:
                break;
        }

        if (database_vendor)
        {
            size_t out_size = strlen(database_vendor);

            out = malloc(out_size+1);
            strcpy(out, database_vendor);

            if (database_product)
            {
                out_size += strlen(database_product);

                out = realloc(out, out_size+1+1);
                strcat(out, " ");
                strcat(out, database_product);

                free(database_product);
            }

            free(database_vendor);
        }

        if (out)
            retval = out;
    }

#endif

    return retval;
}

int usb_get_vendor_name(libusb_device* device, char* data, int length)
{
    struct libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(device, &desc) < 0)
        return 0;

    char* lookup_result = usb_device_lookup(desc.idVendor, desc.idProduct, LOOKUP_VENDOR);

    if (lookup_result)
    {
        strncpy(data, lookup_result, (size_t)length);
        data[length-1] = '\0';

        free(lookup_result);
        return (int)strlen(data);
    }

    // Ask the device what it is
    libusb_device_handle* handle;
    if (libusb_open(device, &handle) >= 0)
    {
        length = libusb_get_string_descriptor_ascii(handle, desc.iProduct, (uint8_t*)data, length);
        libusb_close(handle);

        return length;
    }

    return 0;
}

int usb_get_product_name(libusb_device* device, char* data, int length)
{
    struct libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(device, &desc) < 0)
        return 0;

    char* lookup_result = usb_device_lookup(desc.idVendor, desc.idProduct, LOOKUP_PRODUCT);

    if (lookup_result)
    {
        strncpy(data, lookup_result, (size_t)length);
        data[length-1] = '\0';

        free(lookup_result);
        return (int)strlen(data);
    }

    // Ask the device what it is
    libusb_device_handle* handle;
    if (libusb_open(device, &handle) >= 0)
    {
        length = libusb_get_string_descriptor_ascii(handle, desc.iProduct, (uint8_t*)data, length);
        libusb_close(handle);

        return length;
    }

    return 0;
}
