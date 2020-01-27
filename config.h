#include <stdint.h>

#define SEPARATOR_STR               ":"                  // chars to display as a separator (if enabled)

#define WIRELESS_PATH               "/proc/net/wireless" // don't change
#define WIRELESS_STRENGTH_TRESHOLD  65                   // change color below percentage
#define WIRELESS_CHECK_SECONDS      5                    // seconds inbetween wireless checks 

#define DATETIME_FMT                "%A %d:%m:%Y %H:%M"
#define DATETIME_CHECK_SECONDS      5                    // seconds inbetween battery checks 

#define VOLUME_CHECK_SECONDS        1                    // seconds inbetween battery checks 

#define BATTERY_TRESHOLD            10                   // change color below percentage
#define BATTERY_CHECK_SECONDS       30                   // seconds inbetween battery checks 
#define BATTERY_PATH                   "/sys/class/power_supply"

#define HTTP_CHECK_SECONDS          30                   // seconds inbetween site up checks 
#define HTTP_MAX_TIMEOUT            1L                   // max seconds before timeout


// colors
struct colors_t {
    const char orange[8]    = "#ad6500";
    const char gray[8]      = "#848484";
    const char white[8]     = "#cccccc";
    const char green[8]     = "#009900";
    const char red[8]       = "#F92672";
} colors;

const char* battery_color1_critical  = colors.orange;
const char* battery_color1_normal    = colors.green;
const char* battery_color2           = colors.gray;

const char* wireless_color1_bad      = colors.orange;
const char* wireless_color1_good     = colors.green;
const char* wireless_color2          = colors.gray;

const char* datetime_color1          = colors.gray;

const char* volume_color1            = colors.orange;
const char* volume_color2            = colors.gray;

const char* http_color_up            = colors.green;
const char* http_color_timedout      = colors.orange;
const char* http_color_down          = colors.red;


struct site_t {
    const char* url;
    const char* id;
    long res_code;       // expected response code
};

site_t sites[] = {
//   URL                                ID   EXPECTED STATUS CODE
    {"https://monutor.com",             "Mo",   200},
    {"https://api.monutor.com",         "Api",  404},
    {"https://dashboard.monutor.com",   "Da",   200},
    {"https://rocketchat.monutor.com",  "Rc",   200},
    {"https://nc.freeradical.nl",       "Nc",   200}
};
