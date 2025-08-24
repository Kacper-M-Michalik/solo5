#include "bindings.h"

void solo5_yield(solo5_time_t deadline, solo5_handle_set_t *ready_set)
{
    struct hvt_hc_poll t;
    uint64_t now;

    now = solo5_clock_monotonic();
    if (deadline <= now)
        t.timeout_nsecs = 0;
    else
        t.timeout_nsecs = deadline - now;
    sel4_do_hypercall(HVT_HYPERCALL_POLL, &t);
    if (ready_set != NULL)
        *ready_set = t.ready_set;
}
