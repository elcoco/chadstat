#ifndef CONFIG_H
#define CONFIG_H

#include "block.h"
#include "blocks/datetime.h"
#include "blocks/battery.h"
#include "blocks/volume.h"
#include "blocks/network.h"
#include "blocks/mpd.h"
#include "blocks/http.h"
#include "blocks/maildirs.h"
#include "blocks/caffeine.h"

#define CS_NORMAL   "#aaaaaa"
#define CS_WARNING  "#ad6500"
#define CS_OK       "#009900"
#define CS_ERROR    "#ff0000"

/* Uncomment to change default values */
//#define BATTERY_PATH     "/sys/class/power_supply"
//#define DATETIME_FMT     "%a %d:%m:%Y %H:%M"
//#define HTTP_MAX_TIMEOUT 1L                   // max seconds before timeout
//#define HTTP_SEP_CHR     ":"
//#define MAILDIR_SEP_CHR  ":"
//#define WIRELESS_PATH    "/proc/net/wireless"
//#define CAFFEINE_PATH    "/tmp/caffeine.lock"
//#define CAFFEINE_CHR     "KOFFIE!"
//#define SND_CARD_NAME    "default"

#define GRAPH_CHAR_LEFT  '|'
#define GRAPH_CHAR_RIGHT '|'

// check these sites and their respective status codes.
static struct Site sites[] = {
//    URL                                ID      EXPECTED_STATUS_CODE
    { "https://duckduckgo.com",          "DDG",  200 }
};

// check these maildirs for new mail
static struct Maildir maildirs[] = {
//   MAILDIR PATH                  ID
    {"/home/user/mail/Inbox/new",  "MB"}
};

static struct Block blocks[] = {
//    NAME        ENABLED  TIMEOUT  FUNC      TRESHOLD  MAXLEN  SEPARATOR
    { "mpd",      true,          5, get_mpd,        -1,     40,       " "},
    { "caffeine", true,          2, get_caffeine,   -1,     40,       " "},
    { "http",     true,         30, get_http,       -1,     -1,       " "},
    { "battery",  true,         30, get_battery,    10,     10,       " "},
    { "volume",   true,          3, get_volume,     -1,     10,       " "},
    { "maildirs", false,        15, get_maildirs,   -1,     40,       " "},
    { "network",  true,          3, get_network,    60,     -1,       " "},
    { "datetime", true,          5, get_datetime,   -1,     -1,       " "}
};

#endif
