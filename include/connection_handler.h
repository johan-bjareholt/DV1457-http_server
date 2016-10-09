#pragma once

#include <arpa/inet.h>

#include "request_handler.h"
#include "response_handler.h"

struct handle_connection_params {
    int sd_current;
    struct sockaddr_in pin;
};

void handle_connection(struct handle_connection_params* params);
