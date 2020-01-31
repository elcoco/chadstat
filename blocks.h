#pragma once

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>      // sites up?

#include <sys/statvfs.h>    // fs info
#include <dirent.h>         // battery info
#include <alsa/asoundlib.h> // alsa master volume info

// wireless
#include <sys/ioctl.h>
#include <linux/wireless.h>

// get network interfaces
#include <ifaddrs.h>
#include <netdb.h>


#include "config.h"
#include "utils.h"

static enum block_type_t {
    TEXT,
    GRAPH,
    STR_GRAPH
} block_type;


class block_t {
    private:
    char last_fmt_text[300] = {'\0'};    // contains a backup of text, gets updated on get*() functions
    uint8_t graph_len = 20;
    char graph_chr1 = '|';
    char graph_chr2 = '|';
    bool is_error = false;

    bool has_changed();

    void get_text(bool sep=1);
    void get_graph();
    void get_strgraph();

    public:
    char fmt_text[300] = {'\0'};
    uint8_t strgraph_value;             // percentage used to make str graph
    char text[50] = {'\0'};
    bool separator = 1;
    char color1[8] = {'\0'};
    char color2[8] = {'\0'};
    block_type_t type = TEXT;           // type, in enum 

    void set_color1(const char* color);
    void set_color2(const char* color);

    void set_error(const char* error);
    bool get(bool sep=1);
};


class block_wireless: public block_t {
    private:
    t_delta_t t_delta;

    bool check_wireless(const char* ifname, char* protocol);
    int8_t get_ifaddr(char* ifname);
    int8_t get_signal_strength(char* interface);

    public:
    block_wireless();
    void get_data();
};

class block_datetime: public block_t {
    private:
    t_delta_t t_delta;

    public:
    block_datetime();
    void get_data();
};

class block_volume: public block_t {
    private:
    t_delta_t t_delta;

    public:
    block_volume();

    void get_data();
};

class block_battery: public block_t {
    private:
    t_delta_t t_delta;

    public:
    block_battery();

    void get_data();
};

class block_fs: public block_t {
    private:
    t_delta_t t_delta;

    uint16_t get_fs_usage(const char* path);
    uint16_t get_fs_total(const char* path);

    public:
    block_fs();

    void get_data();
};

class block_site: public block_t {
    private:
    t_delta_t t_delta;
    site_t site;

    int8_t do_request(const char* url, long& response_code);

    public:
    block_site();

    void set_site( site_t s);
    void get_data();
};
