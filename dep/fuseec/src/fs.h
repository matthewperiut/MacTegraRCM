#ifndef FS_H
#define FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

size_t load_file(const char* path, uint8_t** output_buffer);

#ifdef __cplusplus
}
#endif

#endif