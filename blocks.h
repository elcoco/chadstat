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

