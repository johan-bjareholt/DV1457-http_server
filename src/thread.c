#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "thread.h"
#include "main.h"
#include "connection_handler.h"

void dispatch_connection(int sd_current, struct sockaddr_in pin){

    struct handle_connection_params* params = malloc(sizeof(struct handle_connection_params));
    params->sd_current = sd_current;
    params->pin = pin;

    switch(dispatch_method){
        case DISPATCH_METHOD_FORK:
            fork_connection_handler(params);
            break;
        case DISPATCH_METHOD_THREAD:
            thread_connection_handler(params);
            break;
        default:
            printf("Fatal: Unknown dispatch method!\n");
            exit(-1);
            break;
    }    
}

void fork_connection_handler(struct handle_connection_params* params){
    pid_t pid = fork();
    if (pid == -1){
        DIE("Unable to fork");
    }
    else if (pid == 0){
        // Child
        handle_connection(params);
        running = false;
    }
    else {
        // Parent
        close(params->sd_current);
        free(params);
    }
}

void thread_connection_handler(struct handle_connection_params* params){
    pthread_t child_thread;

    // create thread
    if(pthread_create(&child_thread, NULL, (void*)handle_connection, (void*)params)) {
        fprintf(stderr, "Error creating thread\n");
        exit(-1);
    }
    pthread_detach(child_thread);
}
