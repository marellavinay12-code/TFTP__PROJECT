#include "head.h"

// Select transfer mode
int select_mode(char *mode)
{
    int choice;

    printf("\nSelect Transfer Mode\n");
    printf("1. Normal\n");
    printf("2. Netascii\n");
    printf("3. Octet\n");

    printf("Enter your choice: ");

    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input\n");
        return -1;
    }

    // Store the selected mode
    switch (choice)
    {
        case 1:
            strcpy(mode, "normal");
            break;

        case 2:
            strcpy(mode, "netascii");
            break;

        case 3:
            strcpy(mode, "octet");
            break;

        default:
            printf("Invalid mode\n");
            return -1;
    }

    printf("Selected mode: %s\n", mode);

    return 0;
}

// Read data according to selected mode
int read_mode(FILE *fp, char *mode, char *buffer)
{
    if (strcmp(mode, "normal") == 0)
        return normal_read(fp, buffer);

    if (strcmp(mode, "netascii") == 0)
        return netascii_read(fp, buffer);

    if (strcmp(mode, "octet") == 0)
        return octet_read(fp, buffer);

    return -1;
}

// Write data according to selected mode
int write_mode(FILE *fp, char *mode, char *buffer, int size)
{
    if (strcmp(mode, "normal") == 0)
        return normal_write(fp, buffer, size);

    if (strcmp(mode, "netascii") == 0)
        return netascii_write(fp, buffer, size);

    if (strcmp(mode, "octet") == 0)
        return octet_write(fp, buffer, size);

    return -1;
}