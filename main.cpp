#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "blocks.h"


uint8_t timeout;

void print_header() {
    printf("{ \"version\": 1 } \n[\n[],\n");
}

void show_usage() {
    printf("I3 status line, © 2020 spambaconspam@protonmail.com, released under GPL3\n");
    printf("usage: statusline [-t seconds] [-h]\n");
}

void parse_args(int* argc, char** argv) {
    char c;
    bool do_exit = false;

    if (*argc < 2) {
        printf("ERROR: no arguments given\n\n");
        do_exit = true;
    }

    while ((c=getopt(*argc, argv, "ht:")) != -1) {
        switch(c) {
            case 't':
                if ((timeout=atoi(optarg)) == 0) {
                    printf("ERROR: invalid argument\n");
                    do_exit = true;
                }
                break;

            case 'h':
                do_exit = true;
                break;

            // for options not in optstring, and missing required arguments
            case '?':       
                do_exit = true;
                break;

            default:
                do_exit = true;
                printf("setting default\n");
                break;

                
        }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for (; optind < *argc; optind++){      
        printf("ERROR: unknown argument: %s\n", argv[optind]);  
        do_exit = true;
    } 

    if (do_exit) {
        show_usage();
        exit(0);
    }
}

int main(int argc, char **argv) {
    parse_args(&argc, argv);
    print_header();
        
    // configure the separator that can be used inbetween blocks
    block_t separator;;
    strcpy(separator.color1, colors.gray);
    strcpy(separator.text, SEPARATOR_STR);
    separator.separator = false;
    separator.get();

    // create block objects
    uint8_t sites_len = sizeof(sites)/sizeof(sites[0]);
    block_site blocks_sites[sites_len];

    for (uint8_t i=0 ; i<sites_len ; i++) {
        blocks_sites[i].set_site(sites[i]);

        // don't have whitespace between the sites indicators (except for the last one)
        if (i == sites_len-1)
            blocks_sites[i].separator = true;
        else
            blocks_sites[i].separator = false;
    }

    block_datetime datetime;
    datetime.type = TEXT;
    datetime.set_color1(datetime_color1);

    block_wireless wireless;
    wireless.type    = STR_GRAPH;
    wireless.set_color2(wireless_color2);

    block_volume volume;
    volume.type    = GRAPH;
    volume.set_color1(volume_color1);
    volume.set_color2(volume_color2);

    block_battery battery;
    battery.type    = GRAPH;
    battery.set_color2(battery_color2);

    while (1) {
        volume.get_data();

        wireless.get_data();
        wireless.set_color1((wireless.strgraph_value >= WIRELESS_STRENGTH_TRESHOLD) ? wireless_color1_good : wireless_color1_bad);

        datetime.get_data();
        battery.get_data();
        battery.set_color1((atoi(battery.text) >= BATTERY_TRESHOLD) ? battery_color1_normal : battery_color1_critical);

        // keep track of changed state (if data has changed since last check)
        bool changed = false;

        for (block_site& site: blocks_sites) {
            site.get_data();
            if (site.get())
                changed = true;
        }

        if (battery.get())          changed = true;
        if (volume.get())           changed = true;
        if (wireless.get())         changed = true;
        if (datetime.get())         changed = true;

        // print the json to stdout
        if (changed) {
            printf("[\n");

            for (uint8_t i=0 ; i<sites_len ; i++) {
                printf("%s,\n", blocks_sites[i].fmt_text);
                if (i != sites_len-1)
                    printf("%s,\n", separator.fmt_text);
            }

            printf("%s,\n", battery.fmt_text);
            printf("%s,\n", volume.fmt_text);
            printf("%s,\n", wireless.fmt_text);
            printf("%s\n",  datetime.fmt_text);

            printf("],\n");

            fflush(stdout);     // flush buffer
        }

        sleep(timeout);
    }

    return 0;
}
