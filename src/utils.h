#ifndef UTILS_H
#define UTILS_H

//#include "block.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <curl/curl.h>      // sites up?
                            //
// wireless
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <ifaddrs.h>        // get network interfaces
#include <netdb.h>


int8_t do_request(const char* url, long* response_code);
bool is_wlan_connected(const char* ifname, char* protocol);
int8_t get_ifaddr(char* ifname);
int8_t get_signal_strength(char* interface);
void i3ify(char *buf, char *text, char *color);

#endif
