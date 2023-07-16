#include<stdio.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "http.h"
#include "util.h"
#include "structures.h"

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
