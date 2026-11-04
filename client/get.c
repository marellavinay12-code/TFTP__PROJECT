#include "head.h"

void get_data(tftp_packet *opcode_information, char *mode)
{
    char file_name[256];
    char request[300];
    char data_buffer[516];

    FILE *fp = NULL;

    int request_size = 0;
    int data_bytes;
    int bytes_read;
    int block_no = 1;

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

    // Prepare RRQ opcode
    opcode_information->opcode = htons(RRQ);

    // Construct RRQ packet
    memcpy(request + request_size, &opcode_information->opcode, 2);
    request_size += 2;

    strcpy(request + request_size, file_name);
    request_size += strlen(file_name) + 1;

    strcpy(request + request_size, mode);
    request_size += strlen(mode) + 1;

    // Send RRQ request
    sendto(server_fd, request, request_size, 0,
           (struct sockaddr *)&serv, sizeof(serv));

    // Receive DATA packets
    do
    {
        data_bytes = recvfrom(server_fd, data_buffer, sizeof(data_buffer), 0,
                              (struct sockaddr *)&server_addr, &addr_len);

        // Validate DATA packet size
        if (data_bytes < 4 || data_bytes > 516)
        {
            printf("Invalid DATA packet\n");
            break;
        }

        // Validate DATA opcode
        if (ntohs(*(uint16_t *)data_buffer) != DATA)
        {
            printf("Invalid DATA opcode\n");
            break;
        }

        // Validate block number
        if (ntohs(*(uint16_t *)(data_buffer + 2)) != block_no)
        {
            printf("Invalid DATA block number\n");
            break;
        }

        // Open file for writing
        if (fp == NULL)
        {
            fp = fopen(file_name, "wb");

            if (fp == NULL)
            {
                perror("File open failed");
                break;
            }

            // Save server transfer address
            serv = server_addr;
        }

        // Calculate received data size
        bytes_read = data_bytes - 4;

        // Write data according to selected mode
        if (write_mode(fp, mode, data_buffer + 4, bytes_read) < 0)
        {
            printf("File write failed\n");
            break;
        }

        // Prepare ACK packet
        uint16_t ack_opcode = htons(ACK);
        uint16_t ack_block = htons(block_no);

        memcpy(data_buffer, &ack_opcode, 2);
        memcpy(data_buffer + 2, &ack_block, 2);

        // Send ACK packet
        sendto(server_fd, data_buffer, 4, 0,
               (struct sockaddr *)&serv, sizeof(serv));

        block_no++;

    } while (bytes_read == 512);

    if (fp != NULL)
    {
        fclose(fp);
        printf("GET operation completed\n");
    }
}