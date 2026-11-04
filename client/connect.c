
#include "head.h"
#include <sys/time.h>

// Global variables accessible from PUT and GET
int server_fd = -1;
struct sockaddr_in serv;

void connect_server(tftp_packet *opcode_information)
{
    (void)opcode_information;

    char server_ip_adders[100];
    int port_number;

    // Read server IP address
    printf("Enter the server IP address\n");
    scanf("%99s", server_ip_adders);

    // Initialize server address
    memset(&serv, 0, sizeof(serv));

    // Validate IP address
    if (inet_pton(AF_INET, server_ip_adders, &serv.sin_addr) != 1)
    {
        printf("Invalid server IP address\n");
        return;
    }

    // Read port number
    printf("Enter the port number\n");

    if (scanf("%d", &port_number) != 1)
    {
        printf("Invalid port number\n");
        return;
    }

    // Validate port number
    if (port_number < 1024 || port_number > 65535)
    {
        printf("Port number must be between 1024 and 65535\n");
        return;
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(port_number);

    // Create UDP socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        return;
    }

    printf("UDP socket created successfully\n");

    // Set receive timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed");
        close(server_fd);
        server_fd = -1;
        return;
    }

    // Send READY request
    char message[] = "READY";
    char response[10];

    if (sendto(server_fd, message, strlen(message), 0,
               (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("sendto failed");
        close(server_fd);
        server_fd = -1;
        return;
    }

    // Wait for ACK
    int bytes = recvfrom(server_fd, response, sizeof(response), 0,
                         NULL, NULL);

    if (bytes != 3 || memcmp(response, "ACK", 3) != 0)
    {
        printf("\nERROR: Server is not running!\n");
        printf("Please start the server first.\n");

        close(server_fd);
        server_fd = -1;
        return;
    }

    printf("Server is ready. ACK received!\n");
    printf("Server IP: %s\n", server_ip_adders);
    printf("Server port: %d\n", port_number);
}
