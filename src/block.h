#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types

#include "utils.h"

#define BLOCK_MAX_GRAPH_BUF 64
#define BLOCK_MAX_NAME 32
#define BLOCK_MAX_SEPARATOR 16

struct Block {
    char     name[BLOCK_MAX_NAME];
    int8_t   timeout;

    // function pointer to get_<block_name> for collecting and setting data
    bool     (*get)(struct Block *Block);

    // args must be passed in config.h and will be casted to the right type
    // by the get_* function above
    void    *args;

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[BLOCK_MAX_SEPARATOR];

    uint32_t t_last;
    char     *text;
    uint32_t  text_len;
    char     *text_prev;
};


void block_init(struct Block *block);
void block_reset(struct Block *block);
void block_print(struct Block *block, bool last);

bool block_is_changed(struct Block *block);
bool block_is_elapsed(struct Block *block);

void block_set_error(struct Block *block, char* msg);
void block_set_text(struct Block *block, const char *text, const char *color, bool separator);
void block_add_text(struct Block *block, const char *text, const char *color, bool separator);
void block_set_graph(struct Block *block, uint8_t len, uint8_t perc, char* col);
void block_set_strgraph(struct Block *block, char* str, uint8_t perc, char* col);

#endif
