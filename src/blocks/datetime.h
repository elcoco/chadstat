#ifndef DATETIME_H
#define DATETIME_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "../block.h"

#ifndef DATETIME_FMT
  #define DATETIME_FMT "%a %d:%m:%Y %H:%M"
#endif


bool get_datetime(struct Block *block);

#endif // !DATETIME_H
