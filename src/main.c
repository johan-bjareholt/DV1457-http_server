#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>

#include "main.h"
#include "config_parser.h"
#include "daemonize.h"
#include "connection_handler.h"

int portnumber = 8080;
char* wwwdir = "www/";

int main(int argc, char* argv[]) {
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

    parse_config();

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
                daemonize();
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

    printf("wwwdir: %s\n", wwwdir);
    printf("port: %d\n", portnumber);
    
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
            handle_connection(sd_current, pin);
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

