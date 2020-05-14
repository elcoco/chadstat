#include <X11/Xlib.h>

#include "blocks.h"
#include "config.h"


static void xsetroot(const char *name);
static void die(char* msg);
static void parse_args(int*, char** argv);
static void usage();

uint8_t timeout = DEAULT_TIMEOUT;

static void die(char *msg) {
    printf("%s", msg);
    exit(0);
}

static void usage() {
    printf("dface [OPTIONS]\n");
    exit(0);
}

static void parse_args(int *argc, char **argv) {
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

        sleep(timeout);
    }
    return 0;
}
