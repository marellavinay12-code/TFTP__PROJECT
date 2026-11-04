#include "head.h"

void send_file(char *filename, char *mode,
               struct sockaddr_in *client, socklen_t addr_len)
{
    FILE *fp;
    char packet[PACKET_SIZE];
    uint16_t value;
    uint16_t block = 1;
    int data_size;
    int done = 0;

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        send_error(client, addr_len, 1, "File not found");
        perror("fopen");
        return;
    }

    while (!done)
    {
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        int ack_size;
        uint16_t opcode, ack_block;

        data_size = read_mode(fp, mode, packet + 4);
        if (data_size < 0)
        {
            send_error(client, addr_len, 3, "File read failed");
            break;
        }

        value = htons(DATA);
        memcpy(packet, &value, 2);
        value = htons(block);
        memcpy(packet + 2, &value, 2);

        if (sendto(server_fd, packet, 4 + data_size, 0,
                   (struct sockaddr *)client, addr_len) < 0)
        {
            perror("sendto DATA");
            break;
        }

        ack_size = recvfrom(server_fd, packet, sizeof(packet), 0,
                            (struct sockaddr *)&sender, &sender_len);
        if (ack_size != 4)
        {
            printf("Invalid ACK packet\n");
            break;
        }

        memcpy(&value, packet, 2);
        opcode = ntohs(value);
        memcpy(&value, packet + 2, 2);
        ack_block = ntohs(value);

        if (opcode != ACK || ack_block != block)
        {
            printf("Unexpected ACK packet\n");
            break;
        }

        if (data_size < BLOCK_SIZE)
            done = 1;
        else
            block++;
    }

    fclose(fp);
    if (done)
        printf("GET completed: %s\n", filename);
    else
        printf("GET failed: %s\n", filename);
}
