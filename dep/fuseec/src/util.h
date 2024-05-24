#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#include <stdlib.h>
#define MIN(x, y) __min(x, y)
#else
#include <sys/param.h>
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define XSTR(x) #x
#define STR(x) XSTR(x)

#define XGLUE(x, y) x##y
#define GLUE(x, y) XGLUE(x, y)


// Figuring out if we are BIG ENDIAN
#ifdef _MSC_VER
#define IS_BIG_ENDIAN (REG_DWORD == REG_DWORD_BIG_ENDIAN)
#elif defined(__BYTE_ORDER__)
#define IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#else
#warning Found no way to determine if the system is Big Endian
#warning Assuming its not
#define IS_BIG_ENDIAN 0
#endif

#ifdef _WIN32
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_16(x) _byteswap_ushort(x)
#else
// Removed for use on MacOS, not sure why
// works without it though.
//#include <byteswap.h>
#endif

#if IS_BIG_ENDIAN
#define swap_little_32(x) bswap_32(x)
#define swap_little_16(x) bswap_16(x)
#else
#define swap_little_32(x) x
#define swap_little_16(x) x
#endif

#ifdef __cplusplus
}
#endif

#endif