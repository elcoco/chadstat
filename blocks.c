#include "blocks.h"

void get_datetime(block_t* block, char* color) {
    //if (!t_delta.has_elapsed(DATETIME_CHECK_SECONDS))
    //    return;

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buffer[80];

    strftime(buffer, 100, DATETIME_FMT, &tm);

    if (strcmp(buffer, block->text) == 0)
        block->is_changed = false;
    else
        block->is_changed = true;

    strcpy(block->text, buffer);
}
