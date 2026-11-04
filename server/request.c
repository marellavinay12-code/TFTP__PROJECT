#include "head.h"

void handle_request(char *buffer, int size,
                    struct sockaddr_in *client, socklen_t addr_len)
{
    uint16_t net_opcode;
    uint16_t opcode;
    char filename[256];
    char mode[16];
    int pos = 2;
    int n = 0;

    if (size < 4)
    {
        send_error(client, addr_len, 4, "Malformed request");
        return;
    }

    memcpy(&net_opcode, buffer, 2);
    opcode = ntohs(net_opcode);

    while (pos < size && buffer[pos] != '\0' && n < 255)
        filename[n++] = buffer[pos++];
    filename[n] = '\0';

    if (pos >= size || buffer[pos] != '\0' || n == 0)
    {
        send_error(client, addr_len, 4, "Invalid filename");
        return;
    }

    pos++;
    n = 0;
    while (pos < size && buffer[pos] != '\0' && n < 15)
        mode[n++] = (char)tolower((unsigned char)buffer[pos++]);
    mode[n] = '\0';

    if (pos >= size || buffer[pos] != '\0' || n == 0)
    {
        send_error(client, addr_len, 4, "Invalid mode");
        return;
    }

    if (strcmp(mode, "normal") != 0 &&
        strcmp(mode, "netascii") != 0 &&
        strcmp(mode, "octet") != 0)
    {
        send_error(client, addr_len, 4, "Unsupported transfer mode");
        return;
    }

    printf("Request: %s | File: %s | Mode: %s\n",
           opcode == WRQ ? "WRQ" : opcode == RRQ ? "RRQ" : "UNKNOWN",
           filename, mode);

    if (opcode == WRQ)
        receive_file(filename, mode, client, addr_len);
    else if (opcode == RRQ)
        send_file(filename, mode, client, addr_len);
    else
        send_error(client, addr_len, 4, "Unknown request opcode");
}

void send_error(struct sockaddr_in *client, socklen_t addr_len,
                uint16_t code, const char *message)
{
    char packet[4 + 128];
    uint16_t value;
    size_t len = strlen(message) + 1;

    value = htons(ERROR);
    memcpy(packet, &value, 2);
    value = htons(code);
    memcpy(packet + 2, &value, 2);
    if (len > sizeof(packet) - 4)
        len = sizeof(packet) - 4;
    memcpy(packet + 4, message, len);

    sendto(server_fd, packet, 4 + len, 0,
           (struct sockaddr *)client, addr_len);
}
