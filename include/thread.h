#pragma once

#include <netdb.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

enum DISPATCH_METHOD {
    DISPATCH_METHOD_FORK,
    DISPATCH_METHOD_THREAD
};

void dispatch_connection(int sd_current, struct sockaddr_in pin);

void fork_connection_handler(int sd_current, struct sockaddr_in pin);
void thread_connection_handler(int sd_current, struct sockaddr_in pin);
