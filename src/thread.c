#include <pthread.h>

#include "thread.h"
#include "main.h"
#include "connection_handler.h"

void dispatch_connection(int sd_current, struct sockaddr_in pin){
    switch(dispatch_method){
        case DISPATCH_METHOD_FORK:
            fork_connection_handler(sd_current, pin);
            break;
        case DISPATCH_METHOD_THREAD:
            thread_connection_handler(sd_current, pin);
            break;
        default:
            printf("Fatal: Unknown dispatch method!\n");
            exit(-1);
            break;
    }    
}

void fork_connection_handler(int sd_current, struct sockaddr_in pin){
    pid_t pid = fork();
    if (pid == -1){
        DIE("Unable to fork");
    }
    else if (pid == 0){
        // Child
        struct handle_connection_params* params = malloc(sizeof(struct handle_connection_params));
        params->sd_current = sd_current;
        params->pin = pin;
        handle_connection(params);
        running = false;
    }
    else {
        // Parent
        close(sd_current);
    }
}

void thread_connection_handler(int sd_current, struct sockaddr_in pin){
    pthread_t child_thread;

    // Create argument for thread
    struct handle_connection_params* params = malloc(sizeof(struct handle_connection_params));
    params->sd_current = sd_current;
    params->pin = pin;

    // create thread
    if(pthread_create(&child_thread, NULL, (void*)handle_connection, (void*)params)) {
        fprintf(stderr, "Error creating thread\n");
        exit(-1);
    }
    pthread_detach(child_thread);
}
