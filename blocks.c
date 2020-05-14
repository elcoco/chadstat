#include "blocks.h"

bool is_elapsed(Block *block) {
    // check if time has elapsed, reset time of so
    uint32_t t_cur = time(NULL);

    if ((t_cur - block->t_last) > block->timeout) {
        block->t_last = t_cur;
        return true;
    }
    return false;
}

bool is_changed(Block *block) {
    // set flag if value has changed
    if (strcmp(block->text, block->text_prev) == 0) {
        return false;
    }
    else {
        strcpy(block->text_prev, block->text);
        return true;
    }
}

void set_error(Block *block, char* msg) {
    sprintf(block->text, "%s%s", CS_ERROR, msg);
}

void get_graph(Block *block, uint8_t len, uint8_t perc, char* col) {
    char graph_chr1 = '|';
    char graph_chr2 = '|';
    char l_text[21] = {'\0'};
    char r_text[21] = {'\0'};
    len = len + 1;
    uint8_t i;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++) {
        l_text[i] = graph_chr1;
    }
    for (i=0 ; i<len-level ; i++) {
        r_text[i] = graph_chr2;
    }
    sprintf(block->text, "%s%s%s%s", col, l_text, CS_NORMAL, r_text);
}

void get_strgraph(Block *block, char* str, uint8_t perc, char* col) {
    uint8_t len = strlen(str);
    char l_text[50] = {'\0'};
    char r_text[50] = {'\0'};
    uint8_t index = 0;
    uint8_t i;

    if (perc > 100)
        perc = 100;

    int8_t level = (perc / 100.0) * len;

    for (i=0 ; i<level ; i++) {
        l_text[i] = str[index];
        index++;
    }
    for (i=0 ; i<len-level ; i++) {
        r_text[i] = str[index];
        index++;
    }
    sprintf(block->text, "%s%s%s%s", col, l_text, CS_NORMAL, r_text);
}

bool get_datetime(Block *block) {
    time_t t = time(NULL);            // 32bit integer representing time
    struct tm tm = *localtime(&t);    // get struct with time data
    char buf[80];

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

    strftime(buf, 100, DATETIME_FMT, &tm);
    sprintf(block->text, "%s%s", CS_NORMAL, buf);
    return is_changed(block);
}

bool get_volume(Block *block) {
    long min, max;
    long level;
    uint16_t volume;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

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
    get_graph(block, block->maxlen, volume, CS_SELECTED);
    snd_mixer_close(handle);
    return is_changed(block);
}

bool get_battery(Block *block) {
    char pwrpath[100] = {'\0'};
    char cappath[100] = {'\0'};
    struct dirent *de;  // Pointer for directory entry 
    FILE *fp;
    char buf[100] = {'\0'};
    char* col;
    DIR *dr;
    strcpy(pwrpath, BATTERY_PATH);

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

    dr = opendir(pwrpath);

    if (dr == NULL) {
        set_error(block, "DIR ERROR");
        closedir(dr);
        return false;
    }

    // find dir containing BAT*
    while ((de = readdir(dr)) != NULL) {
        if (strstr(de->d_name, "BAT") != NULL) {
            strcat(pwrpath, "/");
            strcat(pwrpath, de->d_name);
            break;
        }
    }

    closedir(dr);

    strcat(cappath, pwrpath);
    strcat(cappath, "/capacity");

    // exit if file doesn't exist
    if (access(cappath, F_OK ) == -1) {
        set_error(block, "CAPACITY FILE ERROR");
        return false;
    }

    fp = fopen(cappath, "r");
    if (fp == NULL) {
        set_error(block, "CAPACITY READ ERROR");
    }
    else {
        fgets(buf, 4, (FILE*)fp);
    }
    // remove trailing newlines
    strtok(buf, "\n");

    col = (atoi(buf) > block->treshold) ? CS_OK : CS_SELECTED;

    get_graph(block, block->maxlen, atoi(buf), col);

    fclose(fp);
    return is_changed(block);
}

