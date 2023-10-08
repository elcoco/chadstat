#include "volume.h"
#include "../config.h"


int set_pulse_volume(struct Block *block, struct BlockClickEvent *ev)
{
    if (ev->mod & BLOCK_LMB_PRESSED)
        pa_toggle_mute();
    else if (ev->mod & BLOCK_MOUSE_SCROLL_UP)
        pa_adjust_volume(10);
    else if (ev->mod & BLOCK_MOUSE_SCROLL_DOWN)
        pa_adjust_volume(-10);
    else
        return 0;

    return 1;
}

bool get_alsa_volume(struct Block *block)
{
    long min, max;
    long level;
    uint16_t volume;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char card[] = SND_CARD_NAME;
    const char selem_name[] = "Master";

    if (! block_is_elapsed(block))
        return false;

    if (snd_mixer_open(&handle, 0) < 0)
        goto cleanup_on_err;
    if (snd_mixer_attach(handle, card) < 0)
        goto cleanup_on_err;
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0)
        goto cleanup_on_err;
    if (snd_mixer_load(handle) < 0)
        goto cleanup_on_err;

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);

    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);
    if (elem == NULL)
        goto cleanup_on_err;

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &level);

    volume = ((float)level/(float)max)*100.00;
    block_reset(block);
    block_set_graph(block, "volume", block->maxlen, volume, CS_WARNING);
    snd_mixer_close(handle);
    //snd_mixer_selem_id_free(sid);
    return block_is_changed(block);

cleanup_on_err:
    if (handle)
        snd_mixer_close(handle);
    if (sid)
        snd_mixer_selem_id_free(sid);

    block_set_error(block, "VOLUME ERROR");
    return block_is_changed(block);
}

bool get_pulse_volume(struct Block *block)
{
    if (! block_is_elapsed(block))
        return false;

    block_reset(block);

    if (pa_is_muted()) {
        block_add_text(block, "volume", "SND:MUTED", CS_WARNING, 1);
        return block_is_changed(block);
    }

    //block_add_text(block, "volume", "SND", CS_WARNING, 0);
    //block_add_text(block, "volume", ":", CS_NORMAL, 0);

    char text[256] = "SND";
    for (int i=0 ; i<block->maxlen ; i++) {
        strcat(text, "|");
    }

    int vol_perc = pa_get_volume();
    block_set_strgraph(block, "volume", text, vol_perc, CS_WARNING);
    block_add_text(block, "volume", "", CS_WARNING, 1);
    return block_is_changed(block);
}
