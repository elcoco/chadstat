#include "datetime.h"
#include "../config.h"

enum DtState {
    DT_STATE_SHORT,
    DT_STATE_LONG,
};

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

    switch (block->state) {
        case DT_STATE_SHORT:
            strftime(buf, 100, args->fmt, &tm);
            break;
        case DT_STATE_LONG:
            strftime(buf, 100, args->alt_fmt, &tm);
            break;
        default:
            block_set_error(block, "NO FMT");
            break;
    }

    
    block_set_text(block, "datetime", buf, block->cs->normal);
    return block_is_changed(block);
}

int set_datetime(struct Block *block, struct BlockClickEvent *ev)
{
    switch (block->state) {
        case DT_STATE_SHORT:
            block->state = DT_STATE_LONG;
            break;
        case DT_STATE_LONG:
            block->state = DT_STATE_SHORT;
            break;
        default:
            block->state = DT_STATE_LONG;
            break;
    }
    return 1;
}
