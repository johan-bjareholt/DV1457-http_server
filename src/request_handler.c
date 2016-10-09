#include "request_handler.h"

void free_http_request(struct http_request* target){
    free(target->path);
    free(target->version);
    free(target->properties);
    free(target);
}

struct http_request* parse_http_request(char* payload){
    int i, start, end;
    char* saveptr;
    
    // Get properties
    strtok_r(payload, "\r\n", &saveptr); // Skip first line to retreive properties
    char* properties_str_tmp = strtok_r(NULL, "\0", &saveptr);
    
    // Parse type
    char* type_str = strtok_r(payload, " ", &saveptr);

    // Parse path
    char* path_str_tmp = strtok_r(NULL, " ", &saveptr);
    
    // Parse version
    char* version_str_tmp = strtok_r(NULL, "\r\n", &saveptr);

    // Check type
    int http_type = HTTP_REQ_TYPE_UNKNOWN;
    if (strcmp(type_str, "HEAD") == 0){
        http_type = HTTP_REQ_TYPE_HEAD;
    }
    else if (strcmp(type_str, "GET") == 0){
        http_type = HTTP_REQ_TYPE_GET;
    }
    else {
        http_type = HTTP_REQ_TYPE_UNKNOWN;
    }

    // Debug Parsing
    //printf("Type: %s\n", type_str);
    //printf("Path: %s\n", path_str);
    //printf("Version: %s\n", version_str);
    //printf("Properties:\n%s\n", properties_str);

    // Copy strings to allocated memory for struct http_request
    char* properties_str = malloc((strlen(properties_str_tmp)+1)*sizeof(char));
    strcpy(properties_str, properties_str_tmp);
    char* path_str = malloc((strlen(path_str_tmp)+1)*sizeof(char));
    strcpy(path_str, path_str_tmp);
    char* version_str = malloc((strlen(version_str_tmp)+1)*sizeof(char));
    strcpy(version_str, version_str_tmp);

    // Check if path should redirect to index.html
    if (strcmp(path_str, "") == 0 || strcmp(path_str, "/") == 0){
        free(path_str);
        const char* index_path = "/index.html";
        path_str = malloc((strlen(index_path)+1)*sizeof(char));
        strcpy(path_str, index_path);
    }

    // Create struct
    struct http_request* req = malloc(sizeof(struct http_request));
    req->type = http_type;
    req->path = path_str;
    req->version = version_str;
    req->properties = properties_str;

    // TODO: Return null if unable to parse so the server can raise a 400-Bad Request
    return req;
}
