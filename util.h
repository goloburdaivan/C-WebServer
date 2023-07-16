#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<fcntl.h>

#define EOL "\r\n\r\n"
#define EOL_SIZE 2
#define WEBROOT "./webroot"

#ifndef UTIL_H
#define UTIL_H

void send_string(int sockfd, unsigned char* buffer);
int recv_line(int sockfd, unsigned char* buffer);
int get_file_size(int fd);
int read_file_line(int fd, unsigned char* buffer);

#endif