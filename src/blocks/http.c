#include "http.h"
#include "../config.h"

bool get_http(struct Block *block)
{
    if (block->args == NULL) {
        block_set_error(block, "UNCONFIGURED");
        return block_is_changed(block);
    }
    if (! block_is_elapsed(block))
        return false;

    struct HTTPArgs *http = block->args;

    // clear
    block->text[0] = '\0';

    long rescode;
    long* ptr = &rescode;
    char col[8];
    char siteid[20] = {'\0'};

    uint8_t res = do_request(http->url, ptr);
    
    if (res == CURLE_OK && http->res_code == rescode)
        strcpy(col, block->cs->ok);
    else if (res == CURLE_OPERATION_TIMEDOUT)
        strcpy(col, block->cs->warning);
    else
        strcpy(col, block->cs->error);

    sprintf(siteid, "%s", http->id);

    block_add_text(block, siteid, siteid, col);

    return block_is_changed(block);
}
