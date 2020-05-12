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


//#include "config.h"
#include "utils.h"


#define DATETIME_FMT                "%A %d:%m:%Y %H:%M"

typedef enum {
    TXT,
    GRAPH,
    TXTGRAPH
} type_t;

// this defines which color scheme should be used
typedef enum {
    OK,
    WARNING,
    ERROR
} level_t;


typedef struct {
    type_t      type;
    level_t     level;
    uint32_t    t_prev;
    char        text[100];
    bool        is_changed;
} block_t;

int8_t get_graph(block_t block, uint8_t len, uint8_t percent);
int8_t get_strgraph(block_t block, char* str, uint8_t percent);

void get_datetime(block_t* datetime, char* color1);
/*
struct block_t get_wireless(char* color1, char* color2);
struct block_t get_volume();
struct block_t get_battery();
struct block_t get_fs();
struct block_t get_sites();
*/

