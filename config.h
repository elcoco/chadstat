#pragma once

#define MAXSTRING           512         // max byte length of status line
#define DEAULT_TIMEOUT      1           // timeout in seconds

// needs fixed color status patch, other one is slow as hell
#define CS_NORMAL   "[1]"
#define CS_SELECTED "[2]"
#define CS_OK       "[3]"
#define CS_ERROR    "[4]"

//#define CS_NORMAL   ""
//#define CS_SELECTED ""
//#define CS_WARNING  ""
//#define CS_URGENT   ""

// minimum timeout inbetween checks, in seconds
#define DATETIME_TIMEOUT            5
#define BATTERY_TIMEOUT             30 
#define HTTP_TIMEOUT                30 
#define VOLUME_TIMEOUT              1
#define WIRELESS_TIMEOUT            3
#define MPD_TIMEOUT                 3

#define VOLUME_BAR_LEN             10

#define BATTERY_TRESHOLD            10                   // change color below percentage
#define BATTERY_PATH                "/sys/class/power_supply"
#define BATTERY_BAR_LEN             10

#define DATETIME_FMT                "%a %d:%m:%Y %H:%M"

#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout
#define HTTP_SEP_CHR                ':'

#define WIRELESS_PATH               "/proc/net/wireless" // don't change
#define WIRELESS_STRENGTH_TRESHOLD  65                   // change color below percentage


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
    {"https://nc.chmod777.org",         "NC",   200}
};
