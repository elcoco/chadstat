#include "http.h"
#include "../config.h"


static int8_t do_request(const char* url, long* response_code, long timeout)
{
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
        curl_easy_cleanup(curl);
        return res;
    }
    return -1;
}


bool get_http(struct Block *block)
{
    if (block->args == NULL) {
        block_set_error(block, "UNCONFIGURED");
        return block_is_changed(block);
    }
    if (! block_is_elapsed(block))
        return false;

    struct HTTPArgs *args = block->args;

    // clear
    block->text[0] = '\0';

    long rescode;
    long* ptr = &rescode;
    char col[8];
    char siteid[20] = {'\0'};

    uint8_t res = do_request(args->url, ptr, args->timeout);
    
    if (res == CURLE_OK && args->res_code == rescode)
        strcpy(col, block->cs->ok);
    else if (res == CURLE_OPERATION_TIMEDOUT)
        strcpy(col, block->cs->warning);
    else
        strcpy(col, block->cs->error);

    sprintf(siteid, "%s", args->id);

    block_add_text(block, siteid, siteid, col);

    return 1;
}
