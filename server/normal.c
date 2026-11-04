#include "head.h"

int normal_read(FILE *fp, char *buffer)
{
    return (int)fread(buffer, 1, BLOCK_SIZE, fp);
}

int normal_write(FILE *fp, char *buffer, int size)
{
    return (int)fwrite(buffer, 1, size, fp);
}
