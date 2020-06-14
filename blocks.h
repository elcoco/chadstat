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


typedef struct Block {
    char     name[20];
    bool     enabled;
    int8_t   timeout;

    // function pointer to get_<block>
    bool     (*get)(struct Block *Block);

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[5];
    uint32_t t_last;
    char     text[1024];
    char     text_prev[1024];
} Block;

typedef struct {
    const char* url;
    const char* id;
    long res_code;       // expected response code
} Site;

typedef struct {
    const char* path;
    const char* id;
} Maildir;

bool is_changed(Block *block);
bool is_elapsed(Block *block);

void set_error(Block *block, char* msg);

void get_graph(Block *block, uint8_t len, uint8_t perc, char* col);
void get_strgraph(Block *block, char* str, uint8_t perc, char* col);

bool get_datetime(Block *block);
bool get_volume(Block *block);
bool get_battery(Block *block);
bool get_sites(Block *block);
bool get_wireless(Block *block);
bool get_mpd(Block *block);
bool get_maildirs(Block *block);

// allow config.h to access above functions
#include "config.h"
