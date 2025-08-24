#include "bindings.h"

void platform_init(const void *arg)
{
    process_bootinfo(arg);
}

void platform_exit(int status, void *cookie)
{
    struct hvt_hc_halt h;

    h.exit_status = status;
    h.cookie = cookie;

    sel4_do_hypercall(HVT_HYPERCALL_HALT, &h);
    for(;;);
}
