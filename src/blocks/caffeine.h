#ifndef CAFFEINE_H
#define CAFFEINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"


struct CaffeineArgs {
    char *path;
    char *alias;
};

bool get_caffeine(struct Block *block);

#endif
