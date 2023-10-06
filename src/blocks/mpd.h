#ifndef MPD_H
#define MPD_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <mpd/client.h>

#include "../block.h"

bool get_mpd(struct Block *block);
int set_mpd(struct Block *block, struct BlockClickEvent *ev);

#endif
