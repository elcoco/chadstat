#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types



#include "utils.h"

#define BLOCK_MAX_TEXT_BUF 256
#define BLOCK_MAX_JSON_BUF 2048

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
    //char     text[BLOCK_MAX_JSON_BUF];
    //char     text_prev[BLOCK_MAX_JSON_BUF];
    char     *text;
    char     *text_prev;
    uint32_t  text_len;
};


void block_init(struct Block *block);

void block_print(struct Block *block, bool last);
void block_reset(struct Block *block);

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
