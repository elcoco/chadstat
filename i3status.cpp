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


//#define IW_INTERFACE "wlp1s0"
#define IW_INTERFACE "wlp3s0"

struct colors_t {
    const char orange[8] = "#ad6500";
    const char gray[8]   = "#848484";
} colors;

struct block_t {
    char full_text[50] = {'\0'};
    char color[10]      = {'\0'};
    char fmt_text[300]  = {'\0'};
    char lgraph[30]    = {'\0'};
    char rgraph[30]    = {'\0'};
    bool separator = 1;

    uint8_t graph_length = 20;
    char    graph_chr1 = '|';
    char    graph_chr2 = '|';
    char    graph_color1[10]      = {'\0'};
    char    graph_color2[10]      = {'\0'};
    bool    is_graph = false;
    bool    is_strgraph = false;

    void set_graph(const char* color1, const char* color2) {
        is_graph = true;
        sprintf(graph_color1, "%s", color1);
        sprintf(graph_color2, "%s", color2);
    }

    void set_strgraph(const char* color1, const char* color2) {
        is_strgraph = true;
        sprintf(graph_color1, "%s", color1);
        sprintf(graph_color2, "%s", color2);
    }

    char* get(bool graph=false) {
        uint8_t sep_block_width = separator ? 20 : 0;
        if (is_graph) {
            get_graph();
            char l[300] = {'\0'};
            char r[300] = {'\0'};
            sprintf(l, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", lgraph, graph_color1);
            sprintf(r, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", rgraph, graph_color2, sep_block_width);
            sprintf(fmt_text, "%s,\n%s", l, r);
        }
        /*
        else if (is_strgraph) {
            get_strgraph();
            char l[300] = {'\0'};
            char r[300] = {'\0'};
            sprintf(l, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", lgraph, graph_color1);
            sprintf(r, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", rgraph, graph_color2, sep_block_width);
            sprintf(fmt_text, "%s,%s", l, r);
        }
        */
        else {
            sprintf(fmt_text, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", full_text, color, sep_block_width);
        }
        return fmt_text;
    }

    void get_graph() {
        is_graph = 1;
        uint8_t percent = atoi(full_text);
        int8_t level = (percent / 100.0) * graph_length;

        for (uint8_t i=0 ; i<level ; i++) {
            lgraph[i] = graph_chr1;
        }
        for (uint8_t i=0 ; i<graph_length-level ; i++) {
            rgraph[i] = graph_chr2;
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
};


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


block_t get_essid() {
    block_t block;

    int sockfd;
    char * id;
    id = new char[IW_ESSID_MAX_SIZE+1];

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    sprintf(wreq.ifr_name, IW_INTERFACE);


    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        sprintf(block.full_text, "%s", "socket error");
        return block;
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        sprintf(block.full_text, "%s: %d", "ioctl error", errno);
    }
    else {
        sprintf(block.full_text, "%s",  (char *)wreq.u.essid.pointer);
    }

    return block;
}

block_t get_datetime(const char* color) {
    block_t block;
    sprintf(block.color, "%s", color);

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    strftime(block.full_text, 100,"%A %Y:%m:%d %H:%M",&tm);

    return block;
}

block_t get_batt_level(const char* path) {
    block_t block;

    FILE *fp;
    char buffer[4];

    fp = fopen(path, "r");
    if (fp == NULL) {
        block.full_text[0] = '0';
    } else {
        fgets(block.full_text, 4, (FILE*)fp);
    }
    strtok(block.full_text, "\n");

    return block;
}

void get_block(char* buffer, const char* full_text, const char* color, bool sep=1) {
    uint8_t sep_block_width = sep ? 20 : 0;
    sprintf(buffer, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", full_text, color, sep_block_width);
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}
void print_footer() {
    printf("],");
}

int main() {
    print_header();

    while (1) {

        block_t battery = get_batt_level("/sys/class/power_supply/BAT1/capacity");
        battery.set_graph(colors.orange, colors.gray);

        block_t datetime = get_datetime(colors.gray);

        block_t essid = get_essid();

        printf("[\n");
        printf("%s,\n", battery.get());
        printf("%s,\n", essid.get());
        printf("%s\n", datetime.get());
        printf("],\n");

        fflush(stdout);     // flush buffer
        sleep(5);
    }

    return 0;


}
