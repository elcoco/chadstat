#include <X11/Xlib.h>

#include "blocks.h"


static void xsetroot(const char *name);
static void die(char* msg);
static void parse_args(int*, char** argv);
static uint8_t get_timeout();
static void usage();

static void die(char *msg) {
    printf("%s", msg);
    exit(0);
}

static void usage() {
    printf("dface, a statusline for DWM, © 2020 spambaconspam@protonmail.com, released under GPL3\n");
    printf("usage: dface [-t seconds] [-h]\n");
    exit(0);
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

static uint8_t get_timeout() {
    /* get the smallest timeout value defined in the blocks array */
    uint8_t timeout = 60;
    uint8_t l = sizeof(block_arr)/sizeof(block_arr[0]);

    for (uint8_t i=0 ; i<l ; i++) {
        block_t block = block_arr[i];
        if (block.timeout < timeout)
            timeout = block.timeout;
    }
    return timeout;
}

static void parse_args(int *argc, char **argv) {
    char c;

    while ((c=getopt(*argc, argv, "h:")) != -1) {
        switch(c) {
            case 'h':
                usage();    // exits

            case '?':       // for options not in optstring, and missing required arguments
                usage();    // exits

            default:
                printf("setting default\n");
                break;
        }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for (; optind < *argc; optind++){      
        char msg[50] = {'\0'};
        sprintf(msg, "ERROR: unknown argument: %s\n", argv[optind]);
        die(msg);  
    } 
}

int main(int argc, char **argv) {
    parse_args(&argc, argv);

    // reset
    xsetroot("");

    while (1) {
        bool is_changed = false;
        uint8_t l = sizeof(block_arr)/sizeof(block_arr[0]);

        // update all the statusses and check if they've changed
        for (uint8_t i=0 ; i<l ; i++) {
            if (block_arr[i].get(&block_arr[i]))
                is_changed = true;
        }

        if (is_changed) {
            char status[MAXSTRING+1] = {'\0'};

            for (uint8_t i=0 ; i<l ; i++) {
                block_t block = block_arr[i];

                strcat(status, block.text);
                strcat(status, block.sep_chr);

            }
            strcat(status, "         ");

            xsetroot(status);
            printf("%s\n", status);
        }

        sleep(get_timeout());
    }
    return 0;
}
