#ifndef VOLUME_H
#define VOLUME_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <alsa/asoundlib.h> // alsa master volume info
 
#include "../block.h"

#ifndef SND_CARD_NAME
  #define SND_CARD_NAME    "default"
#endif

bool get_volume(struct Block *block);

#endif
