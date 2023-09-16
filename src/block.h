#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types



#include "utils.h"


struct Block {
    char     name[20];
    bool     enabled;
    int8_t   timeout;

    // function pointer to get_<block>
    bool     (*get)(struct Block *Block);

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[10];
    uint32_t t_last;
    char     text[2048];
    char     text_prev[2048];
};

bool block_is_changed(struct Block *block);
bool block_is_elapsed(struct Block *block);

void block_set_error(struct Block *block, char* msg);
void block_set_text(struct Block *block, char *text, char *color, bool separator);
void block_add_text(struct Block *block, char *text, char *color, bool separator);

void block_get_graph(struct Block *block, uint8_t len, uint8_t perc, char* col);
void block_get_strgraph(struct Block *block, char* str, uint8_t perc, char* col);

//bool get_sites(struct Block *block);
//bool get_maildirs(struct Block *block);
//bool get_caffeine(struct Block *block);

#endif
