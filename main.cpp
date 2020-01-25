#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>      // sites up?

#include <time.h>           // datetime info
#include <sys/statvfs.h>    // fs info
#include <dirent.h>         // battery info
#include <alsa/asoundlib.h> // alsa master volume info

// wireless
#include <sys/ioctl.h>
#include <linux/wireless.h>

// get network interfaces
#include <ifaddrs.h>
#include <netdb.h>

uint8_t timeout;

enum block_type_t {
    TEXT,
    GRAPH,
    STR_GRAPH
} block_type;

// check time differences
struct t_delta_t {
    uint32_t t_prev     = time(NULL);
    bool     first_run  = true;

    // return true when seconds have elapsed, reset timer
    bool has_elapsed(uint16_t seconds) {
        uint32_t t_cur = time(NULL);

        if (first_run) {
            t_prev = t_cur;
            first_run = false;
            return true;
        }

        if ((t_cur - t_prev) > seconds) {
            t_prev = t_cur;
            return true;
        }
        return false;
    }
} t_volume, t_datetime, t_http, t_wireless, t_battery;

struct block_t {
    char text[50] = {'\0'};
    char fmt_text[300] = {'\0'};
    char last_fmt_text[300] = {'\0'};    // contains a backup of text, gets updated on get*() functions
    bool separator = 1;

    uint8_t strgraph_value;             // percentage used to make str graph

    block_type_t type = TEXT;           // type, in enum 

    uint8_t graph_len = 20;

    char graph_chr1 = '|';
    char graph_chr2 = '|';

    char color1[8] = {'\0'};
    char color2[8] = {'\0'};

    bool is_error = false;

    bool is_def() {
        return (strlen(text) == 0) ? 0 : 1;
    }

    // indicate whether there was an error or not, graphs shouldn't be displayed in case of an error
    void set_error(const char* error) {
        is_error = true;
        strcpy(text, error);
    }

    bool has_changed() {
        return (strcmp(last_fmt_text, fmt_text) == 0) ? false : true;
    }


    bool get(bool sep=1) {
        switch (type) {
            case TEXT:
                return get_text(sep);
            case GRAPH:
                return get_graph();
            case STR_GRAPH:
                return get_strgraph();
            default:
                return 0;       // TODO this is nonsense, should return something sensible

        }
    }


    bool get_text(bool sep=1) {
        if (!sep)
            separator = true;
            
        uint8_t sep_block_width = separator ? 20 : 0;
        sprintf(fmt_text, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", text, color1, sep_block_width);

        if (has_changed()) {
            strcpy(last_fmt_text, fmt_text);
            return true;
        }
        return false;
    }

    bool get_graph() {
        if (is_error)
            return get_text();

        uint8_t percent = atoi(text);
        if (percent > 100)
            percent = 100;
        int8_t level = (percent / 100.0) * graph_len;
        uint8_t sep_block_width = separator ? 20 : 0;

        char l_text[graph_len+1] = {'\0'};
        char r_text[graph_len+1] = {'\0'};
        char l_fmt[150]       = {'\0'};
        char r_fmt[150]       = {'\0'};

        for (uint8_t i=0 ; i<level ; i++) {
            l_text[i] = graph_chr1;
        }
        for (uint8_t i=0 ; i<graph_len-level ; i++) {
            r_text[i] = graph_chr2;
        }

        // handle case where level is maximum graph_len, second line would be empty and therefore skipped and screw up spacing between blocks (see i3 input protocol)
        if (level == graph_len) {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", l_text, color1, sep_block_width);
            sprintf(fmt_text, "%s", l_fmt);
        }
        else {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
            sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
            sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        }
        if (has_changed()) {
            strcpy(last_fmt_text, fmt_text);
            return true;
        }
        return false;
    }

    bool get_strgraph() {
        if (is_error)
            return get_text();

        if (strgraph_value > 100)
            strgraph_value = 100;

        uint8_t graph_len = strlen(text);
        int8_t level = (strgraph_value / 100.0) * graph_len;
        uint8_t sep_block_width = separator ? 20 : 0;
        uint8_t index = 0;

        char l_text[graph_len+1] = {'\0'};
        char r_text[graph_len+1] = {'\0'};
        char l_fmt[150]          = {'\0'};
        char r_fmt[150]          = {'\0'};

        for (uint8_t i=0 ; i<level ; i++) {
            l_text[i] = text[index];
            index++;
        }
        for (uint8_t i=0 ; i<graph_len-level ; i++) {
            r_text[i] = text[index];
            index++;
        }

        // handle case where level is maximum graph_len, second line would be skipped and screw up spacing between blocks (see i3 input protocol)
        if (level == graph_len) {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", l_text, color1, sep_block_width);
            sprintf(fmt_text, "%s", l_fmt);
        }
        else {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
            sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
            sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        }
        if (has_changed()) {
            strcpy(last_fmt_text, fmt_text);
            return true;
        }
        return false;
    }
};


bool check_wireless(const char* ifname, char* protocol) {
    int sock = -1;
    struct iwreq pwrq;
    memset(&pwrq, 0, sizeof(pwrq));
    strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        close(sock);
        return 0;
    }

