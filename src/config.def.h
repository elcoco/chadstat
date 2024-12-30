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
#include "blocks/network_usage.h"
#include "blocks/mpris_ctrl.h"

//#define BLOCK_GRAPH_CHAR_LEFT  '|'
//#define BLOCK_GRAPH_CHAR_RIGHT '|'

// check these maildirs for new mail
//                                      NAME PATH
static struct MaildirArgs maildir0 = {"MD1", "/path/to/Inbox1/new"};
static struct MaildirArgs maildir1 = {"MD2", "/path/to/Inbox2/new"};

// check these sites and their respective status codes.
//                                  URL                        ID      EXPECTED_STATUS_CODE  TIMEOUT
static struct HTTPArgs http_ddg = { "https://duckduckgo.com",  "DDG",  200,                  1L };

static struct NWUsageArgs nw_usage_wlan = { "wlp0s20f3", "WLAN", NW_USAGE_TOTAL_TODAY };
static struct DateTimeArgs dt = {"%a %H:%M","%a %d:%m:%Y %H:%M"};

static struct CaffeineArgs caffeine = { "/tmp/caffeine.lock", "KOFFIE!" };

static struct ColorScheme cs = { .label           = "#f92672",
                                 .normal          = "#aaaaaa",
                                 .ok              = "#009900",
                                 .warning         = "#fd971f",
                                 .error           = "#ff0000",
                                 .enabled         = "#fd5ff0",
                                 .disabled        = "#5B6078",
                                 .active          = "#009900",
                                 .inactive        = "#5B6078",
                                 .graph_left      = "#009900",
                                 .graph_right     = "#5b6078",
                                 .separator_block = "#aaaaaa",
                                 .separator       = "#aaaaaa" };

static struct Block blocks[] = {
//    NAME        TIMEOUT  GET_CB                SET_CB,           ARGS            CS     TRESHOLD  MAXLEN  SEPARATOR
    { "mpris",          5, get_mpris,            set_mpris,        NULL,           &cs,    -1,     20,       " " },
    { "mpd",            5, get_mpd,              set_mpd,          NULL,           &cs,    -1,     20,       " " },
    { "caffeine",       5, get_caffeine,         NULL,             &caffeine,      &cs,    -1,     40,       " " },
    { "http_ddg",      30, get_http,             NULL,             &http_ddg,      &cs,    -1,     -1,       ":" },
    { "battery",       30, get_battery,          NULL,             NULL,           &cs,    10,     10,       " " },
    { "volume",         3, get_pulse_volume,     set_pulse_volume, NULL,           &cs,    -1,     10,       " " },
    { "maildirs0",     15, get_maildirs,         NULL,             &maildir0,      &cs,    -1,     40,       ":" },
    { "maildirs1",     15, get_maildirs,         NULL,             &maildir1,      &cs,    -1,     40,       " " },
    { "network_usage", 30, get_nw_usage,         NULL,             &nw_usage_wlan, &cs,    60,     -1,       " " },
    { "network",        3, get_network,          set_network,      NULL,           &cs,    60,     -1,       " " },
    { "datetime",       5, get_datetime,         set_datetime,     &dt,            &cs,    -1,     -1,       " " }
};

#endif
