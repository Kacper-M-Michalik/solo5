#include "bindings.h"

static const struct mft *mft;

void net_init(struct sel4_boot_info *bi)
{
    //mft = bi->mft;
}

solo5_result_t solo5_net_acquire(const char *name, solo5_handle_t *handle,
        struct solo5_net_info *info)
{
    return SOLO5_R_EUNSPEC;
}

solo5_result_t solo5_net_read(solo5_handle_t handle, uint8_t *buf, size_t size,
        size_t *read_size)
{
    return SOLO5_R_EUNSPEC;
}

solo5_result_t solo5_net_write(solo5_handle_t handle, const uint8_t *buf,
        size_t size)
{
    return SOLO5_R_EUNSPEC;
}