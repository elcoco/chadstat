#ifndef MAILDIR
#define MAILDIR

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"


struct Maildir {
    const char* path;
    const char* id;
};

#ifndef MAILDIR_SEP_CHR
  #define MAILDIR_SEP_CHR  ":"
#endif

bool get_maildirs(struct Block *block);

#endif
