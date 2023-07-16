#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define EOL "\r\n\r\n"
#define EOL_SIZE 2

void send_string(int sockfd, unsigned char* buffer);
int recv_line(int sockfd, unsigned char* buffer);

void handle_connection(int sockfd);
int get_file_size(int fd);

int main(void) {
  int sockfd, client_fd;
  struct sockaddr_in host_addr, client_addr;
  int yes = 1, recv_length;
  socklen_t size;
  
  host_addr.sin_port = htons(80);
  host_addr.sin_addr.s_addr = 0;
  host_addr.sin_family = AF_INET;
  memset(&(host_addr.sin_zero), 0, 8);

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    puts("Error init socket\n");
    return 0;
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    puts("Error setting socket option\n");
    return 0;
  }

  if (bind(sockfd, (struct sockaddr*)&host_addr, sizeof(struct sockaddr)) == -1)
    {
      puts("Error binding the socket\n");
      return 0;
    }

  if (listen(sockfd, 5) == -1) {
    puts("Error listening to clients\n");
    return 0;
  }

  while (true) {
	  puts("Waiting for HTTP request...\n");
    size = sizeof(struct sockaddr_in);
    client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &size);
    if (client_fd == -1) {
      puts("Error accepting the connection\n");
      return 0;
    }

    printf("Got connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    handle_connection(client_fd);
    close(client_fd);
  }
  
  return 0;
}

void handle_connection(int sockfd) {
  char request[255], response[1024];
  int len;
  unsigned char* p;

  len = recv_line(sockfd, request);
  printf("Received request: %s\nSize: %d bytes\n", request, len);
  p = strstr(request, " HTTP/");

  if (p == NULL) {
      puts("HTTP REQUEST NOT RECOGNISED\n");
  }

  *p = 0;
  p = NULL;

  p = request + 4;
  printf("URL: %s\n", p);
}

void send_string(int sockfd, unsigned char* buffer) {
  unsigned char* p = buffer;
  int length = strlen(buffer);
  int sent;
  while (length > 0) {
    sent = send(sockfd, p, length, 0);
    length -= sent;
    p += sent;
  }
}

int recv_line(int sockfd, unsigned char* buffer) {
  unsigned char* p = buffer;
  int eol = 0;
  while (recv(sockfd, p, 1, 0) == 1)  {
    if (*p == EOL[eol]) {
      eol++;
      if (eol == EOL_SIZE) {
        *(p + 1 - EOL_SIZE) = 0;
        return strlen(buffer);
      }
    } else {
      eol = 0;
    }

    p++;
  }
  
  return 0;
}

