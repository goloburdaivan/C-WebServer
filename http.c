#include "http.h"
#include "data_structures.h"

void register_handler(struct handlers** head, char* path, int sockfd, struct http_request* http, void (*controller)(int, struct http_request*)) {
    if (*head == NULL) {
        *head = (struct handlers*) malloc(sizeof(struct handlers));
        (*head)->controller = controller;
        (*head)->next = NULL;
        strcpy((*head)->path, path);

        return;
    }

    struct handlers* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (struct handlers*) malloc(sizeof(struct handlers));
    current->next->controller = controller;
    strcpy(current->next->path, path);
}

void handle_connection(int sockfd) {
    char request[255], *p, resource_path[255], key[255], header[255];
    struct http_request http;
    struct map headers;
    int request_length;

    init_map(&headers); 
    request_length = recv_line(sockfd, request);

    if (request_length <= 0) {
        puts("Error processing the request from browser\n");
        return;
    }

    parse_http_headers(sockfd, key, header, &headers);
    memmove(&http.headers, &headers, sizeof(struct map));

    printf("Cookies: %s\n", map_get(&http.headers, "Cookie"));

    p = strstr(request, " HTTP/");

    if (p == NULL) {
        puts("[ERROR] HTTP Request format is invalid!\n");
        return;
    }

    *p = 0;
    p = NULL;

    parse_request(request, &http);
    printf("Request type: %s\n", http.type_str);
    printf("Request URL: %s\n", http.url);
    printf("Request parameters: %s\n", http.parameters);
    
    strcpy(resource_path, WEBROOT);
    p = strstr(http.url, ".");

    if (p != NULL) {
        strcat(resource_path, http.url);
    } else {
        if (strlen(http.url) == 1) {
            strcat(resource_path, "/index.html");    
        } else {
            if (http.url[strlen(http.url) - 1] != '/') {
                strcat(resource_path, http.url);
                strcat(resource_path, "/index.html");
            } else {
                strcat(resource_path, http.url);
                strcat(resource_path, "index.html");
            }
        }
    }

    printf("Path to resource: %s\n", resource_path);

    switch (http.req_type) {
        case GET: {
            get_response(sockfd, resource_path, &http);
            break;
        }

        case POST: { 
            process_post(sockfd, &http);
            break;
        }
    default:
        break;
    }

    free_map(&http.headers);
}

void parse_request(unsigned char* request_line, struct http_request* http) {
    unsigned char* p = request_line;
    unsigned char* parameters_p;
    size_t index = 0;
    while (*p != ' ') {
        http->type_str[index++] = *p;
        p++;
    }

    p++;
    http->type_str[index] = 0;

    if (strcmp(http->type_str, "GET") == 0) http->req_type = GET;
    if (strcmp(http->type_str, "POST") == 0) http->req_type = POST;
    if (strcmp(http->type_str, "HEAD") == 0) http->req_type = HEAD;
    if (strcmp(http->type_str, "PUT") == 0) http->req_type = PUT;
    if (strcmp(http->type_str, "DELETE") == 0) http->req_type = DELETE;

    parameters_p = strstr(p, "?");
    if (parameters_p == NULL) {
        http->parameters[0] = 0;
        strcpy(http->url, p);
    } 
    else {
        printf("[INFO] Got parameters in request. Parsing...\n");
        size_t index = 0;
        while (p != parameters_p) {
            http->url[index++] = *p;
            p++;
        }

        http->url[index] = 0;
        strcpy(http->parameters, parameters_p);
    }
}

void get_response(int sockfd, unsigned char* resource, struct http_request* http) {

    int fd = open(resource, O_RDONLY);
    size_t file_size;
    unsigned char* buffer;

    if (fd == -1) {
        send_string(sockfd, "HTTP/1.0 404 Not Found\r\n\r\n");
        send_string(sockfd, "<html><head></head><body><center><h1>404 Not Found</h1></center></body></html>");
        return;
    }

    file_size = get_file_size(fd);
    buffer = (unsigned char*) malloc(file_size);
    if (buffer == NULL) {
        send_string(sockfd, "HTTP/1.0 500 File size is to big\r\n\r\n");
        send_string(sockfd, "<html><head></head><body><center><h1>500 Iternal server Error!</h1></center></body></html>");
        return;
    }

    if (read(fd, buffer, file_size) == -1) {
        send_string(sockfd, "HTTP/1.0 500 File size is to big\r\n\r\n");
        send_string(sockfd, "<html><head></head><body><center><h1>500 Iternal server Error!</h1></center></body></html>");
        return;
    }

    send_string(sockfd, "HTTP/1.0 200 OK\r\n\r\n");
    send(sockfd, buffer, file_size, 0);
    free(buffer);
}

void parse_http_headers(int sockfd, unsigned char* key, unsigned char* header, struct map* headers_struct) {
    while (recv_line(sockfd, header)) {
        unsigned char* ptr = strstr(header, ": ");
        unsigned char* h_ptr = header;
        size_t index = 0;
        if (ptr != NULL) {
            while (h_ptr != ptr) {
                key[index++] = *h_ptr;
                h_ptr++;
            }

            key[index] = 0;
            ptr += 2;
            strcpy(header, ptr);
            map_put(headers_struct, key, header);
        } 
    }
}

void users_handler(int sockfd, struct http_request* request) {
    send_string(sockfd, "HTTP/1.0 200 OK\r\n\r\n");
    send_string(sockfd, "{\'name\' : \'ivan\'}");
}

void process_post(int sockfd, struct http_request* http) {
    struct handlers* handlers = NULL;
    register_handler(&handlers, "/api/users", sockfd, http, users_handler);
    struct handlers* current = handlers;
    bool found = false;

    while (current != NULL) {
        if (strcmp(current->path, http->url) == 0) {
            current->controller(sockfd, http);
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found) {
        send_string(sockfd, "HTTP/1.0 404 Not Found\r\n\r\n");
        send_string(sockfd, "No handlers registered for this URL\n");
    }
}