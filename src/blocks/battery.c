#include "battery.h"
#include "../config.h"


bool get_battery(struct Block *block) {
    char pwrpath[100] = {'\0'};
    char cappath[100] = {'\0'};
    struct dirent *de;  // Pointer for directory entry 
    FILE *fp;
    char buf[100] = {'\0'};
    char* col;
    DIR *dr;
    strcpy(pwrpath, BATTERY_PATH);

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! block_is_elapsed(block))
        return false;

    dr = opendir(pwrpath);

    if (dr == NULL) {
        block_set_error(block, "DIR ERROR");
        closedir(dr);
        return false;
    }

    // find dir containing BAT*
    while ((de = readdir(dr)) != NULL) {
        if (strstr(de->d_name, "BAT") != NULL) {
            strcat(pwrpath, "/");
            strcat(pwrpath, de->d_name);
            break;
        }
    }

    closedir(dr);

    strcat(cappath, pwrpath);
    strcat(cappath, "/capacity");

    // exit if file doesn't exist
    if (access(cappath, F_OK ) == -1) {
        block_set_error(block, "CAPACITY FILE ERROR");
        return false;
    }

    fp = fopen(cappath, "r");
    if (fp == NULL) {
        block_set_error(block, "CAPACITY READ ERROR");
    }
    else {
        fgets(buf, 4, (FILE*)fp);
    }
    // remove trailing newlines
    strtok(buf, "\n");

    col = (atoi(buf) > block->treshold) ? CS_OK : CS_WARNING;

    block_get_graph(block, block->maxlen, atoi(buf), col);

    fclose(fp);
    return block_is_changed(block);
}
