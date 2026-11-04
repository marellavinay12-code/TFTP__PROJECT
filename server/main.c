#include "head.h"

int server_fd;
struct sockaddr_in serv;

int main(void)
{
    char buffer[PACKET_SIZE];
    struct sockaddr_in client;
    socklen_t addr_len;
    int bytes;

    create_server();
    printf("TFTP server waiting on UDP port %d...\n", PORT);

    while (1)
    {
        addr_len = sizeof(client);
        bytes = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)&client, &addr_len);

        if (bytes < 0)
        {
            perror("recvfrom");
            continue;
        }
        if (bytes == 5 && memcmp(buffer, "READY", 5) == 0)
        {
            sendto(server_fd, "ACK", 3, 0,
                   (struct sockaddr *)&client, addr_len);

            printf("Client connected. ACK sent.\n");
            continue;
        }
        handle_request(buffer, bytes, &client, addr_len);
    }

    close(server_fd);
    return 0;
}
