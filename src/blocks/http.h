#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <mpd/client.h>

#include "../block.h"

/*
struct Site {
    const char* url;
    const char* id;
    long res_code;       // expected response code
};
*/

struct HTTPArgs {
    const char* url;
    const char* id;
    long res_code;       // expected response code
    long timeout;
};

bool get_http(struct Block *block);

#endif
