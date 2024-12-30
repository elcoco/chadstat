#ifndef DATETIME_H
#define DATETIME_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "../block.h"


struct DateTimeArgs {
    const char *fmt;
    const char *alt_fmt;
};

bool get_datetime(struct Block *block);
int set_datetime(struct Block *block, struct BlockClickEvent *ev);

#endif // !DATETIME_H
