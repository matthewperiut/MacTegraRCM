#ifndef PAYLOAD_H
#define PAYLOAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "tegra.h"

void upload_payload(tegra_handle*, const uint8_t*, const size_t);
int run_payload(tegra_handle*);

#ifdef __cplusplus
}
#endif

#endif