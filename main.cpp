#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <dirent.h>     // list dirs

#include <alsa/asoundlib.h>

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <errno.h>
#include <linux/wireless.h>

// get network interfaces
#include <sys/types.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

// pulseaudio (libpulse-dev)
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/volume.h>

#define BATT_PATH "/sys/class/power_supply"
#define TIMEOUT 1

struct colors_t {
    const char orange[8] = "#ad6500";
    const char gray[8]   = "#848484";
    const char white[8]   = "#cccccc";
    const char green[8]   = "#009900";
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
        if (percent > 100)
            percent = 100;
        int8_t level = (percent / 100.0) * length;
        uint8_t sep_block_width = separator ? 20 : 0;

        char l_text[length+1] = {'\0'};
        char r_text[length+1] = {'\0'};
        char l_fmt[150]       = {'\0'};
        char r_fmt[150]       = {'\0'};

        for (uint8_t i=0 ; i<level ; i++) {
            l_text[i] = graph_chr1;
        }
        for (uint8_t i=0 ; i<length-level ; i++) {
            r_text[i] = graph_chr2;
        }

        // handle case where level is maximum length, second line would be skipped and screw up spacing between blocks (see i3 input protocol)
        if (level == length) {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", l_text, color1, sep_block_width);
            sprintf(fmt_text, "%s", l_fmt);
        }
        else {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
            sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
            sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        }

        return fmt_text;
    }

    char* get_strgraph(const char* color1, const char* color2, uint8_t percent) {
        if (is_error)
            return get(color1);

        if (percent > 100)
            percent = 100;

        uint8_t graph_len = strlen(text);
        int8_t level = (percent / 100.0) * graph_len;
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

        // handle case where level is maximum length, second line would be skipped and screw up spacing between blocks (see i3 input protocol)
        if (level == graph_len) {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", l_text, color1, sep_block_width);
            sprintf(fmt_text, "%s", l_fmt);
        }
        else {
            sprintf(l_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", l_text, color1);
            sprintf(r_fmt, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", r_text, color2, sep_block_width);
            sprintf(fmt_text, "%s,\n%s", l_fmt, r_fmt);
        }
        return fmt_text;
    }
};


uint8_t check_wireless(const char* ifname, char* protocol) {
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

block_t get_essid(int16_t* link_quality) {
    block_t block;

    // find wireless if address
    char ifaddr[20] = {'\0'};
    if (get_ifaddr(ifaddr) == -1){
        block.set_error("IF ERROR");
        return block;
    }

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
    close(sockfd);
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

    char path[100] = {'\0'};
    char capacity_path[100] = {'\0'};
    char status_path[100] = {'\0'};
    char status[20] = {'\0'};

    strcpy(path, BATT_PATH);

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir(path);

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

    fclose(fp);
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
                      "I3Status",           // Our application's name.
                      PA_STREAM_PLAYBACK,
                      NULL,               // Use the default device.
                      "Check",            // Description of our stream.
                      &ss,                // Our sample format.
                      NULL,               // Use default channel map
                      NULL,               // Use default buffering attributes.
                      NULL               // Ignore error code.
                      );

    pa_cvolume pa_cvolume;
    char res[500] = {'\0'};
    pa_cvolume_snprint(res, 500, &pa_cvolume);
    printf("\n%s\n", res);
    sleep(5);
    pa_simple_free(s);

    return block;
}


block_t get_alsa_volume()
{
    block_t block;

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
    return block;
}

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

int main() {
    print_header();

    while (1) {
        block_t volume = get_alsa_volume();
        block_t battery = get_batt_level();
        block_t datetime = get_datetime();

        int16_t link_quality;
        block_t essid = get_essid(&link_quality);

        printf("[\n");
        printf("%s,\n", volume.get_graph(colors.green, colors.gray));
        printf("%s,\n", battery.get_graph(colors.orange, colors.gray));
        printf("%s,\n", essid.get_strgraph(colors.orange, colors.gray, 50));
        printf("%s\n",  datetime.get(colors.gray));
        printf("],\n");

        fflush(stdout);     // flush buffer
        sleep(TIMEOUT);
    }

    return 0;
}
