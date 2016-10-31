#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>

#include "log.h"

char* logfilepath = "";
FILE* logfd;

int log_method = LOG_METHOD_SYSLOG;


void log_init(){
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

void log_request(const char* ip, struct http_request* request, struct http_response* response){
	time_t ctime; // calendar time
    struct tm * timeinfo; // time+timezone
    
	ctime = time(NULL);
    timeinfo = localtime (&ctime);
    
    char timestr [80];
    strftime (timestr,80,"%d/%h/%G:%T %z",timeinfo);

    char* methodstr;

    switch(request->type){
        case HTTP_REQ_TYPE_HEAD:
            methodstr = "HEAD";
            break;
        case HTTP_REQ_TYPE_GET:
            methodstr = "GET";
            break;
        default:
            methodstr = "Invalid";
            break;
    }

    char* typestr;
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

    switch (log_method){
        case LOG_METHOD_SYSLOG:
            syslog(LOG_INFO, "%s - - [%s] \"%s %s %s\" %s %d",
                    ip, timestr,
                    methodstr, request->path, request->version,
                    typestr, response->size);
            break;
        case LOG_METHOD_LOGFILE:
            {
                fprintf(logfd, "%s - - [%s] \"%s %s %s\" %s %d\n",
                    ip, timestr,
                    methodstr, request->path, request->version,
                    typestr, response->size);
            }
            break;
        default:
            printf("Invalid log method\n");
            exit(-1);
            break;
    }
}
