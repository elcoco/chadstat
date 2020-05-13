#pragma once

#include <curl/curl.h>      // sites up?

#include "config.h"
#include "blocks.h"


int8_t do_request(const char* url, long* response_code);
