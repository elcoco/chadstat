#pragma once

#include <stdint.h>
#include <time.h>           // datetime info


// check time differences
class t_delta_t {
    private:
    uint32_t t_prev     = time(NULL);
    bool     first_run  = true;

    public:
    bool has_elapsed(uint16_t seconds);
};
