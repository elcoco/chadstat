#pragma once

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // bool

#include <stdint.h>         // uint types
#include <time.h>           // datetime info

#include <curl/curl.h>      // sites up?

#include <sys/statvfs.h>    // fs info
#include <dirent.h>         // battery info
#include <alsa/asoundlib.h> // alsa master volume info

// wireless
#include <sys/ioctl.h>
#include <linux/wireless.h>

// get network interfaces
#include <ifaddrs.h>
#include <netdb.h>


#include "utils.h"
#include "config.h"


typedef struct {
    uint32_t    t_last;
    char        text[100];
    char        text_prev[100];
    bool        is_changed;
    uint8_t     timeout;
} block_t;

bool is_changed(block_t* block);
bool is_elapsed(block_t* block);

void get_graph(block_t* block, uint8_t graph_len, uint8_t percent, char* color);
void get_strgraph(block_t* block, char* str, uint8_t percent, char* color);

bool get_datetime(block_t* block);
bool get_volume(block_t* block);
bool get_battery(block_t* block);
bool get_sites(block_t* block);

