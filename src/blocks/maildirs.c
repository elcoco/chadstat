#include "maildirs.h"
#include "../config.h"

bool get_maildirs(struct Block *block)
{
    struct dirent *de;  // Pointer for directory entry 
    DIR *dr;
    uint8_t mdlen = sizeof(maildirs)/sizeof(maildirs[0]);
    uint32_t fc;
    uint8_t i;
    struct Maildir *md;
    char fcbuf[10];
    char mdbuf[10];
    const char *pptr;
    bool newmail = false;

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! block_is_elapsed(block))
        return false;

    md = &maildirs[0];

    // clear
    block->text[0] = '\0';

    for (i=0 ; i<mdlen ; i++) {
        char path[100] = {'\0'};

        // expand ~ to homedir
        if (*md->path == '~') {
            pptr = md->path;        // remove ~
            pptr++;
            strcat(path, getenv("HOME"));  // add homedir
            strcat(path, pptr);
        }
        else
            strcpy(path, md->path);

        dr = opendir(path);

        if (dr == NULL) {
            block_set_error(block, "MAILDIR ERROR");
            closedir(dr);
            return false;
        }

        fc = 0;
        while ((de = readdir(dr)) != NULL) {

            // count files in maildir
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
                fc++;
        }

        // only show if there is mail
        if (fc > 0) {

            // show separator if not the first
            if (newmail)
                block_add_text(block, MAILDIR_SEP_CHR, CS_NORMAL, false);

            sprintf(fcbuf, "%d", fc);
            sprintf(mdbuf, "%s", md->id);

            block_add_text(block, mdbuf, CS_OK, false);
            block_add_text(block, fcbuf, CS_NORMAL, false);

            // set flag
            newmail = true;
        }
        md++;
        closedir(dr);
    }

    // display block separator if there is new mail
    if (newmail)
        block_add_text(block, block->sep_chr, CS_NORMAL, false);

    return block_is_changed(block);
}
