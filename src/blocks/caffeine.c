#include "caffeine.h"
#include "../config.h"

bool get_caffeine(struct Block *block)
{
    if (! block_is_elapsed(block))
        return false;

    struct CaffeineArgs *args = block->args;

    if (access(args->path, F_OK ) == -1)
        block_reset(block);
    else
        block_set_text(block, args->path, args->alias, block->cs->enabled);

    return block_is_changed(block);
}
