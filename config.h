#pragma once

#define MAXSTRING           512         // max byte length of status line
#define DEAULT_TIMEOUT      1           // timeout in seconds

// needs fixed color status patch, other one is slow as hell
#define CS_NORMAL   "[1]"
#define CS_SELECTED "[2]"
#define CS_OK       "[3]"
#define CS_ERROR    "[4]"

#define BATTERY_PATH                "/sys/class/power_supply"
#define DATETIME_FMT                "%a %d:%m:%Y %H:%M"
#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout
#define HTTP_SEP_CHR                ':'
#define WIRELESS_PATH               "/proc/net/wireless" // don't change


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
