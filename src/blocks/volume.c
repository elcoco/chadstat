#include "volume.h"
#include "../config.h"

bool get_volume(struct Block *block)
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
