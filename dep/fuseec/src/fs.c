#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "fs.h"

size_t load_file(const char* path, uint8_t** output_buffer)
{
    assert(output_buffer);

    *output_buffer = NULL;

    FILE* fd = fopen(path, "rb");
    if (!fd)
        return 0;

    long file_size;

    fseek(fd, 0L, SEEK_END);
    file_size = ftell(fd);
    rewind(fd);

    if (file_size < 0)
        return 0;

    size_t nfile_size = (size_t)file_size;

    if (nfile_size)
    {

        *output_buffer = malloc(nfile_size * sizeof(**output_buffer));
        fread(*output_buffer, sizeof(**output_buffer), nfile_size, fd);
        fclose(fd);
    }

    return nfile_size;
}