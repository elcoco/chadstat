#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types

#include "utils.h"

#include "lib/json/json.h"

#define BLOCK_MAX_GRAPH_BUF 64
#define BLOCK_MAX_NAME 32
#define BLOCK_MAX_INSTANCE 128
#define BLOCK_MAX_SEPARATOR 16

// Modifier key bits
#define BLOCK_CTRL_PRESSED      0x01 << 0
#define BLOCK_LOCK_PRESSED      0x01 << 1
#define BLOCK_SHIFT_PRESSED     0x01 << 2
#define BLOCK_MOD1_PRESSED      0x01 << 3
#define BLOCK_MOD2_PRESSED      0x01 << 4
#define BLOCK_MOD3_PRESSED      0x01 << 5
#define BLOCK_MOD4_PRESSED      0x01 << 6
#define BLOCK_MOD5_PRESSED      0x01 << 7
#define BLOCK_LMB_PRESSED       0x01 << 8
#define BLOCK_MMB_PRESSED       0x01 << 9
#define BLOCK_RMB_PRESSED       0x01 << 10
#define BLOCK_MOUSE_SCROLL_UP   0x01 << 11
#define BLOCK_MOUSE_SCROLL_DOWN 0x01 << 12

#ifndef BLOCK_GRAPH_CHAR_LEFT
    #define BLOCK_GRAPH_CHAR_LEFT  '|'
#endif

#ifndef BLOCK_GRAPH_CHAR_RIGHT
    #define BLOCK_GRAPH_CHAR_RIGHT '|'
#endif



struct BlockClickEvent {
    int x;
    int y;
    int xsize;
    int ysize;

    char instance[256];

    // 16 bit collection of flags containing the modifier keys/mouse buttons/mouse wheel actions
    unsigned int mod;
};

struct ColorScheme {
    const char *label;

    const char *normal;
    const char *ok;
    const char *warning;
    const char *error;

    const char *enabled;
    const char *disabled;

    const char *active;
    const char *inactive;

    const char *graph_left;
    const char *graph_right;

    const char *separator_block;    // separator within a block
    const char *separator;          // separator inbetween blocks
};

struct Block {
    char     name[BLOCK_MAX_NAME];
    int8_t   timeout;

    // function pointer to get_<block_name> for collecting and setting data
    bool     (*get)(struct Block *Block);
    int      (*set)(struct Block *Block, struct BlockClickEvent *ev);

    // args must be passed in config.h and will be casted to the right type
    // by the get_* function above
    void    *args;
    struct ColorScheme *cs;

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[BLOCK_MAX_SEPARATOR];

    char     instance[BLOCK_MAX_INSTANCE];

    uint32_t t_last;
    char     *text;
    uint32_t  text_len;
    char     *text_prev;
    int state; // debugging
};




void block_init(struct Block *block);
void block_reset(struct Block *block);
void block_print(struct Block *block, bool last);

bool block_is_changed(struct Block *block);
bool block_is_elapsed(struct Block *block);

void block_set_error(struct Block *block, char* msg);
void block_set_text(struct Block *block, const char *instance, const char *text, const char *color);
void block_add_text(struct Block *block, const char *instance, const char *text, const char *color);
void block_set_graph(struct Block *block, const char *instance, uint8_t len, uint8_t perc, const char* lcol, const char *rcol);
void block_set_strgraph(struct Block *block, const char *instance, char* str, uint8_t perc, const char *lcol, const char *rcol);

int block_event_init(struct JSONObject *jo, struct BlockClickEvent *ev);

#endif
