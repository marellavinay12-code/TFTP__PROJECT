#include "head.h"

int read_mode(FILE *fp, const char *mode, char *buffer)
{
    if (strcmp(mode, "normal") == 0)
        return normal_read(fp, buffer);
    if (strcmp(mode, "octet") == 0)
        return octet_read(fp, buffer);
    if (strcmp(mode, "netascii") == 0)
        return netascii_read(fp, buffer);

    return -1;
}

int write_mode(FILE *fp, const char *mode, char *buffer, int size)
{
    if (strcmp(mode, "normal") == 0)
        return normal_write(fp, buffer, size);
    if (strcmp(mode, "octet") == 0)
        return octet_write(fp, buffer, size);
    if (strcmp(mode, "netascii") == 0)
        return netascii_write(fp, buffer, size);

    return -1;
}
