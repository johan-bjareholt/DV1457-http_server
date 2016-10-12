#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include <time.h>
#include <sys/stat.h>

#include "main.h"
#include "response_handler.h"

void free_http_response(struct http_response* res){
    free(res->message);
    free(res);
}

struct http_response* generate_http_response(struct http_request* request){
    struct http_response* res = calloc(1, sizeof(struct http_response));
    res->type = HTTP_RES_TYPE_UNKNOWN;
    res->message = NULL;
	
    // Datetime
    time_t dt; // datetime
    struct tm * timeinfo; // datetime+timezone
	dt = time(NULL);
    timeinfo = localtime (&dt);
    char timestr [60];
    strftime (timestr,60,"%d/%h/%G:%T %z",timeinfo);
        
    const int EXTRA = 150*sizeof(char); // Extra is needed for dynamic fields such as date and content-length

    if (request == NULL){
        res->type = HTTP_RES_TYPE_400;
        const char* res_400 = "HTTP/1.0 400 Bad Request\n"
            "Server: DV1457 http server\n"
            "Date: %s\n"
            "\r\n"
            "<html><body>"
            "<h1>400 Bad Request</h1>"
            "</body></html>";
        int totsize = strlen(res_400)+EXTRA;
        res->message = calloc(1, totsize);
        snprintf(res->message, totsize, res_400, timestr);
    }
    else if (request->type == HTTP_REQ_TYPE_UNKNOWN){
        res->type = HTTP_RES_TYPE_501;
        const char* res_501 = "HTTP/1.0 501 Not Implemented\n"
            "Server: DV1457 http server\n"
            "Date: %s\n"
            "\r\n"
            "<html><body>"
            "<h1>501 Not Implemented</h1>"
            "</body></html>";
        int totsize = strlen(res_501)+EXTRA;
        res->message = calloc(1, totsize);
        snprintf(res->message, totsize, res_501, timestr);
    }
    else {
        char filepath[256];
        snprintf(filepath, 256*sizeof(char), "%s%s", wwwdir, request->path);
        //printf("%s\n",filepath);
        FILE* fd = fopen(filepath, "r");
        if (fd == NULL){
            res->type = HTTP_RES_TYPE_404;
            const char* res_404 = "HTTP/1.0 404 Not found\n"
                "Server: DV1457 http server\n"
                "Date: %s\n"
                "\r\n"
                "<html><body>"
                "<h1>404 Not Found</h1>"
                "</body></html>";
            int totsize = strlen(res_404)+EXTRA;
            res->message = calloc(1, totsize);
            snprintf(res->message, totsize, res_404, timestr);
        }
        else {
            res->type = HTTP_RES_TYPE_200;
            const char* res_200 = "HTTP/1.0 200 OK\n"
                "Server: DV1457 http server\n"
                "Date: %s\n"
                "Last-Modified: %s\n"
                "Content-Type: text/html\n"
                "Content-Length: %d\n"
                "\r\n";
            const char* res_end = "\r\n";
            int esize = strlen(res_end)*sizeof(char);

            // Get file modified date
            struct stat attr;
            stat(filepath, &attr);
            struct tm * modtimeinfo; // datetime+timezone
            modtimeinfo = localtime (&attr.st_mtime);
            char modtimestr [60];
            strftime (modtimestr, 60, "%d/%h/%G:%T %z", modtimeinfo);
            
            // Find file size
            fseek(fd, 0, SEEK_END);
            long fsize = ftell(fd);
            fseek(fd, 0, SEEK_SET);  //same as rewind(f);
            // Find head size
            int hsize = strlen(res_200)*sizeof(char);
            // Malloc buffer
            int totsize = hsize+fsize+esize+EXTRA;
            res->message = calloc(1, totsize);
            // Insert string
            res->size = fsize+(strlen(res_end));
            snprintf(res->message, totsize, res_200, timestr, modtimestr, res->size);
    

            if (request->type == HTTP_REQ_TYPE_GET){ 
                // Append file content to buffer
                fread(res->message+strlen(res->message), fsize, sizeof(char), fd);
                // End request
                strncpy(res->message+strlen(res->message), res_end, strlen(res_end));
            }
            
            // Close file
            fclose(fd);
        }
    }
    return res;
}

