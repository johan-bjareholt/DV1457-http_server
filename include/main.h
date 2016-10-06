#pragma once

#include <stdbool.h>

#define DIE(str) printf(str);exit(-1);

extern int portnumber;
extern char* wwwdir;
extern bool running;
extern int dispatch_method;
