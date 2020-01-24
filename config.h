#include <stdint.h>

// general settings
#define TIMEOUT                     3                   // delay inbetween checks

#define WIRELESS_STRENGTH_TRESHOLD  65                  // change color below percentage
#define BATTERY_TRESHOLD            10                  // change color below percentage
#define DATETIME_FMT                "%A %d:%m:%Y %H:%M"

// sites up
#define HTTP_CHECK_SECONDS          30                   // seconds inbetween site up checks 
#define HTTP_MAX_TIMEOUT            1L                  // max seconds before timeout

// paths, don't change
#define BATT_PATH                   "/sys/class/power_supply"
#define WIRELESS_PATH               "/proc/net/wireless"

// error codes, don't change
#define HTTP_SUCCESS    0
#define HTTP_TIMEDOUT   1
#define HTTP_DOWN       2
#define HTTP_ERROR      3



struct colors_t {
    const char orange[8]    = "#ad6500";
    const char gray[8]      = "#848484";
    const char white[8]     = "#cccccc";
    const char green[8]     = "#009900";
    const char red[8]       = "#F92672";
} colors;

struct site_t {
    const char* url;
    char id;
    uint16_t res_code;       // expected response code
};

site_t sites[] = {
//  URL                                 ID   EXPECTED STATUS CODE
    {"https://api.monutor.com",         'A', 404},
    {"https://dashboard.monutor.com",   'D', 200},
    {"https://rocketchat.monutor.com",  'R', 200},
    {"https://monutor.com",             'M', 200},
    {"https://nc.freeradical.nl",       'N', 200},
    //{"https://httpstat.us/200?sleep=2000",  'D', 200},
};

