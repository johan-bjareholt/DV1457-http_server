#pragma once

#include "request_handler.h"

enum HTTP_RESPONSE_TYPE {
    HTTP_RES_TYPE_200,
    HTTP_RES_TYPE_400,
    HTTP_RES_TYPE_404,
    HTTP_RES_TYPE_500,
    HTTP_RES_TYPE_501,

    HTTP_RES_TYPE_UNKNOWN,
};

struct http_response {
    int type;
    int size;
    char* message;
};

void free_http_response(struct http_response* res);

struct http_response* generate_http_response(struct http_request* request);
