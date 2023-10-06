#include "datetime.h"
#include "../config.h"

bool get_datetime(struct Block *block)
{
    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buf[80];

    if (! block_is_elapsed(block))
        return false;

    if (block->args == NULL) {
        block_set_error(block, "UNCONFIGURED");
        return block_is_changed(block);
    }

    struct DateTimeArgs *args = block->args;

    strftime(buf, 100, args->fmt, &tm);
    block_set_text(block, "datetime", buf, CS_NORMAL, true);
    return block_is_changed(block);
}
