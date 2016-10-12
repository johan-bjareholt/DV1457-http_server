#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
// INET
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "main.h"
#include "log.h"
#include "thread.h"
#include "config_parser.h"
#include "daemonize.h"
#include "connection_handler.h"

// Set externs
int portnumber = 8080;
char* wwwdir = NULL;
bool run_as_daemon = false;
bool running = true;
int dispatch_method = DISPATCH_METHOD_FORK;

pid_t parent_pid;

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

    
    const char* configpath = "./.lab3-config";
    parse_config(configpath);
    
    if (wwwdir == NULL){
        wwwdir = realpath("www", NULL);
        if (wwwdir == NULL){
            printf("No www folder in this directory!\n");
            exit(-1);    
        }
    }


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
                run_as_daemon = true;
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
    

    if (run_as_daemon)
        daemonize();

    printf("wwwdir: %s\n", wwwdir);
    printf("port: %d\n", portnumber);

    log_init();

    // Jail with chroot
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
    log_close();

	exit(0);
}

