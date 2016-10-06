#include "main.h"
#include "response_handler.h"

void free_http_response(struct http_response* res){
    free(res->message);
    free(res);
}

struct http_response* generate_http_response(struct http_request* request){
    struct http_response* res = malloc(sizeof(struct http_response));
    res->type = HTTP_RES_TYPE_UNKNOWN;
    if (request == NULL){
        res->type = HTTP_RES_TYPE_400;
        const char* res_400 = "HTTP/1.0 400 Bad Request\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
            "\r\n"
            "<html><body>"
            "<h1>400 Bad Request</h1>"
            "</body></html>";

        res->message = malloc(strlen(res_400)*sizeof(char));
        strcpy(res->message, res_400);
    }
    else if (request->type == HTTP_REQ_TYPE_UNKNOWN){
        res->type = HTTP_RES_TYPE_501;
        const char* res_501 = "HTTP/1.0 501 Not Implemented\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
            "\r\n"
            "<html><body>"
            "<h1>501 Not Implemented</h1>"
            "</body></html>";
        res->message = malloc(strlen(res_501)*sizeof(char));
        strcpy(res->message, res_501);
    }
    else {
        char filepath[256];
        snprintf(filepath, 256*sizeof(char), "%s%s", wwwdir, request->path);
        printf("%s\n",filepath);
        FILE* fd = fopen(filepath, "r");
        if (fd == NULL){
            res->type = HTTP_RES_TYPE_404;
            const char* res_404 = "HTTP/1.0 404 Not found\n"
                "Date: Not Implemented\n"
                "Server: DV1457 http server\n"
                "Last-Modified: Not Implemented\n"
                "\r\n"
                "<html><body>"
                "<h1>404 Not Found</h1>"
                "</body></html>";
            res->message = malloc(strlen(res_404)*sizeof(char));
            strcpy(res->message, res_404);
        }
        else {
            res->type = HTTP_RES_TYPE_200;
            const char* res_200 = "HTTP/1.0 200 OK\n"
                "Date: Not Implemented\n"
                "Server: DV1457 http server\n"
                "Last-Modified: Not Implemented\n"
                "Content-Type: text/html\n"
                "Content-Length: %d\n"
                "\r\n";
            const char* res_end = "\r\n";
            
            // Find file size
            fseek(fd, 0, SEEK_END);
            long fsize = ftell(fd);
            fseek(fd, 0, SEEK_SET);  //same as rewind(f);
            // Find head size
            int hsize = strlen(res_200)*sizeof(char);
            // Malloc buffer
            const int EXTRA = 128*sizeof(char); // Extra is needed for dynamic fields such as date and content-length
            res->message = malloc(hsize+fsize+EXTRA);
            // 
            res->size = fsize+(strlen(res_end));
            sprintf(res->message, res_200, res->size);
            // Append file content to buffer
            fread(res->message+strlen(res->message), fsize, sizeof(char), fd);
            // End request
            strcpy(res->message+strlen(res->message), res_end);
            
            // Close file
            fclose(fd);
        }
    }
    return res;
}

