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


typedef struct block_t {
    char     name[20];
    bool     enabled;
    int8_t   timeout;

    // function pointer to get_<block>
    bool     (*get)(struct block_t *block_t);

    int8_t   treshold;
    int8_t   maxlen;
    char     sep_chr[5];
    uint32_t t_last;
    char     text[50];
    char     text_prev[50];
} block_t;

typedef struct {
    const char* url;
    const char* id;
    long res_code;       // expected response code
} site_t;


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

// allow config.h to access above functions
#include "config.h"
