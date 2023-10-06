#include "mpd.h"
#include "../config.h"
#include <mpd/player.h>
#include <mpd/song.h>
#include <mpd/status.h>


int mpd_toggle_play()
{
	struct mpd_connection *conn = mpd_connection_new(NULL, 0, 30000);
    if (conn == NULL)
        return -1;

    mpd_run_toggle_pause(conn);
    mpd_connection_free(conn);
    return 0;
}

int mpd_seek(int n)
{
	struct mpd_connection *conn = mpd_connection_new(NULL, 0, 30000);
    if (conn == NULL)
        return -1;

    mpd_send_seek_current(conn, n, true);
    mpd_connection_free(conn);
    return 0;
}

int set_mpd(struct Block *block, struct BlockClickEvent *ev)
{
    /* If something changed, func should return 1 */
    if (ev->mod & BLOCK_LMB_PRESSED)
        mpd_toggle_play();
    else if (ev->mod & BLOCK_MOUSE_SCROLL_UP)
        mpd_seek(10);
    else if (ev->mod & BLOCK_MOUSE_SCROLL_DOWN)
        mpd_seek(-10);
    else
        return 0;

    return 1;
}

struct mpd_connection* mpd_connect(const char *host, unsigned port, unsigned int timeout_ms)
{
    struct mpd_connection *conn = mpd_connection_new(host, port, timeout_ms);

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        return NULL;

    return conn;
}

bool get_mpd(struct Block *block)
{
	struct mpd_status *status;
    struct mpd_song   *song;
	const char *artist;
	const char *title;
	const char *track;
    char col[10];
    mpd_pure unsigned length;
    mpd_pure unsigned pos;
    unsigned int pos_perc;

    if (!block_is_elapsed(block))
        return false;

    struct mpd_connection *conn = mpd_connection_new(NULL, 0, 30000);
    if (conn == NULL)
        goto on_conn_err;

    mpd_command_list_begin(conn, true);
    mpd_send_status(conn);
    mpd_send_current_song(conn);
    mpd_command_list_end(conn);

    status = mpd_recv_status(conn);
    if (status == NULL)
        goto on_conn_err;

    if ( mpd_status_get_state(status) < MPD_STATE_PLAY) {
        block_set_text(block, "mpd", "STOPPED", CS_NORMAL, true);
        mpd_connection_free(conn);
        return block_is_changed(block);
    }

    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        goto on_conn_err;

    mpd_response_next(conn);

    while ((song = mpd_recv_song(conn)) != NULL) {
        artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        track = mpd_song_get_tag(song, MPD_TAG_TRACK, 0);
        length = mpd_song_get_duration(song);
        pos = mpd_status_get_elapsed_ms(status) / 1000;
        pos_perc = ((float)pos/(float)length) * 100;

        strcpy(col, (mpd_status_get_state(status) == MPD_STATE_PLAY) ? CS_WARNING : CS_OK);

        char buf[256] = {'\0'};

        if (artist == NULL && track == NULL)
            snprintf(buf, block->maxlen, "%s", title);
        else
            snprintf(buf, block->maxlen, "%s - [%s] %s", artist, track, title);

        block_reset(block);
        block_add_text(block, "mpd", "MPD", CS_WARNING, false);
        block_add_text(block, "mpd", ":", CS_NORMAL, false);
        block_set_strgraph(block, "mpd", buf, pos_perc, col);
        block_add_text(block, "", "", CS_NORMAL, true);

        mpd_song_free(song);
    }

    mpd_status_free(status);
    mpd_connection_free(conn);

    //strcat(block->text, block->sep_chr);
    return block_is_changed(block);

on_conn_err:
    mpd_status_free(status);
    mpd_connection_free(conn);
    block_set_error(block, "MPD CONNECTION ERROR");
    return block_is_changed(block);

}
