#ifndef NW_USAGE_H
#define NW_USAGE_H

#include <stdio.h>
#include <stdbool.h>        // bool
#include <stdint.h>         // uint types

#include "utils.h"
#include "../block.h"

#define NW_USAGE_CMD_FMT "vnstat -i %s --oneline"
#define NW_USAGE_MAX_CMD_SIZE 256

#define NW_USAGE_MAX_FIELD_SIZE 16
#define NW_USAGE_AMOUNT_FIELDS  15

// what data to show?
enum NWUsageType {
    NW_USAGE_TOTAL_TODAY,
    NW_USAGE_TOTAL_MONTH,
    NW_USAGE_TOTAL_ALLTIME,
};

struct NWUsageArgs {
    const char *iface_name;
    const char *alias;
    enum NWUsageType type;
};

struct NWUsageParsed {
    int api_version;
    char iface[NW_USAGE_MAX_FIELD_SIZE];

    char ts_today[NW_USAGE_MAX_FIELD_SIZE];
    char rx_today[NW_USAGE_MAX_FIELD_SIZE];
    char tx_today[NW_USAGE_MAX_FIELD_SIZE];
    char total_today[NW_USAGE_MAX_FIELD_SIZE];
    char avg_rate_today[NW_USAGE_MAX_FIELD_SIZE];
    
    char ts_month[NW_USAGE_MAX_FIELD_SIZE];
    char rx_month[NW_USAGE_MAX_FIELD_SIZE];
    char tx_month[NW_USAGE_MAX_FIELD_SIZE];
    char total_month[NW_USAGE_MAX_FIELD_SIZE];
    char avg_rate_month[NW_USAGE_MAX_FIELD_SIZE];

    char rx_all_time[NW_USAGE_MAX_FIELD_SIZE];
    char tx_all_time[NW_USAGE_MAX_FIELD_SIZE];
    char total_all_time[NW_USAGE_MAX_FIELD_SIZE];
};

bool get_nw_usage(struct Block *block);



#endif
