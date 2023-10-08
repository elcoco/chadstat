#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/volume.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <pulse/pulseaudio.h>

// read: https://freedesktop.org/software/pulseaudio/doxygen/async.html

#define PA_VOLUME_NORM ((pa_volume_t) 0x10000U)

enum PAActionFlags {
    PA_ACTION_INCR,
    PA_ACTION_SET_MUTED,
    PA_ACTION_SET_UNMUTED,
    PA_ACTION_TOGGLE_MUTED,
    PA_ACTION_SET_SINK_INFO
};

#define MAX_SINK_BUF 256

// action is passed to callbacks in mainloop so we know what to do
struct PAAction {
    enum PAActionFlags action;
    void(*cb)(pa_context *c, const pa_sink_info *i, int is_last, void *userdata);
    double value;
    char sink_name[MAX_SINK_BUF];
};

int get_active_sink(struct PAAction *action);
int pa_adjust_volume(double vol);
int pa_mute();
int pa_unmute();
int pa_toggle_mute();


#endif
