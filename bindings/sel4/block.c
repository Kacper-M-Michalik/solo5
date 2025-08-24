#include "bindings.h"

static const struct mft *mft;

solo5_result_t solo5_block_write(solo5_handle_t handle, solo5_off_t offset,
        const uint8_t *buf, size_t size)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_BLOCK_BASIC);
    if (e == NULL)
        return SOLO5_R_EINVAL;
    if (offset & (e->u.block_basic.block_size - 1))
        return SOLO5_R_EINVAL;
    /*
     * Checks for writes beyond capacity are enforced by the tender in the
     * hypercall handler.
     */
    /*
     * TODO: This artificially enforces the current limit of allowing only
     * single-block operations. Will be removed once all targets can handle
     * reads/writes of >1 block.
     */
    if (size != e->u.block_basic.block_size)
        return SOLO5_R_EINVAL;

    volatile struct hvt_hc_block_write wr;
    wr.handle = handle;
    wr.offset = offset;
    wr.data = buf;
    wr.len = size;
    wr.ret = 0;

    sel4_do_hypercall(HVT_HYPERCALL_BLOCK_WRITE, &wr);

    return wr.ret;
}

solo5_result_t solo5_block_read(solo5_handle_t handle, solo5_off_t offset,
        uint8_t *buf, size_t size)
{
    const struct mft_entry *e =
        mft_get_by_index(mft, handle, MFT_DEV_BLOCK_BASIC);
    if (e == NULL)
        return SOLO5_R_EINVAL;
    if (offset & (e->u.block_basic.block_size - 1))
        return SOLO5_R_EINVAL;
    /*
     * Checks for reads beyond capacity are enforced by the tender in the
     * hypercall handler.
     */
    /*
     * TODO: This artificially enforces the current limit of allowing only
     * single-block operations. Will be removed once all targets can handle
     * reads/writes of >1 block.
     */
    if (size != e->u.block_basic.block_size)
        return SOLO5_R_EINVAL;

    volatile struct hvt_hc_block_read rd;
    rd.handle = handle;
    rd.offset = offset;
    rd.data = buf;
    rd.len = size;
    rd.ret = 0;

    sel4_do_hypercall(HVT_HYPERCALL_BLOCK_READ, &rd);

    return rd.ret;
}

solo5_result_t solo5_block_acquire(const char *name, solo5_handle_t *handle,
        struct solo5_block_info *info)
{
    unsigned index;
    const struct mft_entry *e =
        mft_get_by_name(mft, name, MFT_DEV_BLOCK_BASIC, &index);
    if (e == NULL)
        return SOLO5_R_EINVAL;
    assert(e->attached);

    *handle = index;
    info->capacity = e->u.block_basic.capacity;
    info->block_size = e->u.block_basic.block_size;
    return SOLO5_R_OK;
}

void block_init(const struct hvt_boot_info *bi)
{
    mft = bi->mft;
}
