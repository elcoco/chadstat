#ifndef BATTERY_H
#define BATTERY_H

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <dirent.h>

#include "../block.h"

#ifndef BATTERY_PATH
  #define BATTERY_PATH     "/sys/class/power_supply"
#endif

bool get_battery(struct Block *block);

#endif
