#include "block.h"
#include "config.h"


/* Helper functions *///////////////////////////////////////////////////////////

bool block_is_elapsed(struct Block *block) {
    // check if time has elapsed, reset time of so
    uint32_t t_cur = time(NULL);

    if ((t_cur - block->t_last) > block->timeout) {
        block->t_last = t_cur;
        return true;
    }
    return false;
}

bool block_is_changed(struct Block *block) {
    // set flag if value has changed
    if (strcmp(block->text, block->text_prev) == 0) {
        return false;
    }
    else {
        strcpy(block->text_prev, block->text);
        return true;
    }
}

void block_set_error(struct Block *block, char* msg) {
    block_set_text(block, msg, CS_ERROR, true);
}

void block_set_text(struct Block *block, char *text, char *color, bool separator) {
    char buf[256] = {'\0'};
    if (strlen(text) > 0) {
        i3ify(buf, text, color);
        sprintf(block->text, "%s,\n", buf);
    }

    if (separator) {
        block_add_text(block, block->sep_chr, CS_NORMAL, false);
    }
}

void block_add_text(struct Block *block, char *text, char *color, bool separator) {
    // append text to block
    char buf[1024] = {'\0'};
    if (strlen(text) > 0) {
        i3ify(buf, text, color);
        sprintf(block->text, "%s%s,\n", block->text, buf);
    }

    if (separator) {
        block_add_text(block, block->sep_chr, CS_NORMAL, false);
    }
}

void block_get_graph(struct Block *block, uint8_t len, uint8_t perc, char* col) {
    char graph_chr1 = GRAPH_CHAR_LEFT;
    char graph_chr2 = GRAPH_CHAR_RIGHT;
    char l_text[21] = {'\0'};
    char r_text[21] = {'\0'};
    len = len + 1;
    uint8_t i;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++) {
        l_text[i] = graph_chr1;
    }
    for (i=0 ; i<len-level ; i++) {
        r_text[i] = graph_chr2;
    }
    block->text[0] = '\0';
    block_add_text(block, l_text, col, false);
    block_add_text(block, r_text, CS_NORMAL, true);
}

void block_get_strgraph(struct Block *block, char* str, uint8_t perc, char* col) {
    uint8_t len = strlen(str);
    char l_text[50] = {'\0'};
    char r_text[50] = {'\0'};
    uint8_t index = 0;
    uint8_t i;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++) {
        l_text[i] = str[index];
        index++;
    }
    for (i=0 ; i<len-level ; i++) {
        r_text[i] = str[index];
        index++;
    }
    block->text[0] = '\0';
    block_add_text(block, l_text, col, false);
    block_add_text(block, r_text, CS_NORMAL, true);
}
