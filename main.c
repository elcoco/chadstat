#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // bool

#include <X11/Xlib.h>

#include "blocks.h"
#include "config.h"


static void xsetroot(const char *name);
void die(char* msg);
void parse_args(int*, char** argv);
void usage();
int main();

uint8_t timeout = DEAULT_TIMEOUT;

void die(char* msg) {
    printf("%s", msg);
    exit(0);
}

void usage() {
    printf("dface [OPTIONS]\n");
    exit(0);
}

void parse_args(int* argc, char** argv) {
    char c;
    bool do_exit = false;

    while ((c=getopt(*argc, argv, "ht:")) != -1) {
        switch(c) {
            case 't':
                if ((timeout=atoi(optarg)) == 0) {
                    die("ERROR: invalid argument\n");
                }
                break;

            case 'h':
                usage();

            // for options not in optstring, and missing required arguments
            case '?':       
                usage();

            default:
                printf("setting default\n");
                break;

                
        }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for (; optind < *argc; optind++){      
        char msg[100] = {'\0'};
        sprintf(msg, "ERROR: unknown argument: %s\n", argv[optind]);
        die(msg);  
    } 
}


static void xsetroot(const char *name){
        Display *dpy;

        if (( dpy = XOpenDisplay(0x0)) == NULL )
            die("Can't open display!\n");

        XStoreName(dpy, DefaultRootWindow(dpy), name);
        XSync(dpy, 0);
        //XFlush(dpy);

        XCloseDisplay(dpy);
}

int main(int argc, char **argv) {
    parse_args(&argc, argv);

    block_t datetime;
    block_t volume;
    block_t battery;
    block_t sites;
    block_t wireless;

    datetime.timeout = DATETIME_TIMEOUT;
    volume.timeout   = VOLUME_TIMEOUT;
    battery.timeout  = BATTERY_TIMEOUT;
    sites.timeout    = HTTP_TIMEOUT;
    wireless.timeout = WIRELESS_TIMEOUT;

    // reset
    xsetroot("");


    while (1) {
        bool is_changed = false;

        if (get_datetime(&datetime))    is_changed = true;
        if (get_volume(&volume))        is_changed = true;
        if (get_battery(&battery))      is_changed = true;
        if (get_sites(&sites))          is_changed = true;
        if (get_wireless(&wireless))    is_changed = true;

        if (is_changed) {
            char status[MAXSTRING+1] = {'\0'};

            int16_t r = snprintf(status, MAXSTRING, "%s  %s  %s  %s  %s      ", sites.text,
                                                                          battery.text,
                                                                          volume.text,
                                                                          wireless.text,
                                                                          datetime.text);
            if (r == -1)
                xsetroot("SNPRINTF ENCODING ERROR");
            else if ( r >= MAXSTRING)
                xsetroot("STATUS EXCEEDS MAXLENGTH");
            else
                xsetroot(status);

            printf("%s\n", status);
                
        }

        sleep(timeout);
    }
    return 0;
}















