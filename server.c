#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <arpa/inet.h>

#define DIE(str) perror(str);exit(-1);
#define BUFSIZE 512

enum HTTP_REQUEST_TYPE {
    HTTP_TYPE_HEAD,
    HTTP_TYPE_GET,

    HTTP_TYPE_UNKNOWN,
};

struct http_request {
    int type;
    char* path;
    char* version;
    char* properties;
};

void free_http_request_struct(struct http_request* target){
    free(target->path);
    free(target->version);
    free(target->properties);
}

struct http_request* parse_http_request(const char* payload){
    int i, start, end;
    
    // Copy first line to first_line
    char* first_line;
    for (i=0; i<strlen(payload) && payload[i] != '\n'; i++){}
    first_line = malloc(i*sizeof(char));
    strncpy(first_line, payload, i*sizeof(char));
    first_line[i] = '\0';
    i++;
    
    // Copy properties
    int properties_len = strlen(payload)-i+1;
    char* properties_str = malloc(properties_len*sizeof(char));
    strcpy(properties_str, payload+i);
   
    // Parse type
    start = 0;
    for (i=0; i<strlen(first_line) && first_line[i] != ' '; i++){}
    end = i;
    char* type_str = malloc((end-start+1)*sizeof(char));
    strncpy(type_str, first_line+start, end-start);
    type_str[end-start] = '\0';
    i++;

    // Parse path
    start = i;
    for (i=i; i<strlen(first_line) && first_line[i] != ' '; i++){}
    end = i;
    char* path_str = malloc((end-start+1)*sizeof(char));
    strncpy(path_str, first_line+start, end-start);
    path_str[end-start] = '\0';
    i++;

    // Parse version
    start = i;
    for (i=i; i<strlen(first_line) && first_line[i] != '\n'; i++){}
    end = i;
    char* version_str = malloc((end-start+1)*sizeof(char));
    strncpy(version_str, first_line+start, end-start);
    version_str[end-start] = '\0';
    i++;

    // Debug Parsing
    //printf("First line: %s\n", first_line);
    printf("Type: %s\n", type_str);
    printf("Path: %s\n", path_str);
    printf("Version: %s\n", version_str);
    //printf("Properties:\n%s\n", properties_str);
    //printf("Whole msg:\n%s\n", payload);

    // Check type
    int http_type = HTTP_TYPE_UNKNOWN;
    if (strcmp(type_str, "HEAD") == 0){
        http_type = HTTP_TYPE_HEAD;
    }
    else if (strcmp(type_str, "GET") == 0){
        http_type = HTTP_TYPE_GET;
    }

    // Clean
    free(type_str);
    free(first_line);

    // Create struct
    struct http_request* req = malloc(sizeof(struct http_request));
    req->type = http_type;
    req->path = path_str;
    req->version = version_str;
    req->properties = properties_str;

    return req;
}

void handle_request(int sd_current, struct sockaddr_in pin){
	char buf[BUFSIZE];
    if (recv(sd_current, buf, sizeof(buf), 0) == -1) {
        DIE("recv");
    }

    char ipAddress[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &pin.sin_addr, ipAddress, sizeof(ipAddress));
    
    printf("Request from %s:%i\n", ipAddress, ntohs(pin.sin_port));
    printf("Message: %s\n", buf);
    parse_http_request(buf);
    
    const char* response = "server response";
    if(send(sd_current, response, strlen(response), 0) == -1) {
        DIE("send");
    }
    printf("\nSent response: %s\n", response);
    
    close(sd_current);
}

int main(int argc, char* argv[]) {
	int portnumber;
	struct sockaddr_in sin, pin;
	int sd, sd_current;
	int addrlen;
    
    // Handle children so they don't become zombies
    struct sigaction sigchld_action = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_NOCLDWAIT
    };
    sigaction(SIGCHLD, &sigchld_action, NULL);

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(-1);
	}

	portnumber = atoi(argv[1]);

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		DIE("socket");
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(portnumber);

	if(bind(sd, (struct sockaddr*) &sin, sizeof(sin)) == -1) {
		DIE("bind");
	}

    if(listen(sd, 10) == -1) {
        DIE("listen");
    }

    bool running = true;
    pid_t pid;
    while (running){
        printf("Waiting for connection...\n");
        
        addrlen = sizeof(pin);
        if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
            DIE("accept");
        }
        printf("Accepted connection!\n");
        
        pid_t pid = fork();
        if (pid == -1){
            DIE("Unable to fork");
        }
        else if (pid == 0){
            // Child
            handle_request(sd_current, pin);
            running = false;
        }
        else {
            // Parent
            close(sd_current);
        }
    }
    // Close sockets
    if (pid != 0)
        close(sd);

	exit(0);
}

