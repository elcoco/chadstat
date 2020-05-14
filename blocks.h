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
#include "config.h"


typedef struct block_t {
    char     name[20];
    bool     enabled;
    int8_t   timeout;

    // function pointer to get_<block>
    bool     (*get)(struct block_t *block_t);

    int8_t   treshold;
    int8_t   maxlen;
    bool     separator;
    uint32_t t_last;
    char     text[50];
    char     text_prev[50];
} block_t;



bool is_changed(block_t* block);
bool is_elapsed(block_t* block);

void set_error(block_t* block, char* msg);

void get_graph(block_t* block, uint8_t graph_len, uint8_t percent, char* color);
void get_strgraph(block_t* block, char* str, uint8_t percent, char* color);

bool get_datetime(block_t* block);
bool get_volume(block_t* block);
bool get_battery(block_t* block);
bool get_sites(block_t* block);
bool get_wireless(block_t* block);
bool get_mpd(block_t *block);

static block_t block_arr[] = {
//    NAME        ENABLED TIMEOUT  FUNC          TRESHOLD  MAXLEN SEPARATOR
    { "mpd",      false,       5,   get_mpd,       -1,       30,    true},
    { "http",     true,       30,  get_sites,    -1,       -1,    true},
    { "battery",  true,       30,  get_battery,  10,       10,    true},
    { "volume",   true,        3,  get_volume,   -1,       10,    true},
    { "wireless", true,        3,  get_wireless, 50,       -1,    true},
    { "datetime", false,       5,  get_datetime, -1,       -1,    true}
};
