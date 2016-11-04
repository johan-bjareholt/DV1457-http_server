#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <pthread.h>

#include "log.h"

char* logfilepath = "";
FILE* logfd;

int log_method = LOG_METHOD_SYSLOG;

static pthread_mutex_t log_lock;

void log_init(){
    pthread_mutex_init(&log_lock, NULL);
    switch (log_method){
        case LOG_METHOD_SYSLOG:
            openlog("httpd", LOG_PID | LOG_NDELAY, LOG_DAEMON);
            break;
        case LOG_METHOD_LOGFILE:
            logfd = fopen(logfilepath, "w");
            if (logfd == NULL){
                printf("Invalid log file!\n");
                exit(-1);
            }
            break;
        default:
            printf("Invalid log method\n");
            exit(-1);
            break;
    }
}

void log_close(){
    switch (log_method){
        case LOG_METHOD_SYSLOG:
            closelog();
            break;
        case LOG_METHOD_LOGFILE:
            fclose(logfd);
            break;
        default:
            printf("Invalid log method\n");
            exit(-1);
            break;
    }
}

void log_request(const char* ipstr, struct http_request* request, struct http_response* response){

    // Lock mutex to avoid simultaneous writes
    pthread_mutex_lock(&log_lock);

    // IP is argument
    
    // Time
	time_t ctime; // calendar time
    struct tm * timeinfo; // time+timezone
    
	ctime = time(NULL);
    timeinfo = localtime (&ctime);
    
    char timestr [80];
    strftime (timestr,80,"%d/%h/%G:%T %z",timeinfo);

    // Method
    char* methodstr = "Invalid";
    if (request != NULL){
        switch(request->type){
            case HTTP_REQ_TYPE_HEAD:
                methodstr = "HEAD";
                break;
            case HTTP_REQ_TYPE_GET:
                methodstr = "GET";
                break;
        }
    }

    // Path
    char* pathstr = "None";
    if (request != NULL)
        pathstr = request->path;
    
    // Version
    char* versionstr = "Unknown";
    if (request != NULL)
        versionstr = request->version;
    
    // Type
    char* typestr = "Invalid";
    if (response != NULL){
        switch(response->type){
            case HTTP_RES_TYPE_200:
                typestr = "200"; break;
            case HTTP_RES_TYPE_400:
                typestr = "400"; break;
            case HTTP_RES_TYPE_404:
                typestr = "404"; break;
            case HTTP_RES_TYPE_500:
                typestr = "500"; break;
            case HTTP_RES_TYPE_501:
                typestr = "501"; break;
            case HTTP_RES_TYPE_UNKNOWN:
                typestr = "Invalid"; break;
        }
    }
    
    // Size
    int sizeint = -1;
    if (response != NULL)
        sizeint = response->size;

    switch (log_method){
        case LOG_METHOD_SYSLOG:
            syslog(LOG_INFO, "%s - - [%s] \"%s %s %s\" %s %d",
                    ipstr, timestr,
                    methodstr, pathstr, versionstr,
                    typestr, sizeint);
            break;
        case LOG_METHOD_LOGFILE:
            {
                fprintf(logfd, "%s - - [%s] \"%s %s %s\" %s %d\n",
                    ipstr, timestr,
                    methodstr, pathstr, versionstr,
                    typestr, sizeint);
                fflush(logfd);
            }
            break;
        default:
            printf("Invalid log method\n");
            exit(-1);
            break;
    }
    
    // Unlock mutex for next write
    pthread_mutex_unlock(&log_lock);
}
