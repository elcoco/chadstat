#ifndef UTILS_H
#define UTILS_H

//#include "block.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <curl/curl.h>



#define DO_DEBUG 0
#define DO_INFO  0
#define DO_ERROR 0

#define DEBUG(M, ...) if(DO_DEBUG){fprintf(stdout, "[DEBUG] " M, ##__VA_ARGS__);}
#define INFO(M, ...) if(DO_INFO){fprintf(stdout, M, ##__VA_ARGS__);}
#define ERROR(M, ...) if(DO_ERROR){fprintf(stderr, "[ERROR] (%s:%d) " M, __FILE__, __LINE__, ##__VA_ARGS__);}

#define I3_FMT "{\"name\": \"%s\", \"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0},\n"

struct Block;


int8_t do_request(const char* url, long* response_code);
void i3ify(char *buf, char *text, char *color);
int strcat_alloc(char **dest, int old_size, char *buf);
int i3ify_alloc(struct Block *block, char *text, char *color);

#endif
