#include "main.h"
#include "connection_handler.h"
#include "log.h"

#define BUFSIZE 2048

void handle_connection(int sd_current, struct sockaddr_in pin){
	char buf[BUFSIZE];
    if (recv(sd_current, buf, sizeof(buf), 0) == -1) {
        perror("recv");
        exit(-1);
        DIE("recv");
    }

    char ipAddress[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &pin.sin_addr, ipAddress, sizeof(ipAddress));
    
    printf("Request from %s:%i\n", ipAddress, ntohs(pin.sin_port));
    printf("Message: %s\n", buf);
    struct http_request* request = parse_http_request(buf);

    struct http_response* response = generate_http_response(request);
    
    log_request(ipAddress, request, response);

    if(send(sd_current, response->message, strlen(response->message), 0) == -1) {
        DIE("send");
    }
    printf("\nSent response: %s\n", response->message);

    // Cleanup
    free_http_request(request);
    free_http_response(response);
    close(sd_current);
}

