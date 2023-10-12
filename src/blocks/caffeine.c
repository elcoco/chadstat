#include "caffeine.h"
#include "../config.h"

bool get_caffeine(struct Block *block)
{
    char buf[100] = CAFFEINE_CHR;

    if (! block_is_elapsed(block))
        return false;

    if (access(CAFFEINE_PATH, F_OK ) == -1)
        block_reset(block);
    else
        block_set_text(block, CAFFEINE_PATH, buf, block->cs->enabled);

    return block_is_changed(block);
}
