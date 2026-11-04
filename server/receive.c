#include "head.h"

void receive_file(char *filename, char *mode,
                  struct sockaddr_in *client, socklen_t addr_len)
{
    FILE *fp;
    char packet[PACKET_SIZE];
    
    uint16_t value;
    uint16_t block = 0;
    int received, data_size, written;
    int done = 0;

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        send_error(client, addr_len, 2, "Cannot create file");
        perror("fopen");
        return;
    }

    /* WRQ starts with ACK block 0. */
    value = htons(ACK);
    memcpy(packet, &value, 2);
    value = htons(0);
    memcpy(packet + 2, &value, 2);

    if (sendto(server_fd, packet, 4, 0,
               (struct sockaddr *)client, addr_len) < 0)
    {
        perror("sendto ACK 0");
        fclose(fp);
        return;
    }

    while (!done)
    {
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        uint16_t opcode, packet_block;

        received = recvfrom(server_fd, packet, sizeof(packet), 0,
                            (struct sockaddr *)&sender, &sender_len);
        if (received < 4)
        {
            printf("Invalid DATA packet\n");
            break;
        }

        memcpy(&value, packet, 2);
        opcode = ntohs(value);
        memcpy(&value, packet + 2, 2);
        packet_block = ntohs(value);

        if (opcode != DATA || packet_block != (uint16_t)(block + 1))
        {
            printf("Unexpected DATA packet\n");
            break;
        }

        data_size = received - 4;
        written = write_mode(fp, mode, packet + 4, data_size);
        if (written != data_size)
        {
            send_error(client, addr_len, 3, "File write failed");
            break;
        }

        block = packet_block;

        value = htons(ACK);
        memcpy(packet, &value, 2);
        value = htons(block);
        memcpy(packet + 2, &value, 2);

        if (sendto(server_fd, packet, 4, 0,
                   (struct sockaddr *)client, addr_len) < 0)
        {
            perror("sendto ACK");
            break;
        }

        if (data_size < BLOCK_SIZE)
            done = 1;
    }

    fclose(fp);
    if (done)
        printf("PUT completed: %s\n", filename);
    else
        printf("PUT failed: %s\n", filename);
}
