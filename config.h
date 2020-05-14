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



// check these sites and their respective status codes.
static site_t sites_arr[] = {
//   URL                                ID      EXPECTED STATUS CODE
    {"https://monutor.com",             "MO",   200},
    {"https://api.monutor.com",         "API",  404},
    {"https://dashboard.monutor.com",   "DA",   200},
    {"https://rocketchat.monutor.com",  "RC",   200},
    {"https://nc.chmod777.org",         "NC",   200}
};

static block_t block_arr[] = {
//    NAME        ENABLED TIMEOUT  FUNC      TRESHOLD  MAXLEN  SEPARATOR
    { "mpd",      true,        5,  get_mpd,       -1,      40,     " "},
    { "http",     true,       30,  get_sites,     -1,      -1,     " "},
    { "battery",  true,       30,  get_battery,   10,      10,     " "},
    { "volume",   true,        3,  get_volume,    -1,      10,     " "},
    { "wireless", true,        3,  get_wireless,  50,      -1,     "  "},
    { "datetime", true,        5,  get_datetime,  -1,      -1,     " "}
};
