#ifndef CAFFEINE_H
#define CAFFEINE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"


#ifndef CAFFEINE_CHR
  #define CAFFEINE_CHR     "KOFFIE!"
#endif

#ifndef CAFFEINE_PATH
  #define CAFFEINE_PATH    "/tmp/caffeine.lock"
#endif

bool get_caffeine(struct Block *block);

#endif
