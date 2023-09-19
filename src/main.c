#include <X11/Xlib.h>

#include "block.h"
#include "config.h"
#include "lib/json/json.h"

// TODO make block text dynamic

//#define I3_HEADER "{ \"version\": 1 } \n[\n[],\n"
#define I3_HEADER "{ \"version\":1,\"click_events\":true} \n[\n[],\n"
#define I3_BLOCKS_START  "[\n"
#define I3_BLOCKS_END    "\n],\n"

#define I3_MAX_CLICK_EVENT_BUFSIZE 256

static void die(char* msg);
static void parse_args(int*, char** argv);
static uint8_t get_timeout();
static void usage();


static void die(char *msg)
{
    printf("%s", msg);
    exit(0);
}

static void usage()
{
    printf("chadstat, a statusline for I3, © 2020 spambaconspam@protonmail.com, released under GPL3\n");
    printf("usage: chadstat [-t seconds] [-h]\n");
    exit(0);
}

static uint8_t get_timeout()
{
    /* get the smallest timeout value defined in the blocks array */
    uint8_t timeout = 60;
    uint8_t blen = sizeof(blocks)/sizeof(blocks[0]);
    uint8_t i;

    for (i=0 ; i<blen ; i++) {
        struct Block block = blocks[i];
        if (block.timeout < timeout)
            timeout = block.timeout;
    }
    return timeout;
}

static void parse_args(int *argc, char **argv)
{
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

int listen_for_input(int sec, char *buf, size_t maxlen)
{
    /* Listen for input on STDIN.
     * Block for max SEC seconds.
     * Return after reading \n
     */
    // timeout structure passed into select
    struct timeval tv;
    // fd_set passed into select
    fd_set fds;
    // Set up the timeout.  here we can wait for 1 second
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    int len = 0;

    // Zero out the fd_set - make sure it's pristine
    FD_ZERO(&fds);
    // Set the FD that we want to read
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    // select takes the last file descriptor value + 1 in the fdset to check,
    // the fdset for reads, writes, and errors.  We are only passing in reads.
    // the last parameter is the timeout.  select will return if an FD is ready or 
    // the timeout has occurred
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    // return 0 if STDIN is not ready to be read.
    //
    while (FD_ISSET(STDIN_FILENO, &fds)) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {

            // Reached max len
            // Probably error because json was longer than string could contain, oh well...
            if (len+2 >= maxlen)
                return 0;

            if (ch == '\n') {
                buf[len+1] = '\0';
                return 1;
            }

            buf[len] = ch;
            len++;

        }
    }
    return 0;
}

struct Block* get_block_by_name(struct Block *blocks, int length, const char *name)
{
    for (int i=0 ; i<length ; i++) {
        if (strcmp(blocks[i].name, name) == 0)
            return &blocks[i];
    }
    return NULL;
}

int main(int argc, char **argv)
{
    bool is_changed;
    uint8_t blen;
    uint8_t i;
    struct Block *block;
    blen = sizeof(blocks)/sizeof(blocks[0]);

    parse_args(&argc, argv);
    printf(I3_HEADER);

    for (int i=0 ; i<blen ; i++)
        block_init(&blocks[i]);

    while (1) {

        // update all the statusses and check if something has changed
        block = blocks;
        is_changed = false;
        for (i=0 ; i<blen ; i++, block++) {
            if (block->get(block))
                is_changed = true;
        }

        if (is_changed) {
            printf(I3_BLOCKS_START);

            for (i=0 ; i<blen ; i++)
                block_print(&blocks[i], i==blen-1);

            printf(I3_BLOCKS_END);
            fflush(stdout);
        }

        char buf[I3_MAX_CLICK_EVENT_BUFSIZE] = "";
        if (listen_for_input(get_timeout(), buf, I3_MAX_CLICK_EVENT_BUFSIZE)) {

            // skip array char
            if (strlen(buf) < 5)
                continue;

            // read json
            struct JSONObject *rn = json_load(buf);
            if (rn == NULL)
                continue;

            // get name from json
            struct JSONObject *jo = json_get_path(rn, "name");
            if (jo == NULL)
                continue;

            char *name = json_get_string(jo);
            if (name == NULL)
                continue;

            // get block
            struct Block *inp_block = get_block_by_name(blocks, blen, name);
            block_print(inp_block, 0);

            json_print(rn, 0);
            json_object_destroy(rn);
            fflush(stdout);
        }
    }
    return 0;
}
