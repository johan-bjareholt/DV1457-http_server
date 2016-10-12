#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "config_parser.h"
#include "main.h"



void parse_config(const char* filepath){
    
    FILE* fd = fopen(filepath, "r");
    if (fd == NULL){
        printf("No config file available in this directory!\n");
        return;
    }
    // Read and parse config file
    char line[128];
    while (true){
        if (fgets(line, 128*sizeof(char), fd) == NULL)
            break;
        char* saveptr;
        char* key = strtok_r(line, "=", &saveptr);
        char* value = strtok_r(NULL, "\n", &saveptr);
        //printf("%s\n",key);
        //printf("%s\n",value);

        if (strcmp(key, "port") == 0){
            portnumber = atoi(value);
        }
        else if (strcmp(key, "wwwdir") == 0){
            free(wwwdir);
            size_t wwwdir_size = (strlen(value)+1)*sizeof(char);
            wwwdir = malloc(wwwdir_size);
            strncpy(wwwdir, value, wwwdir_size);
        }
        else {
            printf("Invalid key in configuration: %s\n", key);
        }
    }
    // Close file
    fclose(fd);
}
