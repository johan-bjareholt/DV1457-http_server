#include "main.h"
#include "response_handler.h"

char* generate_http_response(struct http_request* request){
    char* response;
    if (request == NULL){
        const char* res_400 = "HTTP/1.0 400 Bad Request\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
            "\r\n"
            "400 Bad Request";

        response = malloc(strlen(res_400)*sizeof(char));
        strcpy(response, res_400);
    }
    else if (request->type == HTTP_TYPE_UNKNOWN){
        const char* res_501 = "HTTP/1.0 501 Not Implemented\n"
            "Date: Not Implemented\n"
            "Server: DV1457 http server\n"
            "Last-Modified: Not Implemented\n"
            "\r\n"
            "501 Not Implemented";
        response = malloc(strlen(res_501)*sizeof(char));
        strcpy(response, res_501);
    }
    else {
        char filepath[256];
        snprintf(filepath, 256*sizeof(char), "%s%s", wwwdir, request->path);
        FILE* fd = fopen(filepath, "r");
        if (fd == NULL){
            const char* res_404 = "HTTP/1.0 404 Not found\n"
                "Date: Not Implemented\n"
                "Server: DV1457 http server\n"
                "Last-Modified: Not Implemented\n"
                "\r\n"
                "<html><body><h1>404 Not Found</h1></body></html>";
            response = malloc(strlen(res_404)*sizeof(char));
            strcpy(response, res_404);
        }
        else {
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
            response = malloc(hsize+fsize+EXTRA);
            // 
            sprintf(response, res_200, fsize+(strlen(res_end)));
            // Append file content to buffer
            fread(response+strlen(response), fsize, sizeof(char), fd);
            // End request
            strcpy(response+strlen(response), res_end);
            
            // Close file
            fclose(fd);
        }
    }
    return response;
}

