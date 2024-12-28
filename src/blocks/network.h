#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <ifaddrs.h>        // get network interfaces
#include <netdb.h>
#include <arpa/inet.h>     // inet_ntoa

#include "../block.h"

#ifndef WIRELESS_PATH
  #define WIRELESS_PATH    "/proc/net/wireless"
#endif

#define NW_MAX_STR 256

enum NwState {
    NW_SHOW_SSID,
    NW_SHOW_IP,
};

bool get_network(struct Block *block);
int set_network(struct Block *block, struct BlockClickEvent *ev);

#endif
