#include "mpd.h"
#include "../config.h"

int set_mpd(struct Block *block)
{
    printf("disko\n");
    return 1;
}

bool get_mpd(struct Block *block)
{
	struct mpd_connection *conn;
	struct mpd_status *status;
    struct mpd_song   *song;
	const char *artist;
	const char *title;
	const char *track;
    char col[10];

    if (!block_is_elapsed(block))
        return false;

    conn = mpd_connection_new(NULL, 0, 30000);

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        goto on_conn_err;

    mpd_command_list_begin(conn, true);
    mpd_send_status(conn);
    mpd_send_current_song(conn);
    mpd_command_list_end(conn);

    status = mpd_recv_status(conn);
    if (status == NULL)
        goto on_conn_err;

    if ( mpd_status_get_state(status) < MPD_STATE_PLAY) {
        block_set_text(block, "STOPPED", CS_NORMAL, true);
        mpd_connection_free(conn);
        return block_is_changed(block);
    }

    mpd_status_free(status);

    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        goto on_conn_err;

    mpd_response_next(conn);

    while ((song = mpd_recv_song(conn)) != NULL) {
        artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        track = mpd_song_get_tag(song, MPD_TAG_TRACK, 0);
        strcpy(col, (mpd_status_get_state(status) == MPD_STATE_PLAY) ? CS_OK : CS_NORMAL);

        char buf[256] = {'\0'};

        if (artist == NULL && track == NULL)
            snprintf(buf, block->maxlen, "%s", title);
        else
            snprintf(buf, block->maxlen, "%s - [%s] %s", artist, track, title);

        block_set_text(block, buf, col, true);

        mpd_song_free(song);
    }
    mpd_connection_free(conn);

    //strcat(block->text, block->sep_chr);
    return block_is_changed(block);

on_conn_err:
    mpd_connection_free(conn);
    block_set_error(block, "MPD CONNECTION ERROR");
    return block_is_changed(block);

}
