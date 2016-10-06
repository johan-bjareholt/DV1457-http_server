#pragma once

#include "request_handler.h"
#include "response_handler.h"

void log_request(const char* ip, struct http_request* request, struct http_response* response);
