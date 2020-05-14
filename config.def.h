#pragma once

#define MAXSTRING           512         // max byte length of status line

// needs fixed color status patch, other one is slow as hell
#define CS_NORMAL   "[1]"
#define CS_SELECTED "[2]"
#define CS_OK       "[3]"
#define CS_ERROR    "[4]"

#define BATTERY_PATH                "/sys/class/power_supply"
#define DATETIME_FMT                "%a %d:%m:%Y %H:%M"
#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout
#define HTTP_SEP_CHR                ':'
#define MAILDIR_SEP_CHR                ':'
#define WIRELESS_PATH               "/proc/net/wireless" // don't change


// check these sites for status codes.
static Site sites[] = {
//   URL                                ID      EXPECTED STATUS CODE
    {"https://google.com",              "GG",   200}
};

// check these sites and their respective status codes.
static Maildir maildirs[] = {
//   MAILDIR PATH                  ID
    {"/home/user/mail/Inbox/new",  "MB"}
};

static Block blocks[] = {
//    NAME        ENABLED TIMEOUT  FUNC      TRESHOLD  MAXLEN  SEPARATOR
    { "mpd",      true,        5,  get_mpd,       -1,      40,     " "},
    { "http",     true,       30,  get_sites,     -1,      -1,     " "},
    { "battery",  true,       30,  get_battery,   10,      10,     " "},
    { "volume",   true,        3,  get_volume,    -1,      10,     " "},
    { "maildirs", false,      15,  get_maildirs,  -1,      40,     " "},
    { "wireless", true,        3,  get_wireless,  60,      -1,     " "},
    { "datetime", true,        5,  get_datetime,  -1,      -1,     " "}
};
