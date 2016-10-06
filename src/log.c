#include "log.h"

#include <syslog.h>
#include <time.h>

char* logfilepath = "";

int log_method = LOG_METHOD_SYSLOG;


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
            syslog(LOG_NOTICE, "%s - - [%s] \"%s %s %s\" %s %d",
                    ip, timestr,
                    methodstr, request->path, request->version,
                    typestr, response->size);
            break;
        case LOG_METHOD_LOGFILE:
            {
                FILE* fd = fopen(logfilepath, "a");
                if (fd == NULL){
                    printf("Invalid log file!\n");
                    exit(-1);
                }
                fprintf(fd, "%s - - [%s] \"%s %s %s\" %s %d\n",
                    ip, timestr,
                    methodstr, request->path, request->version,
                    typestr, response->size);
                fclose(fd);
            }
            break;
        default:
            printf("Invalid log method\n");
            exit(-1);
            break;
    }
}
