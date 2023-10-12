#include "maildirs.h"
#include "../config.h"

bool get_maildirs(struct Block *block)
{
    uint32_t fcount;
    char fcbuf[10];
    char mdbuf[10];
    const char *pptr;

    DIR *dir = NULL;
    struct dirent *de = NULL;  // don't free: statically allocated in readdir()

    struct MaildirArgs *args = block->args;

    if (! block_is_elapsed(block))
        return false;

    if (block->args == NULL) {
        block_set_error(block, "UNCONFIGURED");
        return block_is_changed(block);
    }

    block->text[0] = '\0';
    char path[100] = "";

    // expand ~ to homedir
    if (*args->path == '~') {
        pptr = args->path;        // remove ~
        pptr++;
        strcat(path, getenv("HOME"));  // add homedir
        strcat(path, pptr);
    }
    else {
        strcpy(path, args->path);
    }

    dir = opendir(path);
    if (dir == NULL) {
        block_set_error(block, "MAILDIR ERROR");
        goto on_cleanup;
    }

    fcount = 0;
    while ((de = readdir(dir)) != NULL) {

        // count files in maildir
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
            fcount++;
    }

    // only show if there is mail
    if (fcount > 0) {
        sprintf(fcbuf, "%d", fcount);
        sprintf(mdbuf, "%s", args->name);

        block_add_text(block, mdbuf, mdbuf, block->cs->label);
        block_add_text(block, mdbuf, fcbuf, block->cs->normal);
    }

on_cleanup:
    if (dir)
        closedir(dir);

    return block_is_changed(block);
}
