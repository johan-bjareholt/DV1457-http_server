#pragma once

#include <stdio.h>
#include <unistd.h>
// INET
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "request_handler.h"
#include "response_handler.h"

void handle_connection(int sd_current, struct sockaddr_in pin);
