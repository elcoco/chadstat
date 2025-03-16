#ifndef VOLUME_H
#define VOLUME_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <alsa/asoundlib.h> // alsa master volume info
#include "../lib/pulseaudio/pulse.h"
 
#include "../block.h"

#ifndef SND_CARD_NAME
  #define SND_CARD_NAME    "default"
#endif

bool get_alsa_volume(struct Block *block);
bool get_pulse_volume(struct Block *block);
int set_pulse_volume(struct Block *block, struct BlockClickEvent *ev);

bool get_pipewire_volume(struct Block *block);
int set_pipewire_volume(struct Block *block, struct BlockClickEvent *ev);

#endif
