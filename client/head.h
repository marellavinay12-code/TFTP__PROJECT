#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5

extern int server_fd;
extern struct sockaddr_in serv;

typedef struct
{
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)

    union
    {
        struct
        {
            char filename[256];
            char mode[16];// Typically "Normal"
        } request; // RRQ and WRQ

        struct
        {
            uint16_t packet_no;
            uint16_t size;
            char data[512];
        } data_packet;// DATA

        struct
        {
            uint16_t size;
            uint16_t packet_no;
        } ack_packet;// ACK

        struct
        {
            uint16_t error_code;
            char error_msg[512];
        } error_packet;// ERROR

    } body;

} tftp_packet;

// Client functions
void connect_server(tftp_packet *opcode_information);
void put_data(tftp_packet *opcode_information, char *mode);
void get_data(tftp_packet *opcode_information, char *mode);

// Mode functions
int select_mode(char *mode);
int read_mode(FILE *fp, char *mode, char *buffer);
int write_mode(FILE *fp, char *mode, char *buffer, int size);

// Normal mode
int normal_read(FILE *fp, char *buffer);
int normal_write(FILE *fp, char *buffer, int size);

// Netascii mode
int netascii_read(FILE *fp, char *buffer);
int netascii_write(FILE *fp, char *buffer, int size);

// Octet mode
int octet_read(FILE *fp, char *buffer);
int octet_write(FILE *fp, char *buffer, int size);

#endif