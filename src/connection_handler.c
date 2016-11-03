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
	char* payload = calloc(BUFSIZE, 1);
    char* ipaddr_str = calloc(INET_ADDRSTRLEN, 1);

    if (recv(params->sd_current, payload, BUFSIZE, 0) == -1) {
        perror("recv");
        exit(-1);
    }

    inet_ntop(AF_INET, &params->pin.sin_addr, ipaddr_str, INET_ADDRSTRLEN);

    //printf("Request from %s:%i\n", ipaddr_str, ntohs(params->pin.sin_port));

    struct http_request* request = parse_http_request(payload);

    struct http_response* response = generate_http_response(request);

    log_request(ipaddr_str, request, response);

    if(send(params->sd_current, response->message, strlen(response->message), 0) == -1) {
        printf("Failed to send reponse\n");
    }

    // Cleanup
    if (request != NULL)
        free_http_request(request);
    if (response != NULL)
        free_http_response(response);
    close(params->sd_current);
    free(ipaddr_str);
    free(payload);
    free(params);
}
