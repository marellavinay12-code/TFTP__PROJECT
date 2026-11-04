#include "head.h"

int octet_read(FILE *fp, char *buffer)
{
    return (int)fread(buffer, 1, BLOCK_SIZE, fp);
}

int octet_write(FILE *fp, char *buffer, int size)
{
    return (int)fwrite(buffer, 1, size, fp);
}
