#include "util.h"

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

int get_file_size(int fd) {
    if (fd == -1) return -1;
    struct stat stat_struct;
    if (fstat(fd, &stat_struct) == -1) return -1;
    return stat_struct.st_size;
}

int read_file_line(int fd, unsigned char* buffer) {
    if (fd == -1) return 0;

    unsigned char* p = buffer;
    while (read(fd, p, 1) == 1) {
        if (*p == '\r' || *p == '\n') break;
        p++;
    }

    *p = 0;
    printf("File line: %s\n", buffer);
    return strlen(buffer);
}