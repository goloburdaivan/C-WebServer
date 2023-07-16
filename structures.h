#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "data_structures.h"

enum request_type {
    GET,
    POST,
    HEAD,
    PUT,
    DELETE
};

struct http_request {
    char type_str[8];
    char url[255];
    enum request_type req_type;
    char parameters[255];
    struct map headers;
};

#endif