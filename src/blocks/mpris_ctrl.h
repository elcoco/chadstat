#ifndef MPRIS_CTRL
#define MPRIS_CTRL

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"
#include "../lib/mpris/mpris.h"

#define MPRIS_MAX_BUFFER 256
//#define MPRIS_CHR_PLAY  "▶" unicode
#define MPRIS_CHR_PLAY  ""

//#define MPRIS_CHR_PAUSE "Ⅱ"
#define MPRIS_CHR_PAUSE ""

bool get_mpris(struct Block *block);

int set_mpris(struct Block *block, struct BlockClickEvent *ev);




#endif
