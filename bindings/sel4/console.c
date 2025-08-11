#include "bindings.h"

void console_init(void)
{
    //Might have kernel create client IDs, means we dont have to deal with race conditions
}

void solo5_console_write(const char *buf, size_t size)
{
    (void)platform_puts(buf, size);
}