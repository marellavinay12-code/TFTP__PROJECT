#include "head.h"

// Read file data in normal mode
int normal_read(FILE *fp, char *buffer)
{
    return fread(buffer, 1, 512, fp);
}

// Write received data in normal mode
int normal_write(FILE *fp, char *buffer, int size)
{
    return fwrite(buffer, 1, size, fp);
}