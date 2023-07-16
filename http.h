#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "util.h"
#include "structures.h"

#ifndef HTTP_H
#define HTTP_H

void handle_connection(int sockfd);
void parse_request(unsigned char* request_line, struct http_request* http);
void parse_http_headers(int sockfd, unsigned char* key, unsigned char* header, struct map* headers_struct);
void get_response(int sockfd, unsigned char* resource, struct http_request* http);
void process_post(int sockfd, struct http_request* http);

#endif