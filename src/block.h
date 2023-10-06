#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types

#include "utils.h"

#include "lib/json/json.h"

#define BLOCK_MAX_GRAPH_BUF 64
#define BLOCK_MAX_NAME 32
//#define BLOCK_MAX_INSTANCE 32
#define BLOCK_MAX_SEPARATOR 16

// Modifier key bits
#define BLOCK_CTRL_PRESSED      0b0000000000000001
#define BLOCK_LOCK_PRESSED      0b0000000000000010
#define BLOCK_SHIFT_PRESSED     0b0000000000000100
#define BLOCK_MOD1_PRESSED      0b0000000000010000
#define BLOCK_MOD2_PRESSED      0b0000000000100000
#define BLOCK_MOD3_PRESSED      0b0000000001000000
#define BLOCK_MOD4_PRESSED      0b0000000010000000
#define BLOCK_MOD5_PRESSED      0b0000000100000000
#define BLOCK_LMB_PRESSED       0b0000001000000000
#define BLOCK_MMB_PRESSED       0b0000010000000000
#define BLOCK_RMB_PRESSED       0b0000100000000000
#define BLOCK_MOUSE_SCROLL_UP   0b0001000000000000
#define BLOCK_MOUSE_SCROLL_DOWN 0b0010000000000000


struct Block {
    char     name[BLOCK_MAX_NAME];
    int8_t   timeout;

    // function pointer to get_<block_name> for collecting and setting data
    bool     (*get)(struct Block *Block);
    int      (*set)(struct Block *Block);

    // args must be passed in config.h and will be casted to the right type
    // by the get_* function above
    void    *args;

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[BLOCK_MAX_SEPARATOR];

    //char     instance[BLOCK_MAX_INSTANCE];

    uint32_t t_last;
    char     *text;
    uint32_t  text_len;
    char     *text_prev;
};

struct BlockClickEvent {
    int x;
    int y;
    int xsize;
    int ysize;

    // 16 bit collection of flags containing the modifier keys/mouse buttons/wheel
    unsigned int mod;
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

int block_event_init(struct JSONObject *jo, struct BlockClickEvent *ev);

#endif
