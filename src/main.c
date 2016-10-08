#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <pwd.h>

#include "main.h"
#include "log.h"
#include "thread.h"
#include "config_parser.h"
#include "daemonize.h"
#include "connection_handler.h"

// Set externs
int portnumber = 8080;
char* wwwdir;
bool running = true;
int dispatch_method = DISPATCH_METHOD_FORK;

pid_t parent_pid;

int main(int argc, char* argv[]) {
	struct sockaddr_in sin, pin;
	int sd, sd_current;
	int addrlen;

    wwwdir = malloc(50*sizeof(char));
    strcpy(wwwdir, "/home/johan/Programming/C/DV1457-http_server/www");

    const char* helpmsg =
        "Simple HTTP 1.0 server\n"
        "A DV1457 assignment\n"
        "Options:\n"
        "\t-h Show this help message\n"
        "\t-p port (default: 8080)\n"
        "\t-d Run as daemon\n"
        "\t-l logfile\n"
        "\t-s [fork|thread|prefork|mux]\n";


    const char* configpath = "./.lab3-config";
    parse_config(configpath);

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
                if (argi+1 >= argc){
                    printf("No logfile specified!\n");
                    printf(helpmsg);
                    exit(-1);
                }
                log_method = LOG_METHOD_LOGFILE;
                argi++;
                logfilepath = argv[argi];
                break;
            case 's':
                if (argi+1 >= argc){
                    printf("No multiprocessing method specified!\n");
                    printf(helpmsg);
                    exit(-1);
                }
                argi++;
                if (strcmp(argv[argi], "fork") == 0){
                    dispatch_method = DISPATCH_METHOD_FORK;
                     
                }
                else if (strcmp(argv[argi], "thread") == 0){
                    dispatch_method = DISPATCH_METHOD_THREAD;
                }
                else {
                    printf("Invalid method\n");
                    exit(-1);
                }
                break;
            default:
                printf(helpmsg);
                printf("Unknown parameter\n");
                exit(-1);
                break;
        }
    }
    
    printf("wwwdir: %s\n", wwwdir);
    printf("port: %d\n", portnumber);

    // chroot

    chdir(wwwdir);
    if (chroot(wwwdir) != 0) {
        perror("Unable to chroot");
        return 1;
    }
    free(wwwdir);
    wwwdir = malloc(sizeof(char));
    strcpy(wwwdir, "");
    
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

    parent_pid = getpid();
    
    printf("Waiting for connections...\n");
    while (running){
        
        addrlen = sizeof(pin);
        if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
            DIE("accept");
        }
        //printf("Accepted connection!\n");
        dispatch_connection(sd_current, pin);
    }
    if (getpid() == parent_pid)
        close(sd);

    free(wwwdir);

	exit(0);
}

