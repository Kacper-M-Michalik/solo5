#include "bindings.h"

#if defined(__x86_64__)
#define READ_CPU_TICKS cpu_rdtsc
#elif defined(__aarch64__)
#define READ_CPU_TICKS cpu_cntvct
#else
#error Unsupported architecture
#endif

void time_init(void)
{
    //Might have kernel create client IDs, means we dont have to deal with race conditions
}

solo5_time_t solo5_clock_monotonic(void)
{
    return (solo5_time_t)READ_CPU_TICKS();
}

solo5_time_t solo5_clock_wall(void)
{
    //timer server call
}

void solo5_yield(solo5_time_t deadline, solo5_handle_set_t *ready_set)
{
    solo5_handle_set_t tmp_ready_set = 0;        
    do 
    {
        /* Add when we implement net
        for (solo5_handle_t i = 0U; i < MFT_MAX_ENTRIES; ++i) {
            if (muen_net_pending_data(i))
                tmp_ready_set |= 1UL << i;
        }

        if (tmp_ready_set > 0)
            break;
        */

        cpu_pause();
    } 
    while (solo5_clock_monotonic() < deadline);

    if (ready_set) *ready_set = tmp_ready_set;
}