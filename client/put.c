#include "head.h"

void put_data(tftp_packet *opcode_information, char *mode)
{
    char file_name[256];
    char request[300];
    char ack_buffer[516];
    char read_buff[512];
    char data_buffer[516];

    FILE *fp;

    int bytes_read;
    int Ack_bytes;
    int block_no = 1;
    int request_size = 0;

    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Check connection
    if (server_fd < 0)
    {
        printf("Connect to server first\n");
        return;
    }

    // Get filename
    printf("Enter file name: ");
    scanf("%255s", file_name);

    // Open local file
    fp = fopen(file_name, "rb");

    if (fp == NULL)
    {
        perror("File open failed");
        return;
    }

    // Prepare WRQ opcode
    opcode_information->opcode = htons(WRQ);

    // Construct WRQ packet
    memcpy(request + request_size, &opcode_information->opcode, 2);
    request_size += 2;

    strcpy(request + request_size, file_name);
    request_size += strlen(file_name) + 1;

    strcpy(request + request_size, mode);
    request_size += strlen(mode) + 1;

    // Send WRQ request
    sendto(server_fd, request, request_size, 0,
           (struct sockaddr *)&serv, sizeof(serv));

    // Receive ACK block 0
    Ack_bytes = recvfrom(server_fd, ack_buffer, sizeof(ack_buffer), 0,
                         (struct sockaddr *)&server_addr, &addr_len);

    // Validate ACK
    if (Ack_bytes != 4 ||
        ntohs(*(uint16_t *)ack_buffer) != ACK ||
        ntohs(*(uint16_t *)(ack_buffer + 2)) != 0)
    {
        printf("Invalid WRQ acknowledgement\n");
        fclose(fp);
        return;
    }

    // Save server transfer address
    serv = server_addr;

    // Send file data block by block
    do
    {
        // Read data according to selected mode
        bytes_read = read_mode(fp, mode, read_buff);

        if (bytes_read < 0)
        {
            printf("File read failed\n");
            break;
        }

        // Prepare DATA packet
        opcode_information->opcode = htons(DATA);

        uint16_t packet_no = htons(block_no);

        memcpy(data_buffer, &opcode_information->opcode, 2);
        memcpy(data_buffer + 2, &packet_no, 2);
        memcpy(data_buffer + 4, read_buff, bytes_read);

        // Send DATA packet
        sendto(server_fd, data_buffer, 4 + bytes_read, 0,
               (struct sockaddr *)&serv, sizeof(serv));

        // Receive ACK packet
        Ack_bytes = recvfrom(server_fd, ack_buffer, sizeof(ack_buffer), 0,
                             NULL, NULL);

        // Validate ACK packet
        if (Ack_bytes != 4 ||
            ntohs(*(uint16_t *)ack_buffer) != ACK ||
            ntohs(*(uint16_t *)(ack_buffer + 2)) != block_no)
        {
            printf("Invalid DATA acknowledgement\n");
            break;
        }

        block_no++;

    } while (bytes_read == 512);

    fclose(fp);

    printf("PUT operation completed\n");
}