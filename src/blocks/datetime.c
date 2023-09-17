#include "datetime.h"
#include "../config.h"

bool get_datetime(struct Block *block)
{
    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buf[80];

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! block_is_elapsed(block))
        return false;

    strftime(buf, 100, DATETIME_FMT, &tm);
    block_set_text(block, buf, CS_NORMAL, true);
    return block_is_changed(block);
}
