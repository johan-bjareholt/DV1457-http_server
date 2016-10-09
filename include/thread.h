#pragma once

#include "connection_handler.h"

enum DISPATCH_METHOD {
    DISPATCH_METHOD_FORK,
    DISPATCH_METHOD_THREAD
};

void dispatch_connection(int sd_current, struct sockaddr_in pin);

void fork_connection_handler(struct handle_connection_params* params);
void thread_connection_handler(struct handle_connection_params* params);
