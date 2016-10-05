#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "config_parser.h"
#include "main.h"



void parse_config(){

    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    const char* filename = "/.lab3-config";
    char* filepath = malloc((strlen(homedir)+strlen(filename)+1)*sizeof(char));
    strcpy(filepath, homedir);
    strcpy(filepath+strlen(homedir), filename);
    
    FILE* fd = fopen(filepath, "r");
    if (fd == NULL){
        printf("No config file available!\n");
        return;
    }
    // Read and parse config file
    char line[128];
    while (true){
        if (fgets(line, 128*sizeof(char), fd) == NULL)
            break;
        char* key = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        //printf("%s\n",key);
        //printf("%s\n",value);

        if (strcmp(key, "port") == 0){
            portnumber = atoi(value);
        }
        else if (strcmp(key, "wwwdir") == 0){
            // Fix memory leak
            wwwdir = malloc((strlen(value)+1)*sizeof(char));
            strcpy(wwwdir, value);
        }
        else {
            printf("Invalid key in configuration: %s\n", key);
        }
    }
    // Close file
    fclose(fd);
}
