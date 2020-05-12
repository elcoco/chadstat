#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> // bool

#include <X11/Xlib.h>

#include "blocks.h"


#define datetime_color  ""
#define wireless_color1 ""
#define wireless_color2 ""

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

    // return changed
    //
    while (1) {
        get_datetime(&datetime, datetime_color);

        if (datetime.is_changed) {
            printf(">>> changed %s\n", datetime.text);

            init_x11();
            XStoreName(dpy, root, datetime.text);
            close_x11();
        }



        sleep(timeout);
    }
    return 0;
}















