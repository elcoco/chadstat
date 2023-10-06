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
};

bool get_datetime(struct Block *block);

#endif // !DATETIME_H
