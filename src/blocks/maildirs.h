#ifndef MAILDIR
#define MAILDIR

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../block.h"


struct MaildirArgs {
    const char* name;
    const char* path;
};

bool get_maildirs(struct Block *block);

#endif
