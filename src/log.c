#include "log.h"

#include <syslog.h>
#include <time.h>

void log_request(const char* ip, struct http_request* request){
	time_t ctime; // calendar time
    struct tm * timeinfo; // time+timezone
    
	ctime = time(NULL);
    timeinfo = localtime (&ctime);
    
    char timestr [80];
    strftime (timestr,80,"%d/%h/%G:%T %z",timeinfo);

    syslog(LOG_NOTICE, "%s - - [%s] %s",ip, timestr, request->path);
}
