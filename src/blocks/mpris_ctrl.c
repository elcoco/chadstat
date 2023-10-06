#include "mpris_ctrl.h"
#include "../config.h"

int set_mpris(struct Block *block, struct BlockClickEvent *ev)
{
    int ret = -1;
    struct MPRISPlayer *mp_head = NULL;

    mp_head = mpris_player_load_all(NULL);
    if (mp_head == NULL) {
        block_set_error(block, "MPRIS DBUS ERROR");
        goto cleanup;

    }
    struct MPRISPlayer *mp = mp_head;

    while (mp != NULL) {
        //printf("namespace: >>%s<<    >>%s<<\n", ev->instance, mp->namespace);
        if (strcmp(mp->namespace, ev->instance) == 0) {

            //printf("Found namespace: %s\n", mp->namespace);

            if (ev->mod & BLOCK_LMB_PRESSED)
                mpris_player_toggle(mp->namespace);
            else if (ev->mod & BLOCK_MOUSE_SCROLL_UP)
                mpris_player_set_position(mp->namespace, mp->metadata->track_id, mp->properties->position+30);
            else if (ev->mod & BLOCK_MOUSE_SCROLL_DOWN)
                mpris_player_set_position(mp->namespace, mp->metadata->track_id, mp->properties->position-30);

            ret = 0;
            break;
        }

        mp = mp->next;
    }


cleanup:
    if (mp_head != NULL)
        mpris_player_destroy_all(mp_head);


    return ret;
}

bool get_mpris(struct Block *block)
{
    struct MPRISPlayer *mp_head = NULL;

    if (!block_is_elapsed(block))
        return false;

    mp_head = mpris_player_load_all(NULL);
    if (mp_head == NULL) {
        block_set_error(block, "MPRIS DBUS ERROR");
        goto cleanup;
    }

    struct MPRISPlayer *mp = mp_head;

    //mpris_player_debug_all(mp_head);

    block_reset(block);

    while (mp != NULL) {
        unsigned int pos = mp->properties->position;
        unsigned int length = mp->metadata->length;
        unsigned int pos_perc = ((float)pos/(float)(length)) * 100;

        char buf[MPRIS_MAX_BUFFER] = "";
        snprintf(buf, block->maxlen, "%s", mp->metadata->title);


        // TODO pass instance, need to change all calls to set_strgraph/set_text etc...
        block_add_text(block, mp->namespace, "MPRIS", CS_WARNING, false);
        block_add_text(block, mp->namespace, ":", CS_NORMAL, false);
        block_set_strgraph(block, mp->namespace, buf, pos_perc, CS_OK);
        block_add_text(block, mp->namespace, " ", CS_WARNING, false);

        mp = mp->next;
    }



    //snprintf(buf, block->maxlen, "%s", "bever");

    //block_set_strgraph(block, buf, pos_perc, CS_OK);

cleanup:
    if (mp_head != NULL)
        mpris_player_destroy_all(mp_head);

    return block_is_changed(block);
}