bool get_sites(Block *block) {
    // get lenght of sites array
    uint8_t slen = sizeof(sites)/sizeof(sites[0]);
    char buf[50] = {'\0'};
    uint8_t i;

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

    for (i=0 ; i<slen ; i++) {
        Site site = sites[i];
        long rescode;
        long* ptr = &rescode;
        char col[strlen(CS_NORMAL+1)];

        uint8_t res = do_request(site.url, ptr);
        
        if (res == CURLE_OK && site.res_code == rescode)
            strcpy(col, CS_OK);
        else if (res == CURLE_OPERATION_TIMEDOUT)
            strcpy(col, CS_SELECTED);
        else
            strcpy(col, CS_ERROR);

        strcat(buf, col);
        strcat(buf, site.id);

        if (i < slen-1) {
            strcat(buf, CS_NORMAL);
            strcat(buf, ":");
        }
    }

    strcpy(block->text, buf);
    return is_changed(block);
}
        
bool get_wireless(Block *block) {
    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

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

    char id[IW_ESSID_MAX_SIZE+1] = {'\0'};

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
            if (signal > block->treshold)
                get_strgraph(block, (char *)wreq.u.essid.pointer, signal, CS_OK);
            else
                get_strgraph(block, (char *)wreq.u.essid.pointer, signal, CS_SELECTED);
        }
        else {
            set_error(block, "DISCONNECTED");
            return is_changed(block);
        }
    }
    close(sockfd);
    return is_changed(block);
}

bool get_mpd(Block *block) {
    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

	struct mpd_connection *conn;

	conn = mpd_connection_new(NULL, 0, 30000);

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        mpd_connection_free(conn);
        strcpy(block->text, "MPD CONNECTION ERROR");
        return is_changed(block);
	}

	struct mpd_status *status;
    struct mpd_song   *song;

    mpd_command_list_begin(conn, true);
    mpd_send_status(conn);
    mpd_send_current_song(conn);
    mpd_command_list_end(conn);

    status = mpd_recv_status(conn);
    if (status == NULL) {
        mpd_connection_free(conn);
        strcpy(block->text, "MPD CONNECTION ERROR");
        return is_changed(block);
    }

    if ( mpd_status_get_state(status) < MPD_STATE_PLAY) {
        strcpy(block->text, "");
        mpd_connection_free(conn);
        return is_changed(block);
    }

    mpd_status_free(status);


    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        mpd_connection_free(conn);
        strcpy(block->text, "MPD CONNECTION ERROR");
        return is_changed(block);
    }

    mpd_response_next(conn);

	const char *artist;
	const char *title;
	const char *track;

    while ((song = mpd_recv_song(conn)) != NULL) {
        artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        track = mpd_song_get_tag(song, MPD_TAG_TRACK, 0);

        snprintf(block->text, block->maxlen, "%s%s - [%s] %s", CS_NORMAL, artist, track, title);
        mpd_song_free(song);
    }
    mpd_connection_free(conn);

    return is_changed(block);
}

bool get_maildirs(Block *block) {
    struct dirent *de;  // Pointer for directory entry 
    char buf[100] = {'\0'};
    DIR *dr;
    uint8_t mdlen = sizeof(maildirs)/sizeof(maildirs[0]);
    uint32_t fc;
    uint8_t i;
    Maildir *md;
    char *col;
    char fcbuf[10];

    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

    if (! is_elapsed(block))
        return false;

    md = &maildirs[0];

    for (i=0 ; i<mdlen ; i++) {
        dr = opendir(md->path);

        if (dr == NULL) {
            set_error(block, "MAILDIR ERROR");
            closedir(dr);
            return false;
        }

        fc = 0;
        while ((de = readdir(dr)) != NULL) {

            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                fc++;
            }
        }

        col = (fc > 0) ? CS_OK : CS_NORMAL;

        sprintf(fcbuf, "%d", fc);

        strcat(buf, col);
        strcat(buf, md->id);
        strcat(buf, CS_NORMAL);
        strcat(buf, fcbuf);

        if (i < mdlen-1)
            strcat(buf, ":");

        md++;

        closedir(dr);
    }


    printf("%s\n", buf);
    strcpy(block->text, buf);
    return is_changed(block);
}
