#include "head.h"

// Read binary data in octet mode
int octet_read(FILE *fp, char *buffer)
{
    return fread(buffer, 1, 512, fp);
}

// Write binary data in octet mode
int octet_write(FILE *fp, char *buffer, int size)
{
    return fwrite(buffer, 1, size, fp);
}