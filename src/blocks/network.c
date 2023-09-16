#include "network.h"
#include "../config.h"

bool get_network(struct Block *block) {
    if (!block->enabled) {
        strcpy(block->text, "");
        return false;
    }

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
        close(sockfd);
        return block_is_changed(block);
    }

    // ioctl manipulates device parameters of special files
    wreq.u.essid.pointer = id;

    if (ioctl(sockfd,SIOCGIWESSID, &wreq) == -1) {
        block_set_error(block, "IOCTL ERROR");
        return block_is_changed(block);
    }
    else {
        if (strlen((char *)wreq.u.essid.pointer) > 0) {
            if (signal > block->treshold)
                block_get_strgraph(block, (char *)wreq.u.essid.pointer, signal, CS_OK);
            else
                block_get_strgraph(block, (char *)wreq.u.essid.pointer, signal, CS_WARNING);
        }
        else {
            block_set_error(block, "DISCONNECTED");
            return block_is_changed(block);
        }
    }
    close(sockfd);
    return block_is_changed(block);
}
