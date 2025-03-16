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
    block_set_graph(block, "volume", block->maxlen, volume, block->cs->ok, block->cs->graph_right);
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
        block_add_text(block, "volume", "SND_MUTED", block->cs->disabled);
        return block_is_changed(block);
    }

    char text[256] = "SND";
    for (int i=0 ; i<block->maxlen ; i++) {
        strcat(text, "|");
    }

    int vol_perc = pa_get_volume();
    block_set_strgraph(block, "volume", text, vol_perc, block->cs->graph_left, block->cs->graph_right);
    block_add_text(block, "volume", "", block->cs->separator);
    return block_is_changed(block);
}

bool get_pipewire_volume(struct Block *block)
{
    /* Coulnd't find info on how to use the pipewire headers so taking the shortcut */
    if (! block_is_elapsed(block))
        return false;

    block_reset(block);

    char buf[128];
    FILE *fp = popen("wpctl get-volume @DEFAULT_AUDIO_SINK@", "r");

    if (fp == NULL) {
        block_set_error(block, "VOLUME ERROR");
        return false;
    }

    char *vol;
    char *muted_status;

    fgets(buf, sizeof(buf), fp);
    char *tok;
    char *saveptr;
    tok = strtok_r(buf, " ", &saveptr);
    vol = strtok_r(NULL, " ", &saveptr);
    muted_status = strtok_r(NULL, " ", &saveptr);

    if (! tok) {
        block_set_error(block, "VOLUME ERROR");
        return block_is_changed(block);
    }
    if (muted_status && strncmp(muted_status, "[MUTED]", 7) == 0) {
        block_add_text(block, "volume", "SND_MUTED", block->cs->disabled);
        return block_is_changed(block);
    }

    *(vol + strlen(vol)-1) = '\0';
    float vol_perc = atof(vol) * 100;

    char text[256] = "SND";
    for (int i=0 ; i<block->maxlen ; i++)
        strcat(text, "|");

    pclose(fp);

    block_set_strgraph(block, "volume", text, vol_perc, block->cs->graph_left, block->cs->graph_right);
    block_add_text(block, "volume", "", block->cs->separator);
    return block_is_changed(block);
}

int set_pipewire_volume(struct Block *block, struct BlockClickEvent *ev)
{
    int bufsize = 256;
    char cmd[256];

    if (ev->mod & BLOCK_LMB_PRESSED)
        strncpy(cmd, "wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle", bufsize-1);
    else if (ev->mod & BLOCK_MOUSE_SCROLL_UP)
        strncpy(cmd, "wpctl set-volume @DEFAULT_AUDIO_SINK@ 1%+", bufsize-1);
    else if (ev->mod & BLOCK_MOUSE_SCROLL_DOWN)
        strncpy(cmd, "wpctl set-volume @DEFAULT_AUDIO_SINK@ 1%-", bufsize-1);
    else
        return 0;

    int result = system(cmd);
    return 1;
}
