#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "request_handler.h"

char* generate_http_response(struct http_request* request);
