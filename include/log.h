#pragma once

#include "request_handler.h"
#include "response_handler.h"

enum LOG_METHOD {
    LOG_METHOD_SYSLOG,
    LOG_METHOD_LOGFILE
};

extern char* logfilepath;

extern int log_method;

void log_request(const char* ip, struct http_request* request, struct http_response* response);
