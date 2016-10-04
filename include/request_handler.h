#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum HTTP_REQUEST_TYPE {
    HTTP_TYPE_HEAD,
    HTTP_TYPE_GET,

    HTTP_TYPE_UNKNOWN,
};

struct http_request {
    int type;
    char* path;
    char* version;
    char* properties;
};


void free_http_request_struct(struct http_request* target);

struct http_request* parse_http_request(const char* payload);
