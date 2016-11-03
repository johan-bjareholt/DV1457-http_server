#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// INET
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "main.h"
#include "connection_handler.h"
#include "log.h"

#define BUFSIZE 8192

void handle_connection(struct handle_connection_params* params){
	char* buf = malloc(BUFSIZE);
    if (recv(params->sd_current, buf, BUFSIZE, 0) == -1) {
        perror("recv");
        exit(-1);
        DIE("recv");
    }

    char ipAddress[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &params->pin.sin_addr, ipAddress, sizeof(ipAddress));

    //printf("Request from %s:%i\n", ipAddress, ntohs(params->pin.sin_port));
    //printf("Received request: %s\n", buf);

    struct http_request* request = parse_http_request(buf);

    struct http_response* response = generate_http_response(request);

    if (request != NULL && response != NULL)
        log_request(ipAddress, request, response);

    if(send(params->sd_current, response->message, strlen(response->message), 0) == -1) {
        DIE("send");
    }
    //printf("\nSent response: %s\n", response->message);

    // Cleanup
    if (request != NULL)
        free_http_request(request);
    if (response != NULL)
        free_http_response(response);
    close(params->sd_current);
    free(buf);
    free(params);
}
