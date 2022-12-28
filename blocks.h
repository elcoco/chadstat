#pragma once

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types

#include <curl/curl.h>      // sites up?

#include <dirent.h>         // battery info
#include <alsa/asoundlib.h> // alsa master volume info

// wireless
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <ifaddrs.h>        // get network interfaces
#include <netdb.h>

#include <mpd/client.h>     // mpd

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

struct Site {
    const char* url;
    const char* id;
    long res_code;       // expected response code
};

struct Maildir {
    const char* path;
    const char* id;
};

bool is_changed(struct Block *block);
bool is_elapsed(struct Block *block);

void set_error(struct Block *block, char* msg);
void set_text(struct Block *block, char *text, char *color, bool separator);
void add_text(struct Block *block, char *text, char *color, bool separator);

void get_graph(struct Block *block, uint8_t len, uint8_t perc, char* col);
void get_strgraph(struct Block *block, char* str, uint8_t perc, char* col);

bool get_datetime(struct Block *block);
bool get_volume(struct Block *block);
bool get_battery(struct Block *block);
bool get_sites(struct Block *block);
bool get_wireless(struct Block *block);
bool get_mpd(struct Block *block);
bool get_maildirs(struct Block *block);
bool get_caffeine(struct Block *block);

// allow config.h to access above functions
#include "config.h"
