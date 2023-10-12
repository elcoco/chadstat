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


        block_add_text(block, mp->namespace, "MPRIS", block->cs->label);
        if (mp->properties->status == MPRIS_STATUS_PLAYING) {
            //block_add_text(block, mp->namespace, MPRIS_CHR_PLAY, block->cs->active, false);
            block_add_text(block, mp->namespace, ":", block->cs->separator_block);
            block_set_strgraph(block, mp->namespace, buf, pos_perc, block->cs->active, block->cs->graph_right);
        }
        else {
            //block_add_text(block, mp->namespace, MPRIS_CHR_PAUSE, CS_WARNING, false);
            block_add_text(block, mp->namespace, ":", block->cs->separator_block);
            block_set_strgraph(block, mp->namespace, buf, pos_perc, block->cs->inactive, block->cs->graph_right);
        }
        //else
        //else
        //    block_add_text(block, mp->namespace, MPRIS_CHR_PAUSE, CS_NORMAL, false);

        if (mp->next != NULL)
            block_add_text(block, mp->namespace, " ", block->cs->separator_block);

        mp = mp->next;
    }

cleanup:
    if (mp_head != NULL)
        mpris_player_destroy_all(mp_head);

    return block_is_changed(block);
}
