#pragma once

#define DATETIME_FMT                "%A %d:%m:%Y %H:%M"

#define CS_NORMAL   ""
#define CS_SELECTED ""
#define CS_WARNING  ""
#define CS_URGENT   ""

#define COL_DATETIME         CS_NORMAL
#define COL_VOLUME           CS_SELECTED
#define COL_BATTERY_NORMAL   CS_WARNING
#define COL_BATTERY_CRITICAL CS_SELECTED

#define COL_HTTP_UP          CS_WARNING
#define COL_HTTP_TIMEDOUT    CS_SELECTED
#define COL_HTTP_DOWN        CS_URGENT
#define COL_HTTP_SEP         CS_NORMAL

#define DATETIME_TIMEOUT            5                    // seconds inbetween battery checks 
#define BATTERY_TIMEOUT             30 
#define HTTP_TIMEOUT                30 
#define VOLUME_TIMEOUT              1

#define BATTERY_TRESHOLD            10                   // change color below percentage
#define BATTERY_PATH                   "/sys/class/power_supply"

#define HTTP_CHECK_SECONDS          30                   // seconds inbetween site up checks 
#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout
#define HTTP_SEP_CHR                :


#define BLOCK_SEPARATOR             "  "

typedef struct {
    const char* url;
    const char* id;
    long res_code;       // expected response code
} site_t;

// check these sites and their respective status codes.
static site_t sites_arr[] = {
//   URL                                ID   EXPECTED STATUS CODE
    {"https://monutor.com",             "MO",   200},
    {"https://api.monutor.com",         "API",  404},
    {"https://dashboard.monutor.com",   "DA",   200},
    {"https://rocketchat.monutor.com",  "RC",   200},
    {"https://nc.freeradical.nl",       "NC",   200}
};
