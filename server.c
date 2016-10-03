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
#include <fcntl.h>
#include <sys/errno.h>
#include <arpa/inet.h>

#define DIE(str) printf(str);exit(-1);
#define BUFSIZE 2048

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

    // Check type
    int http_type = HTTP_TYPE_UNKNOWN;
    if (strcmp(type_str, "HEAD") == 0){
        http_type = HTTP_TYPE_HEAD;
    }
    else if (strcmp(type_str, "GET") == 0){
        http_type = HTTP_TYPE_GET;
    }
    else {
        http_type = HTTP_TYPE_UNKNOWN;
    }

    //
    if (strcmp(path_str,"") == 0 || strcmp(path_str,"/") == 0){
        free(path_str);
        const char* index_path = "index.html";
        path_str = malloc(strlen(index_path)*sizeof(char));
        strcpy(path_str, index_path);
    }

    // Debug Parsing
    //printf("First line: %s\n", first_line);
    printf("Type: %s\n", type_str);
    printf("Path: %s\n", path_str);
    printf("Version: %s\n", version_str);
    //printf("Properties:\n%s\n", properties_str);
    //printf("Whole msg:\n%s\n", payload);

    // Clean
    free(type_str);
    free(first_line);

    // Create struct
    struct http_request* req = malloc(sizeof(struct http_request));
    req->type = http_type;
    req->path = path_str;
    req->version = version_str;
    req->properties = properties_str;

    // TODO: Return null if unable to parse so the server can raise a 400-Bad Request
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
    struct http_request* request = parse_http_request(buf);
    
    
    char response[BUFSIZE];
    if (request == NULL){
        strcpy(response, "HTTP/1.0 400 Bad Request\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
        );
    }
    else if (request->type == HTTP_TYPE_UNKNOWN){
        strcpy(response, "HTTP/1.0 501 Not Implemented\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
        );
    }
    else {
        const char* basedir = "www/";
        char filepath[BUFSIZE];
        snprintf(filepath, BUFSIZE*sizeof(char), "%s%s", basedir, request->path);
        FILE* fd = fopen(filepath, "r");
        if (fd == NULL){
            strcpy(response, "HTTP/1.0 404 Not found\n"
                "Date: Not Implemented\n"
                "Server: DV1457 http server\n"
                "Last-Modified: Not Implemented\n"
                "\r\n"
                "404 Not Found"
            );
        }
        else {
            // Find file size
            fseek(fd, 0, SEEK_END);
            long fsize = ftell(fd);
            fseek(fd, 0, SEEK_SET);  //same as rewind(f);
            // Malloc buffer
            char *response_body = malloc(fsize + 1);
            // Copy file to buffer
            fread(response_body, fsize, 1, fd);
            // Close file
            fclose(fd);


            snprintf(
                response,
                sizeof(response),
                "HTTP/1.0 200 OK\n"
                "Date: Not Implemented\n"
                "Server: DV1457 http server\n"
                "Last-Modified: Not Implemented\n"
                "Content-Type: text/html\n"
                "Content-Length: %d\n"
                "\r\n"
                "%s\n",
                strlen(response_body)*sizeof(char),
                response_body
            );
            free(response_body);
        }
    }
    if(send(sd_current, response, strlen(response), 0) == -1) {
        DIE("send");
    }
    printf("\nSent response: %s\n", response);

    // Cleanup
    free_http_request_struct(request);
    close(sd_current);
}

int main(int argc, char* argv[]) {
	int portnumber = 8080;
	struct sockaddr_in sin, pin;
	int sd, sd_current;
	int addrlen;

    const char* helpmsg =
        "Simple HTTP 1.0 server\n"
        "A DV1457 assignment\n"
        "Options:\n"
        "\t-h Show this help message\n"
        "\t-p port (default: 8080)\n"
        "\t-d Run as daemon\n"
        "\t-l logfile\n"
        "\t-s [fork|thread|prefork|mux]\n";

    for (int argi=1; argi<argc; argi++){
        if (strlen(argv[argi]) < 1){
            printf("Unknown parameter\n");
            printf(helpmsg);
        }
        switch(argv[argi][1]){
            case 'h':
                printf(helpmsg);
                exit(0);
                break;
            case 'p':
                if (argi+1 >= argc){
                    printf("No port specified!\n");
                    printf(helpmsg);
                    exit(-1);
                }
                argi++;
                portnumber = atoi(argv[argi]);
                break;
            case 'd':
                printf("Daemons not yet implemented!\n");
                exit(-1);
                break;
            case 'l':
                printf("Log output not yet implemented!\n");
                exit(-1);
                break;
            case 's':
                printf("Different fork techniques not yet implemented!\n");
                exit(-1);
                break;
            default:
                printf(helpmsg);
                printf("Unknown parameter\n");
                exit(-1);
                break;
        }
    }
    
    // Handle children so they don't become zombies
    struct sigaction sigchld_action = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_NOCLDWAIT
    };
    sigaction(SIGCHLD, &sigchld_action, NULL);

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

