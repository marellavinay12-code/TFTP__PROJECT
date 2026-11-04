#include "head.h"

/*
 * Basic Netascii conversion:
 * local LF -> network CR LF
 * local CR -> network CR NUL
 *
 * This simple implementation keeps one pending encoded byte between calls.
 * It is suitable for the basic classroom transfer flow, but is not a full
 * production-grade TFTP implementation.
 */

static int pending_byte = -1;

int netascii_read(FILE *fp, char *buffer)
{
    int count = 0;
    int ch;

    while (count < BLOCK_SIZE)
    {
        if (pending_byte != -1)
        {
            buffer[count++] = (char)pending_byte;
            pending_byte = -1;
            continue;
        }

        ch = fgetc(fp);
        if (ch == EOF)
            break;

        if (ch == '\n')
        {
            buffer[count++] = '\r';
            if (count < BLOCK_SIZE)
                buffer[count++] = '\n';
            else
                pending_byte = '\n';
        }
        else if (ch == '\r')
        {
            buffer[count++] = '\r';
            if (count < BLOCK_SIZE)
                buffer[count++] = '\0';
            else
                pending_byte = '\0';
        }
        else
        {
            buffer[count++] = (char)ch;
        }
    }

    if (count == 0 && ferror(fp))
        return -1;

    return count;
}

int netascii_write(FILE *fp, char *buffer, int size)
{
    int i = 0;
    int written = 0;

    while (i < size)
    {
        if (buffer[i] == '\r' && i + 1 < size)
        {
            if (buffer[i + 1] == '\n')
            {
                if (fputc('\n', fp) == EOF)
                    return -1;
                i += 2;
            }
            else if (buffer[i + 1] == '\0')
            {
                if (fputc('\r', fp) == EOF)
                    return -1;
                i += 2;
            }
            else
            {
                if (fputc('\r', fp) == EOF)
                    return -1;
                i++;
            }
            written = i;
        }
        else
        {
            if (fputc((unsigned char)buffer[i], fp) == EOF)
                return -1;
            i++;
            written = i;
        }
    }

    return written == size ? size : -1;
}
