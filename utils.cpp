#include "utils.h"


// return true when seconds have elapsed, reset timer
bool t_delta_t::has_elapsed(uint16_t seconds) {
    uint32_t t_cur = time(NULL);

    if (first_run) {
        t_prev = t_cur;
        first_run = false;
        return true;
    }

    if ((t_cur - t_prev) > seconds) {
        t_prev = t_cur;
        return true;
    }
    return false;
}
