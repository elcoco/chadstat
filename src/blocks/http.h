#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <mpd/client.h>

#include "../block.h"

struct Site {
    const char* url;
    const char* id;
    long res_code;       // expected response code
};

struct HTTP {
    const char* url;
    const char* id;
    long res_code;       // expected response code
};

#ifndef HTTP_MAX_TIMEOUT
  #define HTTP_MAX_TIMEOUT 1L // max seconds before timeout
#endif

#ifndef HTTP_SEP_CHR
  #define HTTP_SEP_CHR     ":"
#endif

bool get_http(struct Block *block);

#endif
