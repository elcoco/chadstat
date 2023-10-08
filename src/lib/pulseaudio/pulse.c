#include "pulse.h"

pa_mainloop_api *mainloop_api = NULL;
pa_mainloop *mainloop;
pa_context *context;

static void exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata);
static void quit(int ret);
static void set_rel_sink_volume_callback(pa_context *c, const pa_sink_info *i, int is_last, void *userdata);
static void get_default_sink_callback(pa_context *c, const pa_server_info *i, void *userdata);
static void context_state_callback(pa_context *c, void *userdata);
static void pa_cleanup();
static int pa_run();

static void exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e, int sig, void *userdata)
{
    printf("Got SIGINT, exiting.\n");
    m->quit(m, 0);
}

static void quit(int ret)
{
    /* call from callbacks to quit mainloop */
    printf("quit\n");
    assert(mainloop_api);
    mainloop_api->quit(mainloop_api, ret);
}

static void set_rel_sink_volume_callback(pa_context *c, const pa_sink_info *i, int is_last, void *userdata)
{
    /* Callback is ran by context_state_callback() when context is ready
     */
    printf("setting sink volume\n");

    struct PAAction *action = userdata;
    pa_cvolume cv;
    pa_operation *o;

    if (is_last < 0) {
        printf("Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        quit(1);
        return;
    }

    if (is_last)
        return;

    assert(i);

    cv = i->volume;

    for (int c = 0; c < cv.channels; c++) {
        double vol_prev = cv.values[c] / ((double) PA_VOLUME_NORM / 100);
        double vol_new =  (action->value+vol_prev) * (double) PA_VOLUME_NORM / 100;
        cv.values[c] = (pa_volume_t)vol_new;
        printf("Setting chan %d => %f%% %f\n", c, vol_prev+action->value, vol_new);
    }

    o = pa_context_set_sink_volume_by_name(c, action->sink_name, &cv, NULL, NULL);

    if (o)
        pa_operation_unref(o);

    printf("done setting sink volume\n");
    quit(0);
}

static void get_default_sink_callback(pa_context *c, const pa_server_info *i, void *userdata)
{
    struct PAAction *action = userdata;
    strcpy(action->sink_name, i->default_sink_name);
    printf("default sink: %s\n", i->default_sink_name);
    quit(0);
}

static void set_sink_mute_callback(pa_context *c, const pa_sink_info *i, int is_last, void *userdata)
{
    /* Callback is ran by context_state_callback() when context is ready
     */
    printf("setting sink mute\n");
    struct PAAction *action = userdata;

    if (is_last < 0) {
        printf("Failed to get sink information: %s", pa_strerror(pa_context_errno(c)));
        quit(1);
        return;
    }

    if (is_last)
        return;

    assert(i);

    switch (action->action) {
        case PA_ACTION_SET_MUTED:
            pa_operation_unref(pa_context_set_sink_mute_by_name(c, action->sink_name, 1, NULL, NULL));
            break;
        case PA_ACTION_SET_UNMUTED:
            pa_operation_unref(pa_context_set_sink_mute_by_name(c, action->sink_name, 0, NULL, NULL));
            break;
        case PA_ACTION_TOGGLE_MUTED:
            pa_operation_unref(pa_context_set_sink_mute_by_name(c, action->sink_name, !i->mute, NULL, NULL));
            break;
    }

    printf("done setting sink volume\n");
    quit(0);
}

static void context_state_callback(pa_context *c, void *userdata)
{
    /* Do operation when context is ready */

    struct PAAction *action = userdata;
    pa_operation* o = NULL;
    printf("Start op\n");

    if (action->action == PA_ACTION_SET_SINK_INFO)
        //o = pa_context_get_sink_info_list(c, action->cb, action);
        o = pa_context_get_server_info(c, get_default_sink_callback, action);
    else
        o = pa_context_get_sink_info_by_name(c, action->sink_name, action->cb, action);

    if (o)
        pa_operation_unref(o);
    printf("End op\n");
}

static void pa_cleanup()
{
    if (context) {
        pa_context_disconnect(context);
        pa_context_unref(context);
    }

    if (mainloop) {
        pa_signal_done();
        pa_mainloop_free(mainloop);
    }
}

static int pa_init()
{
    // Get a mainloop and its context
    if ((mainloop = pa_mainloop_new()) == NULL)
        goto cleanup;
    if ((mainloop_api = pa_mainloop_get_api(mainloop)) == NULL)
        goto cleanup;
    if ((context = pa_context_new(mainloop_api, "pcm-playback")) == NULL)
        goto cleanup;

    pa_signal_new(SIGINT, exit_signal_callback, NULL);
    pa_signal_new(SIGTERM, exit_signal_callback, NULL);

    return 0;

cleanup:
    pa_cleanup();
    return -1;
}

static int pa_run()
{
    int ret;
    // Start the mainloop and connect to server
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) != 0)
        return -1;

    if (pa_mainloop_run(mainloop, &ret) < 0) {
        printf("pa_mainloop_run() failed.");
        return -1;
    }

    pa_cleanup();
    return 0;
}

int get_active_sink(struct PAAction *action)
{
    action->action = PA_ACTION_SET_SINK_INFO;

    if (pa_init() < 0) {
        pa_cleanup();
        return -1;
    }
    pa_context_set_state_callback(context, &context_state_callback, action);
    pa_run();
    return 0;
}

int pa_adjust_volume(double vol)
{
    //action = PA_ACTION_INCR;
    //vol_incr = vol;
    struct PAAction action;

    get_active_sink(&action);
    if (strlen(action.sink_name) == 0)
        return -1;

    action.action = PA_ACTION_INCR;
    action.value = vol;
    action.cb = &set_rel_sink_volume_callback;

    if (pa_init() < 0) {
        pa_cleanup();
        return -1;
    }

     // Set a callback so we can wait for the context to be ready
    pa_context_set_state_callback(context, context_state_callback, &action);

    pa_run();
    return 0;
}

int pa_mute()
{
    struct PAAction action;

    get_active_sink(&action);
    if (strlen(action.sink_name) == 0)
        return -1;

    action.action = PA_ACTION_SET_MUTED;
    action.cb = &set_sink_mute_callback;

    if (pa_init() < 0) {
        pa_cleanup();
        return -1;
    }

     // Set a callback so we can wait for the context to be ready
    pa_context_set_state_callback(context, context_state_callback, &action);

    pa_run();
    return 0;
}

int pa_unmute()
{
    struct PAAction action;

    get_active_sink(&action);
    if (strlen(action.sink_name) == 0)
        return -1;

    action.action = PA_ACTION_SET_UNMUTED;
    action.cb = &set_sink_mute_callback;

    if (pa_init() < 0) {
        pa_cleanup();
        return -1;
    }

     // Set a callback so we can wait for the context to be ready
    pa_context_set_state_callback(context, context_state_callback, &action);

    pa_run();
    return 0;
}

int pa_toggle_mute()
{
    struct PAAction action;

    get_active_sink(&action);
    if (strlen(action.sink_name) == 0)
        return -1;

    action.action = PA_ACTION_TOGGLE_MUTED;
    action.cb = &set_sink_mute_callback;

    if (pa_init() < 0) {
        pa_cleanup();
        return -1;
    }

     // Set a callback so we can wait for the context to be ready
    pa_context_set_state_callback(context, context_state_callback, &action);

    pa_run();
    return 0;
}
