#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // bool

#include <X11/Xlib.h>

#include "blocks.h"



void die(char* msg);
void parse_args(int*, char** argv);
void usage();
int main();

uint8_t timeout = 3;

// x11
Display* dpy;
int screen;
Window root;


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

void init_x11() {
    dpy = XOpenDisplay(NULL);

    if (dpy == NULL) 
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);
}

void close_x11() {
	XCloseDisplay(dpy);	
}

int main(int argc, char **argv) {
    parse_args(&argc, argv);

    block_t datetime;
    block_t volume;
    block_t battery;

    datetime.timeout = 10;
    volume.timeout = 1;
    battery.timeout = 1;

    // return changed
    //

    while (1) {
        bool is_changed = false;

        if (get_datetime(&datetime)) is_changed = true;
        if (get_volume(&volume))     is_changed = true;
        if (get_battery(&battery))     is_changed = true;

        if (is_changed) {
            char status[500] = {'\0'};

            strcat(status, battery.text);
            strcat(status, "  ");
            strcat(status, volume.text);
            strcat(status, "  ");
            strcat(status, datetime.text);

            init_x11();
            XStoreName(dpy, root, status);
            close_x11();

            printf(">>> changed %s\n", status);
        }



        sleep(timeout);
    }
    return 0;
}