    if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
        if (protocol) strncpy(protocol, pwrq.u.name, IFNAMSIZ); {
            close(sock);
            return 1;
        }
    }

    close(sock);
    return 0;
}

int8_t get_ifaddr(char* ifname) {
    ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        return -1;
    }

    /* Walk through linked list, maintaining head pointer so we
      can free list later */

    for (ifa=ifaddr, n=0 ; ifa!=NULL ; ifa=ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET) {
            freeifaddrs(ifaddr);
            continue;
        }

        char protocol[IFNAMSIZ]  = {0};

        if (check_wireless(ifa->ifa_name, protocol)) {
            strcpy(ifname, ifa->ifa_name);
            freeifaddrs(ifaddr);
            return 0;
        }
    }

    freeifaddrs(ifaddr);
    return -1;
}

uint16_t get_fs_usage(const char* path) {
    struct statvfs stat;
    statvfs(path, &stat);
    return (stat.f_bsize * stat.f_bavail)/1024/1024/1024;
}

uint16_t get_fs_total(const char* path) {
    struct statvfs stat;
    statvfs(path, &stat);
    return (stat.f_bsize * stat.f_blocks)/1024/1024/1024;
}

int8_t get_signal_strength(char* interface) {
    // exit if file doesn't exist
    if (access(WIRELESS_PATH, F_OK ) == -1)
        return -1;

    FILE *fp;
    char buffer[4];

    fp = fopen(WIRELESS_PATH, "r");
    if (fp == NULL) {
        fclose(fp);
        return -1;
    }

    size_t len = 0;
    char* line = NULL;

    // get line containing interface name
    while (getline(&line, &len, fp) != -1) {
        if (strstr(line, interface))
            break;
    }

    fclose(fp);

    if (line == NULL)
        return -1;

    // split string and get 3rd element
    char* tok = strtok(line, " ");
    tok = strtok(NULL, " ");
    tok = strtok(NULL, " .");

    // convert to integer
    return atoi(tok);
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

void show_usage() {
    printf("I3 status line, © 2020 spambaconspam, see LICENSE for details\n");
    printf("usage: statusline [-t seconds] [-h]\n");
}

void parse_args(int* argc, char** argv) {
    char c;
    bool do_exit = false;

    if (*argc < 2) {
        printf("ERROR: no arguments given\n\n");
        do_exit = true;
    }

    while ((c=getopt(*argc, argv, "ht:")) != -1) {
        switch(c) {
            case 't':
                if ((timeout=atoi(optarg)) == 0) {
                    printf("ERROR: invalid argument\n");
                    do_exit = true;
                }
                break;

            case 'h':
                do_exit = true;
                break;

            // for options not in optstring, and missing required arguments
            case '?':       
                do_exit = true;
                break;

            default:
                do_exit = true;
                printf("setting default\n");
                break;

                
        }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for (; optind < *argc; optind++){      
        printf("ERROR: unknown argument: %s\n", argv[optind]);  
        do_exit = true;
    } 

    if (do_exit) {
        show_usage();
        exit(0);
    }
}


void get_essid(block_t& block) {
    if (!t_wireless.has_elapsed(WIRELESS_CHECK_SECONDS))
        return;

    // find wireless if address
    char ifaddr[20] = {'\0'};
    if (get_ifaddr(ifaddr) == -1){
        block.set_error("IF ERROR");
        return;
    }

    int8_t signal;

    if ((signal=get_signal_strength(ifaddr)) == -1) {
        block.set_error("SIGNAL ERROR");
        return;
    }

    block.strgraph_value = signal;
        
    int sockfd;
    char * id;
    id = new char[IW_ESSID_MAX_SIZE+1];

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    strcpy(wreq.ifr_name, ifaddr);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        block.set_error("SOCKET ERROR");
        close(sockfd);
        return;
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        block.set_error("IOCTL ERROR");
    }
    else {
        if (strlen((char *)wreq.u.essid.pointer) > 0)
            strcpy(block.text,  (char *)wreq.u.essid.pointer);
        else {
            block.set_error("DISCONNECTED");
        }
    }
    close(sockfd);
}

void get_datetime(block_t& block) {
    if (!t_datetime.has_elapsed(DATETIME_CHECK_SECONDS))
        return;

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    strftime(block.text, 100, DATETIME_FMT, &tm);
}

void get_batt_level(block_t& block) {
    if (!t_battery.has_elapsed(BATTERY_CHECK_SECONDS))
        return;

    char path[100] = {'\0'};
    char capacity_path[100] = {'\0'};

    strcpy(path, BATTERY_PATH);

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir(path);

    if (dr == NULL) {
        block.set_error("DIR ERROR");
        closedir(dr);
        return;
    }

    // find dir containing BAT*
    while ((de = readdir(dr)) != NULL) {
        if (strstr(de->d_name, "BAT") != NULL) {
            strcat(path, "/");
            strcat(path, de->d_name);
            break;
        }
    }

    closedir(dr);

    strcat(capacity_path, path);
    strcat(capacity_path, "/capacity");

    // exit if file doesn't exist
    if (access(capacity_path, F_OK ) == -1) {
        block.set_error("CAPACITY FILE ERROR");
        return;
    }

    FILE *fp;
    char buffer[4];

    fp = fopen(capacity_path, "r");
    if (fp == NULL) {
        block.set_error("CAPACITY READ ERROR");
    }
    else {
        fgets(block.text, 4, (FILE*)fp);
    }
    // remove trailing newlines
    strtok(block.text, "\n");

    fclose(fp);
    return;
}

void get_alsa_volume(block_t& block) {
    if (!t_volume.has_elapsed(VOLUME_CHECK_SECONDS))
        return;

    long min, max;
    long level;
    uint16_t volume;

    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;

    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &level);

    volume = ((float)level/(float)max)*100.00;

    sprintf(block.text, "%d", volume);

    snd_mixer_close(handle);
    return;
}

