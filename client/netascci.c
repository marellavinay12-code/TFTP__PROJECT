#include "head.h"

// Read and convert local data into Netascii
int netascii_read(FILE *fp, char *buffer)
{
    int ch;
    int count = 0;

    while (count < 512)
    {
        ch = fgetc(fp);

        if (ch == EOF)
            break;

        if (ch == '\n')
        {
            if (count + 2 > 512)
            {
                ungetc(ch, fp);
                break;
            }

            buffer[count++] = '\r';
            buffer[count++] = '\n';
        }
        else if (ch == '\r')
        {
            if (count + 2 > 512)
            {
                ungetc(ch, fp);
                break;
            }

            buffer[count++] = '\r';
            buffer[count++] = '\0';
        }
        else
        {
            buffer[count++] = ch;
        }
    }

    return count;
}

// Convert received Netascii data into local data
int netascii_write(FILE *fp, char *buffer, int size)
{
    static int previous_cr = 0;

    int i;
    int count = 0;

    for (i = 0; i < size; i++)
    {
        if (previous_cr)
        {
            if (buffer[i] == '\n')
                count += (fputc('\n', fp) != EOF);
            else if (buffer[i] == '\0')
                count += (fputc('\r', fp) != EOF);
            else
            {
                count += (fputc('\r', fp) != EOF);
                count += (fputc(buffer[i], fp) != EOF);
            }

            previous_cr = 0;
        }
        else if (buffer[i] == '\r')
        {
            previous_cr = 1;
        }
        else
        {
            count += (fputc(buffer[i], fp) != EOF);
        }
    }

    return count;
}