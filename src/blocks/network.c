#include "network.h"
#include "../config.h"

static bool is_wlan_connected(const char* ifname, char* protocol);
static int8_t get_ifaddr(char* ifname);
static int8_t get_signal_strength(char* interface);


static int8_t get_ifaddr(char* ifname)
{
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

static bool is_wlan_connected(const char* ifname, char* protocol)
{
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

static int8_t get_signal_strength(char* interface)
{
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

bool get_network(struct Block *block)
{
    if (! block_is_elapsed(block))
        return false;

    // find wireless if address
    char ifaddr[20] = {'\0'};
    if (get_ifaddr(ifaddr) == -1){
        block_set_error(block, "IF ERROR");
        return block_is_changed(block);
    }

    int8_t signal;

    if ((signal=get_signal_strength(ifaddr)) == -1) {
        block_set_error(block, "SIGNAL ERROR");
        return block_is_changed(block);
    }

    int sockfd;

    char id[IW_ESSID_MAX_SIZE+1] = {'\0'};

    struct iwreq wreq;
    memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;

    strcpy(wreq.ifr_name, ifaddr);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        block_set_error(block, "SOCKET ERROR");
        goto cleanup;
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        block_set_error(block, "IOCTL ERROR");
        goto cleanup;
    }
    else {
        if (strlen((char *)wreq.u.essid.pointer) > 0) {

            block_reset(block);

            if (signal > block->treshold)
                block_set_strgraph(block, ifaddr, (char *)wreq.u.essid.pointer, signal, block->cs->ok, block->cs->graph_right);
            else
                block_set_strgraph(block, ifaddr, (char *)wreq.u.essid.pointer, signal, block->cs->warning, block->cs->graph_right);

            block_add_text(block, "", "", block->cs->separator);
        }
        else {
            block_set_error(block, "DISCONNECTED");
        }
    }

cleanup:
    close(sockfd);
    return block_is_changed(block);
}
