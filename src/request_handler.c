#include "request_handler.h"

void free_http_request_struct(struct http_request* target){
    free(target->path);
    free(target->version);
    free(target->properties);
}

struct http_request* parse_http_request(const char* payload){
    int i, start, end;
    
    // Copy first line to first_line
    char* first_line;
    for (i=0; i<strlen(payload) && payload[i] != '\n'; i++){}
    first_line = malloc(i*sizeof(char));
    strncpy(first_line, payload, i*sizeof(char));
    first_line[i] = '\0';
    i++;
    
    // Copy properties
    int properties_len = strlen(payload)-i+1;
    char* properties_str = malloc(properties_len*sizeof(char));
    strcpy(properties_str, payload+i);
   
    // Parse type
    start = 0;
    for (i=0; i<strlen(first_line) && first_line[i] != ' '; i++){}
    end = i;
    char* type_str = malloc((end-start+1)*sizeof(char));
    strncpy(type_str, first_line+start, end-start);
    type_str[end-start] = '\0';
    i++;

    // Parse path
    start = i;
    for (i=i; i<strlen(first_line) && first_line[i] != ' '; i++){}
    end = i;
    char* path_str = malloc((end-start+1)*sizeof(char));
    strncpy(path_str, first_line+start, end-start);
    path_str[end-start] = '\0';
    if (strlen(path_str) > 20){
        free(first_line);
        free(type_str);
        free(path_str);
        return NULL;
    }
    i++;

    // Parse version
    start = i;
    for (i=i; i<strlen(first_line) && first_line[i] != '\n'; i++){}
    end = i;
    char* version_str = malloc((end-start+1)*sizeof(char));
    strncpy(version_str, first_line+start, end-start);
    version_str[end-start] = '\0';
    i++;

    // Check type
    int http_type = HTTP_TYPE_UNKNOWN;
    if (strcmp(type_str, "HEAD") == 0){
        http_type = HTTP_TYPE_HEAD;
    }
    else if (strcmp(type_str, "GET") == 0){
        http_type = HTTP_TYPE_GET;
    }
    else {
        http_type = HTTP_TYPE_UNKNOWN;
    }

    //
    if (strcmp(path_str,"") == 0 || strcmp(path_str,"/") == 0){
        free(path_str);
        const char* index_path = "/index.html";
        path_str = malloc(strlen(index_path)*sizeof(char));
        strcpy(path_str, index_path);
    }

    // Debug Parsing
    //printf("First line: %s\n", first_line);
    printf("Type: %s\n", type_str);
    printf("Path: %s\n", path_str);
    printf("Version: %s\n", version_str);
    //printf("Properties:\n%s\n", properties_str);
    //printf("Whole msg:\n%s\n", payload);

    // Clean
    free(type_str);
    free(first_line);

    // Create struct
    struct http_request* req = malloc(sizeof(struct http_request));
    req->type = http_type;
    req->path = path_str;
    req->version = version_str;
    req->properties = properties_str;

    // TODO: Return null if unable to parse so the server can raise a 400-Bad Request
    return req;
}
