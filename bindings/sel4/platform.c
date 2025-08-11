#include "bindings.h"

static const char *cmdline;
static uint64_t mem_size;

const struct mft *device_manifest = NULL;
extern const struct mft1_note __solo5_mft1_note;

void platform_init(const void *arg)
{
    const struct sel4_boot_info *bi = arg;

    cmdline = bi->cmdline;
    mem_size = bi->mem_size;

    /* Do we actually want to immediately close if we can't locate devices? */    
    // Need to check how validation actually occurs
    
    const struct mft *mft;
    size_t mft_size;
    mft_get_builtin_mft1(&__solo5_mft1_note, &mft, &mft_size);
    if (mft_validate(mft, mft_size) != 0) {
        log(ERROR, "Solo5: Built-in manifest validation failed. Aborting.\n");
        solo5_abort();
    }
    device_manifest = mft;
}

void platform_exit(int status, void *cookie __attribute__((unused)))
{
    const char msg[] = "Solo5: Halted\n";
    platform_puts(msg, strlen(msg));

    //Need to cause fault here -> use a notification?
}

const char *platform_cmdline(void)
{
    return cmdline;
}

uint64_t platform_mem_size(void)
{
    return mem_size;
}

int platform_puts(const char *buf, int n)
{
    //Add sel4 serial server call here
}

int platform_set_tls_base(uint64_t base)
{
    //Use seL4_TCB_SetTLSBase here
}
