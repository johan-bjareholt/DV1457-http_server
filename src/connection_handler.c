#include "main.h"
#include "connection_handler.h"

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
    
    
    char* response = generate_http_response(request);

    if(send(sd_current, response, strlen(response), 0) == -1) {
        DIE("send");
    }
    printf("\nSent response: %s\n", response);

    // Cleanup
    free_http_request_struct(request);
    free(response);
    close(sd_current);
}

