#ifndef MPRIS_CTRL
#define MPRIS_CTRL

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"
#include "../lib/mpris/mpris.h"

#define MPRIS_MAX_BUFFER 256

bool get_mpris(struct Block *block);

int set_mpris(struct Block *block, struct BlockClickEvent *ev);




#endif
