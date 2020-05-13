#include "blocks.h"


bool is_elapsed(block_t* block) {
    // check if time has elapsed, reset time of so
    uint32_t t_cur = time(NULL);

    if ((t_cur - block->t_last) > block->timeout) {
        block->t_last = t_cur;
        return true;
    }
    return false;
}

bool is_changed(block_t* block) {
    // set flag if value has changed
    if (strcmp(block->text, block->text_prev) == 0) {
        return false;
    }
    else {
        strcpy(block->text_prev, block->text);
        return true;
    }
}

void set_error(block_t* block, char* msg) {
    sprintf(block->text, "%s%s", CS_URGENT, msg);
}

void get_graph(block_t* block, uint8_t graph_len, uint8_t percent, char* color) {
    if (percent > 100)
        percent = 100;
    int8_t level = (percent / 100.0) * graph_len;

    char graph_chr1 = '|';
    char graph_chr2 = '|';

    graph_len = 20;

    char l_text[21] = {'\0'};
    char r_text[21] = {'\0'};
    //char l_text[graph_len+1] = {'\0'};
    //char r_text[graph_len+1] = {'\0'};

    for (uint8_t i=0 ; i<level ; i++) {
        l_text[i] = graph_chr1;
    }
    for (uint8_t i=0 ; i<graph_len-level ; i++) {
        r_text[i] = graph_chr2;
    }

    // handle case where level is maximum graph_len, second line would be empty and therefore skipped and screw up spacing between blocks (see i3 input protocol)
    if (level == graph_len)
        sprintf(block->text, "%s%s", color, l_text);
    else
        sprintf(block->text, "%s%s%s%s", color, l_text, CS_NORMAL, r_text);
}

void get_strgraph(block_t* block, char* str, uint8_t percent, char* color) {
    if (percent > 100)
        percent = 100;

    uint8_t graph_len = strlen(str);
    int8_t level = (percent / 100.0) * graph_len;

    //char l_text[graph_len+1] = {'\0'};
    //char r_text[graph_len+1] = {'\0'};

    char l_text[50] = {'\0'};
    char r_text[50] = {'\0'};

    uint8_t index = 0;
    for (uint8_t i=0 ; i<level ; i++) {
        l_text[i] = str[index];
        index++;
    }
    for (uint8_t i=0 ; i<graph_len-level ; i++) {
        r_text[i] = str[index];
        index++;
    }

    // handle case where level is maximum graph_len, second line would be skipped and screw up spacing between blocks (see i3 input protocol)
    if (level == graph_len)
        sprintf(block->text, "%s%s", color, l_text);
    else
        sprintf(block->text, "%s%s%s%s", color, l_text, CS_NORMAL, r_text);
}

bool get_datetime(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return false;
    }

    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buffer[80];

    strftime(buffer, 100, DATETIME_FMT, &tm);
    sprintf(block->text, "%s%s", COL_DATETIME, buffer);
    return is_changed(block);
}

bool get_volume(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return false;
    }

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
    get_graph(block, 21, volume, COL_VOLUME);
    snd_mixer_close(handle);
    return is_changed(block);
}

bool get_battery(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return false;
    }

    char path[100] = {'\0'};
    char capacity_path[100] = {'\0'};

    strcpy(path, BATTERY_PATH);

    struct dirent *de;  // Pointer for directory entry 
    DIR *dr = opendir(path);

    if (dr == NULL) {
        set_error(block, "DIR ERROR");
        closedir(dr);
        return false;
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
        set_error(block, "CAPACITY FILE ERROR");
        return false;
    }

    FILE *fp;

    char buffer[100] = {'\0'};

    fp = fopen(capacity_path, "r");
    if (fp == NULL) {
        set_error(block, "CAPACITY READ ERROR");
    }
    else {
        fgets(buffer, 4, (FILE*)fp);
    }
    // remove trailing newlines
    strtok(buffer, "\n");

    char* color = (atoi(buffer) > BATTERY_TRESHOLD) ? COL_BATTERY_NORMAL : COL_BATTERY_CRITICAL;

    get_graph(block, 21, atoi(buffer), color);

    fclose(fp);
    return is_changed(block);
}

bool get_sites(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return false;
    }

    // get lenght of sites array
    uint8_t sites_len = sizeof(sites_arr)/sizeof(sites_arr[0]);

    char buffer[50];

    for (uint8_t i=0 ; i<sites_len ; i++) {
        site_t site = sites_arr[i];

        long response_code;
        long* ptr = &response_code;

        uint8_t res = do_request(site.url, ptr);

        char color[5];
        
        if (res == CURLE_OK && site.res_code == response_code)
            strcpy(color, COL_HTTP_UP);
        else if (res == CURLE_OPERATION_TIMEDOUT)
            strcpy(color, COL_HTTP_TIMEDOUT);
        else
            strcpy(color, COL_HTTP_DOWN);

        strcat(buffer, color);
        strcat(buffer, site.id);

        if (i < sites_len-1) {
            strcat(buffer, COL_HTTP_SEP);
            strcat(buffer, ":");
        }
    }

    strcpy(block->text, buffer);
    return is_changed(block);
}
        
bool get_wireless(block_t* block) {
    if (! is_elapsed(block)) {
        block->is_changed = false;
        return false;
    }

    // find wireless if address
    char ifaddr[20] = {'\0'};
    if (get_ifaddr(ifaddr) == -1){
        set_error(block, "IF ERROR");
        return is_changed(block);
    }

    int8_t signal;

    if ((signal=get_signal_strength(ifaddr)) == -1) {
        set_error(block, "SIGNAL ERROR");
        return is_changed(block);
    }

    int sockfd;
    //char * id;
    //id = char[IW_ESSID_MAX_SIZE+1];

    char id[IW_ESSID_MAX_SIZE+1];

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    strcpy(wreq.ifr_name, ifaddr);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        set_error(block, "SOCKET ERROR");
        close(sockfd);
        return is_changed(block);
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        set_error(block, "IOCTL ERROR");
        return is_changed(block);
    }
    else {
        if (strlen((char *)wreq.u.essid.pointer) > 0) {
            if (signal > WIRELESS_STRENGTH_TRESHOLD)
                get_strgraph(block, (char *)wreq.u.essid.pointer, signal, COL_WIRELESS_NORMAL);
            else
                get_strgraph(block, (char *)wreq.u.essid.pointer, signal, COL_WIRELESS_LOW);
        }
        else {
            set_error(block, "DISCONNECTED");
            return is_changed(block);
        }
    }
    close(sockfd);
    return is_changed(block);
}
