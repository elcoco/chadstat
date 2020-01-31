#include "blocks.h"


bool block_t::has_changed() {
    return (strcmp(last_fmt_text, fmt_text) == 0) ? false : true;
}

void block_t::get_text(bool sep) {
    if (!sep)
        separator = true;
        
    uint8_t sep_block_width = separator ? 20 : 0;
    sprintf(fmt_text, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": %d}", text, color1, sep_block_width);
}

void block_t::get_graph() {
    if (is_error) {
        get_text();
        return;
    }

    uint8_t percent = atoi(text);
    if (percent > 100)
        percent = 100;
    int8_t level = (percent / 100.0) * graph_len;
    uint8_t sep_block_width = separator ? 20 : 0;

    char l_text[graph_len+1] = {'\0'};
    char r_text[graph_len+1] = {'\0'};
    char l_fmt[150]          = {'\0'};
    char r_fmt[150]          = {'\0'};

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
}

void block_t::get_strgraph() {
    if (is_error) {
        get_text();
        return;
    }

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
}

void block_t::set_color1(const char* color) {strcpy(color1, color);}
void block_t::set_color2(const char* color) {strcpy(color2, color);}

// indicate whether there was an error or not, graphs shouldn't be displayed in case of an error
void block_t::set_error(const char* error) {
    is_error = true;
    type = TEXT;            // set type to text in case of error, just display the message alright!
    strcpy(text, error);
}

bool block_t::get(bool sep) {
    switch (type) {
        case TEXT:
            get_text(sep);
            break;
        case GRAPH:
            get_graph();
            break;
        case STR_GRAPH:
            get_strgraph();
            break;
        default:
            set_error("ERROR MAXIMUS");
            get_text(sep);
            break;
    }

    if (has_changed()) {
        strcpy(last_fmt_text, fmt_text);
        return true;
    }
    return false;
}


bool block_wireless::check_wireless(const char* ifname, char* protocol) {
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
        if (protocol) {
            strncpy(protocol, pwrq.u.name, IFNAMSIZ);
            close(sock);
            return 1;
        }
    }

    close(sock);
    return 0;
}

int8_t block_wireless::get_ifaddr(char* ifname) {
    ifaddrs *ifaddr, *ifa;
    int n;

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

int8_t block_wireless::get_signal_strength(char* interface) {
    // exit if file doesn't exist
    if (access(WIRELESS_PATH, F_OK ) == -1)
        return -1;

    FILE *fp;

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

block_wireless::block_wireless() { t_delta_t t_delta; }
        
void block_wireless::get_data() {
    if (!t_delta.has_elapsed(WIRELESS_CHECK_SECONDS))
        return;

    // find wireless if address
    char ifaddr[20] = {'\0'};
    if (get_ifaddr(ifaddr) == -1){
        set_error("IF ERROR");
        return;
    }

    int8_t signal;

    if ((signal=get_signal_strength(ifaddr)) == -1) {
        set_error("SIGNAL ERROR");
        return;
    }

    strgraph_value = signal;
        
    int sockfd;
    char * id;
    id = new char[IW_ESSID_MAX_SIZE+1];

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    strcpy(wreq.ifr_name, ifaddr);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        set_error("SOCKET ERROR");
        close(sockfd);
        return;
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        set_error("IOCTL ERROR");
    }
    else {
        if (strlen((char *)wreq.u.essid.pointer) > 0)
            strcpy(text,  (char *)wreq.u.essid.pointer);
        else {
            set_error("DISCONNECTED");
        }
    }
    close(sockfd);
}



block_datetime::block_datetime() { t_delta_t t_delta; }

void block_datetime::get_data() {
    if (!t_delta.has_elapsed(DATETIME_CHECK_SECONDS))
        return;

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    strftime(text, 100, DATETIME_FMT, &tm);
}


block_volume::block_volume() { t_delta_t t_delta; }

void block_volume::get_data() {
    if (!t_delta.has_elapsed(VOLUME_CHECK_SECONDS))
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

    sprintf(text, "%d", volume);

    snd_mixer_close(handle);
    return;
}


block_battery::block_battery() { t_delta_t t_delta; }

void block_battery::get_data() {
    if (!t_delta.has_elapsed(BATTERY_CHECK_SECONDS))
        return;

    char path[100] = {'\0'};
    char capacity_path[100] = {'\0'};

    strcpy(path, BATTERY_PATH);

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir(path);

    if (dr == NULL) {
        set_error("DIR ERROR");
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
        set_error("CAPACITY FILE ERROR");
        return;
    }

    FILE *fp;

    fp = fopen(capacity_path, "r");
    if (fp == NULL) {
        set_error("CAPACITY READ ERROR");
    }
    else {
        fgets(text, 4, (FILE*)fp);
    }
    // remove trailing newlines
    strtok(text, "\n");

    fclose(fp);
    return;
}


uint16_t block_fs::get_fs_usage(const char* path) {
    struct statvfs stat;
    statvfs(path, &stat);
    return (stat.f_bsize * stat.f_bavail)/1024/1024/1024;
}

uint16_t block_fs::get_fs_total(const char* path) {
    struct statvfs stat;
    statvfs(path, &stat);
    return (stat.f_bsize * stat.f_blocks)/1024/1024/1024;
}

block_fs::block_fs() { t_delta_t t_delta; }

void block_fs::get_data() { }



int8_t block_site::do_request(const char* url, long& response_code) {
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

block_site::block_site::block_site() { t_delta_t t_delta; }

void block_site::set_site( site_t s) {site = s;}

void block_site::get_data() {
    // return if a check is not due
    if (!t_delta.has_elapsed(HTTP_CHECK_SECONDS))
        return;

    sprintf(text, "%s", site.id);

    long response_code;
    uint8_t res = do_request(site.url, response_code);
    
    if (res == CURLE_OK && site.res_code == response_code)
        set_color1(http_color_up);
    else if (res == CURLE_OPERATION_TIMEDOUT)
        set_color1(http_color_timedout);
    else
        set_color1(http_color_down);
}
