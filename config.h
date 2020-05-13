#pragma once


// max byte length of status line
#define MAXSTRING 512

#define DEAULT_TIMEOUT      1
#define BLOCK_SEPARATOR             "  "

// needs fixed color status patch, other one is slow as hell
#define CS_NORMAL   "[1]"
#define CS_SELECTED "[2]"
#define CS_OK       "[3]"
#define CS_ERROR    "[4]"

//#define CS_NORMAL   ""
//#define CS_SELECTED ""
//#define CS_WARNING  ""
//#define CS_URGENT   ""

#define DATETIME_TIMEOUT            5                    // seconds inbetween battery checks 
#define BATTERY_TIMEOUT             30 
#define HTTP_TIMEOUT                30 
#define VOLUME_TIMEOUT              1
#define WIRELESS_TIMEOUT            3

#define VOLUME_BAR_LEN             10

#define BATTERY_TRESHOLD            10                   // change color below percentage
#define BATTERY_PATH                "/sys/class/power_supply"
#define BATTERY_BAR_LEN             10

#define DATETIME_FMT                "%a %d:%m:%Y %H:%M"

#define HTTP_CHECK_SECONDS          30                   // seconds inbetween site up checks 
#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout
#define HTTP_SEP_CHR                ':'

#define WIRELESS_PATH               "/proc/net/wireless" // don't change
#define WIRELESS_STRENGTH_TRESHOLD  65                   // change color below percentage


typedef struct {
    uint8_t     timeout;
    bool        is_changed;
    uint32_t    t_last;
    char        text[50];
    char        text_prev[50];
} block_t;


typedef struct {
    const char* url;
    const char* id;
    long res_code;       // expected response code
} site_t;

// check these sites and their respective status codes.
static site_t sites_arr[] = {
//   URL                                ID      EXPECTED STATUS CODE
    {"https://monutor.com",             "MO",   200},
    {"https://api.monutor.com",         "API",  404},
    {"https://dashboard.monutor.com",   "DA",   200},
    {"https://rocketchat.monutor.com",  "RC",   200},
    {"https://nc.freeradical.nl",       "NC",   200}
};
