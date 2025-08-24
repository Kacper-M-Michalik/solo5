#include "bindings.h"

int platform_puts(const char *buf, int n)
{
    struct hvt_hc_puts str;

    str.data = (char *)buf;
    str.len = n;

    sel4_do_hypercall(HVT_HYPERCALL_PUTS, &str);

    return str.len;
}

void solo5_console_write(const char *buf, size_t size)
{
    (void)platform_puts(buf, size);
}

void console_init(void)
{
}