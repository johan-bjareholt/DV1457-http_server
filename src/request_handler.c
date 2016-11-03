#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    char* type_str_tmp = strtok_r(payload, " ", &saveptr);

    // Parse path
    char* path_str_tmp = strtok_r(NULL, " ", &saveptr);

    // Parse version
    char* version_str_tmp = strtok_r(NULL, "\r\n", &saveptr);

    // Check type
    int http_type = HTTP_REQ_TYPE_UNKNOWN;
    if (strcmp(type_str_tmp, "HEAD") == 0){
        http_type = HTTP_REQ_TYPE_HEAD;
    }
    else if (strcmp(type_str_tmp, "GET") == 0){
        http_type = HTTP_REQ_TYPE_GET;
    }
    else {
        // Unknown request type
        //http_type = HTTP_REQ_TYPE_UNKNOWN;
        printf("Bad request parsed\n");
        return NULL;
    }

    // Copy strings to allocated memory for struct http_request
    size_t properties_str_size = (strlen(properties_str_tmp)+1)*sizeof(char);
    char* properties_str = malloc(properties_str_size);
    strncpy(properties_str, properties_str_tmp, properties_str_size);

    // Debug Parsing
    //printf("Type: %s\n", type_str_tmp);
    //printf("Path: %s\n", path_str_tmp);
    //printf("Version: %s\n", version_str_tmp);
    //printf("Properties:\n%s\n", properties_str);

    size_t path_str_size = (strlen(path_str_tmp)+1)*sizeof(char);
    char* path_str = malloc(properties_str_size);
    strncpy(path_str, path_str_tmp, path_str_size);

    size_t version_str_size = (strlen(version_str_tmp)+1)*sizeof(char);
    char* version_str = malloc(version_str_size);
    strncpy(version_str, version_str_tmp, version_str_size);

    // Check if path should redirect to index.html
    if (strcmp(path_str, "") == 0 || strcmp(path_str, "/") == 0){
        free(path_str);
        const char* index_path = "/index.html";
        path_str_size = (strlen(index_path)+1)*sizeof(char);
        path_str = malloc(path_str_size);
        strncpy(path_str, index_path, path_str_size);
    }

    // Create struct
    struct http_request* req = malloc(sizeof(struct http_request));
    req->type = http_type;
    req->path = path_str;
    req->version = version_str;
    req->properties = properties_str;

    return req;
}
