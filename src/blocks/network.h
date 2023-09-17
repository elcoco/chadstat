#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <ifaddrs.h>        // get network interfaces
#include <netdb.h>

#include "../block.h"

#ifndef WIRELESS_PATH
  #define WIRELESS_PATH    "/proc/net/wireless"
#endif

bool get_network(struct Block *block);

#endif
