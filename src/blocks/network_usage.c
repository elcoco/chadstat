#include "network_usage.h"
#include "../config.h"

int get_line(char *cmd, char *buf, int maxlen)
{
    int len = 0;
    FILE *fp;
    char c;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

    while ((c = fgetc(fp)) != EOF) {
        if (len+2 >= maxlen)
            return -1;

        if (c == '\n') {
            buf[len+1] = '\0';
            return 1;
        }

        buf[len] = c;
        len++;
    }

    if (pclose(fp)) {
        printf("Command not found or exited with error status\n");
        return -1;
    }

    return -1;
}

int parse_line(struct NWUsageParsed *nw, const char *buf)
{
    int field = 0;
    int index = 0;

    char *c = buf;
    char tmp[NW_USAGE_MAX_FIELD_SIZE] = "";

    for (int i=0 ; i<strlen(buf) ; i++, c++) {
        tmp[index] = *c;
        index++;
        if (index > NW_USAGE_MAX_FIELD_SIZE) {
            return -1;
        }

        // end of field
        if (*c == ';') {

            tmp[index-1] = '\0';
            switch (field) {
                case 0:
                    nw->api_version = atoi(tmp);
                    break;
                case 1:
                    strcpy(nw->iface, tmp);
                    break;
                case 2:
                    strcpy(nw->ts_today, tmp);
                    break;
                case 3:
                    strcpy(nw->rx_today, tmp);
                    break;
                case 4:
                    strcpy(nw->tx_today, tmp);
                    break;
                case 5:
                    strcpy(nw->total_today, tmp);
                    break;
                case 6:
                    strcpy(nw->avg_rate_today, tmp);
                    break;
                case 7:
                    strcpy(nw->ts_month, tmp);
                    break;
                case 8:
                    strcpy(nw->rx_month, tmp);
                    break;
                case 9:
                    strcpy(nw->tx_month, tmp);
                    break;
                case 10:
                    strcpy(nw->total_month, tmp);
                    break;
                case 11:
                    strcpy(nw->avg_rate_month, tmp);
                    break;
                case 12:
                    strcpy(nw->rx_all_time, tmp);
                    break;
                case 13:
                    strcpy(nw->tx_all_time, tmp);
                    break;

            }
            field++;
            index = 0;
            tmp[0] = '\0';
            continue;

        }
        if (i == strlen(buf)-1) {
            tmp[index] = '\0';
            strcpy(nw->total_all_time, tmp);
            return 0;
        }
    }
    return -1;
}

void nw_usage_debug(struct NWUsageParsed *nw)
{
    printf("api_version: %d\n", nw->api_version);
    printf("iface:       %s\n", nw->iface);

    printf("ts_today:       %s\n", nw->ts_today);
    printf("rx_today:       %s\n", nw->rx_today);
    printf("tx_today:       %s\n", nw->tx_today);
    printf("total_today:    %s\n", nw->total_today);
    printf("avg_rate_today: %s\n", nw->avg_rate_today);

    printf("ts_month:       %s\n", nw->ts_month);
    printf("rx_month:       %s\n", nw->rx_month);
    printf("tx_month:       %s\n", nw->tx_month);
    printf("total_month:    %s\n", nw->total_month);
    printf("avg_rate_month: %s\n", nw->avg_rate_month);

    printf("rx_all_time:    %s\n", nw->rx_all_time);
    printf("tx_all_time:    %s\n", nw->tx_all_time);
    printf("total_all_time: %s\n\n", nw->total_all_time);
}

bool get_nw_usage(struct Block *block)
{
    char cmd[NW_USAGE_MAX_CMD_SIZE] = "";

    struct NWUsageArgs *args = block->arg;

    sprintf(cmd, NW_USAGE_CMD_FMT, args->iface_name);

    char buf[256] = "";
    if (get_line(cmd, buf, 256) < 0) {
        block_set_error(block, "VNSTAT ERROR");
        return false;
    }

    struct NWUsageParsed nw_usage;
    if (parse_line(&nw_usage, (const char*)buf) < 0) {
        block_set_error(block, "IFACE ERROR");
        return false;
    }

    block_set_text(block, nw_usage.iface, CS_OK, false);
    block_add_text(block, ":", CS_NORMAL, false);
    block_add_text(block, nw_usage.total_today, CS_NORMAL, true);

    return block_is_changed(block);
}
