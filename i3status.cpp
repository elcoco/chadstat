#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>
#include <unistd.h>


#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <errno.h>
#include <linux/wireless.h>


#define IW_INTERFACE "wlp1s0"

struct colors_t {
    const char orange[8] = "#ad6500";
    const char gray[8]   = "#848484";
} colors;

struct block_t {
    char full_text[200] = {'\0'};
    char color[10]      = {'\0'};
    char fmt_text[500]  = {'\0'};
    char lgraph[200]    = {'\0'};
    char rgraph[200]    = {'\0'};
    bool separator = 1;
    uint8_t length = 20;
    char chr1 = 'x';
    char chr2 = '0';
    bool is_graph = 0;

    char* get() {
        uint8_t sep_block_width = separator ? 20 : 0;
        if (is_graph)
            sprintf(fmt_text, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", full_text, color, sep_block_width);
        else {
            get_graph();
            char l[300] = {'\0'};
            char r[300] = {'\0'};
            sprintf(l, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", lgraph, color);
            sprintf(r, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", rgraph, color, sep_block_width);
            sprintf(fmt_text, "%s,%s", l, r);
        }
        return fmt_text;
    }

    void get_graph() {
        is_graph = 1;
        uint8_t percent = atoi(full_text);
        int8_t level = (percent / 100.0) * length;

        for (uint8_t i=0 ; i<level ; i++) {
            lgraph[i] = chr1;
        }
        for (uint8_t i=0 ; i<length-level ; i++) {
            rgraph[i] = chr2;
        }
    }
};


void get_graph(char* lgraph, char* rgraph, uint8_t length, uint8_t percent, char chr1='x', char chr2='o') {
    int8_t level = (percent / 100.0) * length;

    for (uint8_t i=0 ; i<level ; i++) {
        lgraph[i] = chr1;
    }
    for (uint8_t i=0 ; i<length-level ; i++) {
        rgraph[i] = chr2;
    }
}

void get_strgraph(char* lgraph, char* rgraph, char* inp_str, uint8_t length, uint8_t percent) {
    int8_t level = (percent / 100.0) * length;
    uint8_t index = 0;

    for (uint8_t i=0 ; i<level ; i++) {
        lgraph[i] = inp_str[index];
        index++;
    }
    for (uint8_t i=0 ; i<length-level ; i++) {
        rgraph[i] = inp_str[index];
        index++;
    }
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


void get_essid(char* essid) {
    int sockfd;
    char * id;
    id = new char[IW_ESSID_MAX_SIZE+1];

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    sprintf(wreq.ifr_name, IW_INTERFACE);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        sprintf(essid, "%s", "socket error");
        return;
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;
    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        sprintf(essid, "%s: %d", "ioctl error", errno);
        return;
    }

    sprintf(essid, "%s",  (char *)wreq.u.essid.pointer);
}

void get_datetime(block_t& block, const char* color) {
    sprintf(block.color, "%s", color);

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    strftime(block.full_text, 100,"%A %Y:%m:%d %H:%M",&tm);

    return;
}

void get_batt_level(block_t& block, const char* path, const char* color) {
    sprintf(block.color, "%s", color);

    FILE *fp;
    char buffer[4];

    fp = fopen(path, "r");
    if (fp == NULL) {
        block.full_text[0] = '0';
    } else {
        fgets(block.full_text, 4, (FILE*)fp);
    }
    strtok(block.full_text, "\n");

    return;
}

void get_block(char* buffer, const char* full_text, const char* color, bool sep=1) {
    uint8_t sep_block_width = sep ? 20 : 0;
    sprintf(buffer, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", full_text, color, sep_block_width);
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

int main() {
    print_header();

    while (1) {
        char essid[81] = {'\0'};
        char essid_lgraph[81] = {'\0'};
        char essid_rgraph[81] = {'\0'};

        get_essid(essid);
        get_strgraph(essid_lgraph, essid_rgraph, essid, strlen(essid), 50);

        char essid_lblock[200] = {'\0'};
        char essid_rblock[200] = {'\0'};

        //get_block(essid_lblock, essid_lgraph, colors.orange, 0);
        //get_block(essid_rblock, essid_rgraph, colors.gray);
        block_t datetime;
        block_t battery;

        //get_batt_level(battery, "/sys/class/power_supply/BAT0/capacity", colors.orange);
        get_datetime(datetime, colors.orange);
        battery.is_graph = 1;

        //printf("[%s,%s,%s,%s],\n", battery.get(), essid_lblock, essid_rblock, datetime.get());
        printf("[%s],\n", datetime.get());

        fflush(stdout);     // flush buffer
        sleep(5);
    }

    return 0;


}
