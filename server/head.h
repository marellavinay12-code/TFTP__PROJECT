
#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5

#define PORT 5000
#define BLOCK_SIZE 512
#define PACKET_SIZE 516

extern int server_fd;
extern struct sockaddr_in serv;

void create_server(void);

void handle_request(char *buffer, int size,
                    struct sockaddr_in *client, socklen_t addr_len);

void receive_file(char *filename, char *mode,
                  struct sockaddr_in *client, socklen_t addr_len);

void send_file(char *filename, char *mode,
               struct sockaddr_in *client, socklen_t addr_len);

int read_mode(FILE *fp, const char *mode, char *buffer);
int write_mode(FILE *fp, const char *mode, char *buffer, int size);

int normal_read(FILE *fp, char *buffer);
int normal_write(FILE *fp, char *buffer, int size);

int octet_read(FILE *fp, char *buffer);
int octet_write(FILE *fp, char *buffer, int size);

int netascii_read(FILE *fp, char *buffer);
int netascii_write(FILE *fp, char *buffer, int size);

void send_error(struct sockaddr_in *client, socklen_t addr_len,
                uint16_t code, const char *message);

#endif
