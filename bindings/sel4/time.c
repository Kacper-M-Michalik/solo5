#include "bindings.h"

void time_init(const struct hvt_boot_info *bi)
{
    assert(tscclock_init(bi->cpu_cycle_freq) == 0);
}

/* return time in nsecs */
uint64_t solo5_clock_monotonic(void)
{
    return tscclock_monotonic();
}

/* return wall time in nsecs */
uint64_t solo5_clock_wall(void)
{
    struct hvt_hc_walltime t;
    sel4_do_hypercall(HVT_HYPERCALL_WALLTIME, &t);
    return t.nsecs;
}
