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


typedef struct {
    uint8_t  timeout;
    bool     is_changed;
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