int8_t do_request(const char* url, long& response_code) {
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_MAX_TIMEOUT);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_cleanup(curl);
        return res;
    }
    return -1;
}


void get_sites_up(site_t* sites, block_t* blocks, uint8_t length, const char* c_up, const char* c_timeout, const char* c_down) {
    // return if a check is not due
    if (!t_http.has_elapsed(HTTP_CHECK_SECONDS))
        return;

    for (uint8_t i=0 ; i<length ; i++) {
        site_t*  site  = &sites[i];
        block_t* block = &blocks[i];
        sprintf(block->text, "%s", site->id);

        // last block should have a spacer
        block->separator = (i == length-1) ? 1 : 0;

        long response_code;
        uint8_t res = do_request(site->url, response_code);
        
        if (res == CURLE_OK && site->res_code == response_code)
            strcpy(block->color1, c_up);
        else if (res == CURLE_OPERATION_TIMEDOUT)
            strcpy(block->color1, c_timeout);
        else
            strcpy(block->color1, c_down);
    }
}

int main(int argc, char **argv) {
    parse_args(&argc, argv);
    print_header();
        
    uint8_t sites_len = sizeof(sites)/sizeof(sites[0]);
    block_t site_blocks[sites_len];

    block_t volume;
    block_t battery;
    block_t datetime;
    block_t essid;

    strcpy(volume.color1,   volume_color1);
    strcpy(volume.color2,   volume_color2);
    strcpy(battery.color2,  battery_color2);
    strcpy(essid.color2,    wireless_color2);
    strcpy(datetime.color1, datetime_color1);

    // set display type (TEXT, GRAPH, TXT_GRAPH)
    datetime.type = TEXT;
    volume.type   = GRAPH;
    battery.type  = GRAPH;
    essid.type    = STR_GRAPH;

    for (block_t& block: site_blocks)
        block.type = TEXT;

    while (1) {
        int8_t link_quality;

        get_sites_up(sites, site_blocks, sites_len, colors.green, colors.orange, colors.red);

        get_alsa_volume(volume);
        get_batt_level(battery);
        get_datetime(datetime);
        get_essid(essid);

        strcpy(essid.color1, (link_quality >= WIRELESS_STRENGTH_TRESHOLD) ? wireless_color1_good : wireless_color1_bad);
        strcpy(battery.color1, (atoi(battery.text) >= BATTERY_TRESHOLD) ? battery_color1_normal : battery_color1_critical);

        // only update when changes are made
        bool changed = false;

        // TODO stop checking when changed == true
        for (block_t& block: site_blocks) {
            if (block.get())
                changed = true;
        }

        if (battery.get())  changed = true;
        if (volume.get())   changed = true;
        if (essid.get())    changed = true;
        if (datetime.get()) changed = true;

        if (changed) {
            printf("[\n");

            for (block_t& block: site_blocks)
                printf("%s,\n", block.fmt_text);

            printf("%s,\n", battery.fmt_text);
            printf("%s,\n", volume.fmt_text);
            printf("%s,\n", essid.fmt_text);
            printf("%s\n",  datetime.fmt_text);
            printf("],\n");

            fflush(stdout);     // flush buffer
        }

        sleep(timeout);
    }

    return 0;
}
