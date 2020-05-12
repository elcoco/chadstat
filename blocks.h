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

#define CS_NORMAL   ""
#define CS_SELECTED ""
#define CS_WARNING  ""
#define CS_URGENT   ""

#define COL_DATETIME CS_WARNING

typedef enum {
    TXT,
    GRAPH,
    TXTGRAPH
} type_t;

// this defines which color scheme should be used
typedef enum {
    NORMAL,
    SELECTED,
    WARNING,
    URGENT
} level_t;


typedef struct {
    type_t      type;
    level_t     level;
    uint32_t    t_last;
    char        text[100];
    char        text_prev[100];
    bool        is_changed;
    uint8_t     timeout;
} block_t;

void set_tlast(block_t* block);
bool is_elapsed(block_t* block);

void get_graph(block_t* block, uint8_t graph_len, uint8_t percent, char* color);
void get_strgraph(block_t* block, char* str, uint8_t percent, char* color);

void get_datetime(block_t* datetime);
/*
struct block_t get_wireless(char* color1, char* color2);
struct block_t get_volume();
struct block_t get_battery();
struct block_t get_fs();
struct block_t get_sites();
*/

