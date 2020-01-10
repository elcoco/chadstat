#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <dirent.h>     // list dirs


#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <errno.h>
#include <linux/wireless.h>

// pulseaudio (libpulse-dev)
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/volume.h>

#define IW_INTERFACE "wlp1s0"
#define BATT_PATH "/sys/class/power_supply"
#define TIMEOUT 3
//#define IW_INTERFACE "wlp3s0"

struct colors_t {
    const char orange[8] = "#ad6500";
    const char gray[8]   = "#848484";
    const char white[8]   = "#cccccc";
} colors;

struct block_t {
    char text[50] = {'\0'};
    char fmt_text[300] = {'\0'};
    bool separator = 1;

    char    graph_chr1 = '|';
    char    graph_chr2 = '|';

    bool is_error = false;

    // indicate whether there was an error or not, graphs shouldn't be displayed in case of an error
    void set_error(const char* error) {
        is_error = true;
        strcpy(text, error);
    }

    char* get(const char* color) {
        uint8_t sep_block_width = separator ? 20 : 0;
        sprintf(fmt_text, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", text, color, sep_block_width);
        return fmt_text;
    }

    char* get_graph(const char* color1, const char* color2, uint8_t length=20) {
        if (is_error)
            return get(color1);

        uint8_t percent = atoi(text);
        int8_t level = (percent / 100.0) * length;
        uint8_t sep_block_width = separator ? 20 : 0;

        char l_text[length+1] = {'\0'};
        char r_text[length+1] = {'\0'};
        char l_fmt[300]       = {'\0'};
        char r_fmt[300]       = {'\0'};

        for (uint8_t i=0 ; i<level ; i++) {
            l_text[i] = graph_chr1;
        }
        for (uint8_t i=0 ; i<length-level ; i++) {
            r_text[i] = graph_chr2;
        }

        sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
        sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
        sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        return fmt_text;
    }

    char* get_strgraph(const char* color1, const char* color2, uint8_t percent) {
        if (is_error)
            return get(color1);

        uint8_t graph_len = strlen(text);
        int8_t level = (percent / 100.0) * graph_len;
        uint8_t sep_block_width = separator ? 20 : 0;
        uint8_t index = 0;

        char l_text[graph_len+1] = {'\0'};
        char r_text[graph_len+1] = {'\0'};
        char l_fmt[300]          = {'\0'};
        char r_fmt[300]          = {'\0'};

        for (uint8_t i=0 ; i<level ; i++) {
            l_text[i] = text[index];
            index++;
        }
        for (uint8_t i=0 ; i<graph_len-level ; i++) {
            r_text[i] = text[index];
            index++;
        }

        sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
        sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
        sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        return fmt_text;
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

    strcpy(wreq.ifr_name, IW_INTERFACE);


    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        block.set_error("SOCKET ERROR");
        return block;
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

    return block;
}

block_t get_datetime() {
    block_t block;

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    strftime(block.text, 100,"%A %d:%m:%Y %H:%M",&tm);

    return block;
}

block_t get_batt_level() {
    block_t block;

    char path[300] = {'\0'};
    char capacity_path[300] = {'\0'};
    char status_path[300] = {'\0'};
    char status[300] = {'\0'};

    strcpy(path, BATT_PATH);

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir (path);

    if (dr == NULL) {
        block.set_error("DIR ERROR");
        return block;
    }

    // find dir containing BAT*
    while ((de = readdir(dr)) != NULL) {
        if (strstr(de->d_name, "BAT") != NULL) {
            strcat(path, "/");
            strcat(path, de->d_name);
            break;
        }
    }
    strcat(capacity_path, path);
    strcat(capacity_path, "/capacity");
    strcat(status_path, path);
    strcat(status_path, "/status");


    // exit if file doesn't exist
    if (access(capacity_path, F_OK ) == -1) {
        block.set_error("CAPACITY FILE ERROR");
        return block;
    }
    // exit if file doesn't exist
    if (access(status_path, F_OK ) == -1) {
        block.set_error("STATUS FILE ERROR");
        return block;
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

    return block;
}

block_t get_pavolume() {
    block_t block;

    pa_simple *s;
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 44100;
    s = pa_simple_new(NULL,               // Use the default server.
                      "Fooapp",           // Our application's name.
                      PA_STREAM_PLAYBACK,
                      NULL,               // Use the default device.
                      "Music",            // Description of our stream.
                      &ss,                // Our sample format.
                      NULL,               // Use default channel map
                      NULL,               // Use default buffering attributes.
                      NULL               // Ignore error code.
                      );

    pa_cvolume pa_cvolume;
    char res[500] = {'\0'};
    pa_cvolume_snprint(res, 500, &pa_cvolume);
    //printf("\n%s\n", res);

    return block;
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

int main() {
    print_header();

    while (1) {
        block_t volume = get_pavolume();
        block_t battery = get_batt_level();
        block_t datetime = get_datetime();
        block_t essid = get_essid();

        printf("[\n");
        printf("%s,\n", battery.get_graph(colors.orange, colors.gray));
        printf("%s,\n", essid.get_strgraph(colors.orange, colors.gray, 50));
        printf("%s\n",  datetime.get(colors.gray));
        printf("],\n");

        fflush(stdout);     // flush buffer
        sleep(TIMEOUT);
    }

    return 0;


}
