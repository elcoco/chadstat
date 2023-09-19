#include "http.h"
#include "../config.h"

bool get_http_bak(struct Block *block)
{
    // get lenght of sites array
    uint8_t slen = sizeof(sites)/sizeof(sites[0]);
    uint8_t i;
    char siteid[20] = {'\0'};

    if (! block_is_elapsed(block))
        return false;

    // clear
    block->text[0] = '\0';

    for (i=0 ; i<slen ; i++) {
        struct Site site = sites[i];
        long rescode;
        long* ptr = &rescode;
        char col[8];

        uint8_t res = do_request(site.url, ptr);
        
        if (res == CURLE_OK && site.res_code == rescode)
            strcpy(col, CS_OK);
        else if (res == CURLE_OPERATION_TIMEDOUT)
            strcpy(col, CS_WARNING);
        else
            strcpy(col, CS_ERROR);

        sprintf(siteid, "%s", site.id);

        if (i < slen-1) {
            block_add_text(block, siteid, col, false);
            block_add_text(block, HTTP_SEP_CHR, CS_NORMAL, false);
        }
        else {
            block_add_text(block, siteid, col, true);
        }
    }

    return block_is_changed(block);
}

bool get_http(struct Block *block)
{
    if (block->arg == NULL) {
        block_set_error(block, "UNCONFIGURED");
        return block_is_changed(block);
    }
    if (! block_is_elapsed(block))
        return false;

    struct HTTP *http = block->arg;

    // clear
    block->text[0] = '\0';

    long rescode;
    long* ptr = &rescode;
    char col[8];
    char siteid[20] = {'\0'};

    uint8_t res = do_request(http->url, ptr);
    
    if (res == CURLE_OK && http->res_code == rescode)
        strcpy(col, CS_OK);
    else if (res == CURLE_OPERATION_TIMEDOUT)
        strcpy(col, CS_WARNING);
    else
        strcpy(col, CS_ERROR);

    sprintf(siteid, "%s", http->id);

    block_add_text(block, siteid, col, true);

    return block_is_changed(block);
}
