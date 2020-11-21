#pragma once

#define CS_NORMAL   "#848484"
#define CS_WARNING  "#ad6500"
#define CS_OK       "#009900"
#define CS_ERROR    "#ff0000"

#define BATTERY_PATH     "/sys/class/power_supply"
#define DATETIME_FMT     "%a %d:%m:%Y %H:%M"
#define HTTP_MAX_TIMEOUT 1L                   // max seconds before timeout
#define HTTP_SEP_CHR     ":"
#define MAILDIR_SEP_CHR  ":"
#define WIRELESS_PATH    "/proc/net/wireless" // don't change
#define CAFFEINE_PATH    "/tmp/caffeine.pid" // don't change
#define CAFFEINE_CHR     "C" // don't change

#define GRAPH_CHAR_LEFT  '|'
#define GRAPH_CHAR_RIGHT '|'

// check these sites and their respective status codes.
static Site sites[] = {
//   URL                                ID      EXPECTED STATUS CODE
    {"https://duckduckgo.com",          "DDG",  200}
};

// check these maildirs for new mail
static Maildir maildirs[] = {
//   MAILDIR PATH                  ID
    {"/home/user/mail/Inbox/new",  "MB"}
};

static Block blocks[] = {
//    NAME        ENABLED  TIMEOUT  FUNC      TRESHOLD  MAXLEN  SEPARATOR
    { "mpd",      true,          5, get_mpd,        -1,     40,       " "},
    { "caffeine", true,          5,  get_caffeine,  -1,     40,       " "},
    { "http",     true,         30, get_sites,      -1,     -1,       " "},
    { "battery",  true,         30, get_battery,    10,     10,       " "},
    { "volume",   true,          3, get_volume,     -1,     10,       " "},
    { "maildirs", false,        15, get_maildirs,   -1,     40,       " "},
    { "wireless", true,          3, get_wireless,   60,     -1,       " "},
    { "datetime", true,          5, get_datetime,   -1,     -1,       " "}
};
