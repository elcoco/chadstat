#include "utils.h"
#include "config.h"


int8_t do_request(const char* url, long* response_code) {
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_MAX_TIMEOUT);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
        curl_easy_cleanup(curl);
        return res;
    }
    return -1;
}

bool is_wlan_connected(const char* ifname, char* protocol) {
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

int8_t get_ifaddr(char* ifname) {
    struct ifaddrs *ifaddr, *ifa;
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

        if (is_wlan_connected(ifa->ifa_name, protocol)) {
            strcpy(ifname, ifa->ifa_name);
            freeifaddrs(ifaddr);
            return 0;
        }
    }

    freeifaddrs(ifaddr);
    return -1;
}

int8_t get_signal_strength(char* interface) {
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

void i3ify(char *buf, char *text, char *color) {
    sprintf(buf, "{\"full_text\": \"%s\", \"color\": \"%s\", \"separator\": false, \"separator_block_width\": 0}", text, color);
}
