#pragma once

#include "blocks.h"


int8_t do_request(const char* url, long* response_code);
bool is_wlan_connected(const char* ifname, char* protocol);
int8_t get_ifaddr(char* ifname);
int8_t get_signal_strength(char* interface);
void i3ify(char *buf, char *text, char *color);
