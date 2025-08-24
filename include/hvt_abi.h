/*
 * Copyright (c) 2015-2019 Contributors as noted in the AUTHORS file
 *
 * This file is part of Solo5, a sandboxed execution environment.
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * hvt_abi.h: hvt guest hypercall ABI definitions.
 *
 * This header file must be kept self-contained with no external dependencies
 * other than C99 headers.
 *
 * This header file is dual-use; tender code will define HVT_HOST when
 * including it.
 * 
 * Binding code can define USE_HYPERCALL_INSTRUCTION to use hvc/vmcall based hypercalls instead of MMIO / I/O-port based hypercalls.
 */

#ifndef HVT_ABI_H
#define HVT_ABI_H

#include <stddef.h>
#include <stdint.h>
#include "elf_abi.h"

/*
 * ABI version. This must be incremented before cutting a release of Solo5 if
 * any material changes are made to the interfaces or data structures defined
 * in this file.
 */

#define HVT_ABI_VERSION 3

/*
 * Lowest virtual address at which guests can be loaded.
 */
#define HVT_GUEST_MIN_BASE 0x100000


#if defined(__x86_64__) && defined(USE_HYPERCALL_INSTRUCTION)
/*
 * Bindings only.
 */
#    ifndef HVT_HOST
static inline void hvt_do_hypercall(int n, volatile void *arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif

    /* Store hypercall ID in rax */
    /* Arg pointer in rbx */
    register uint64_t rax __asm__("rax") = n;
    register void* rbx __asm__("rbx") = arg;

    __asm__ __volatile__(
            "mfence\n\t"
            "vmcall\n\t"
            "mfence\n\t"
            : "+r"(rax)
            : "r"(rbx)
            : "rcx", "rdx", "rsi", "rdi", "memory");
}
#    endif

#elif defined(__x86_64__)
/*
 * PIO base address used to dispatch hypercalls.
 */
#define HVT_HYPERCALL_PIO_BASE 0x500

#    ifndef HVT_HOST
/*
 * On x86, 32-bit PIO is used as the hypercall mechanism. This only supports
 * sending 32-bit pointers; raise an assertion if a bigger pointer is used.
 *
 * On x86 the compiler-only memory barrier ("memory" clobber) is sufficient
 * across the hypercall boundary.
 */
static inline void hvt_do_hypercall(int n, volatile void *arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif
    __asm__ __volatile__("outl %0, %1"
            :
            : "a" ((uint32_t)((uint64_t)arg)),
              "d" ((uint16_t)(HVT_HYPERCALL_PIO_BASE + n))
            : "memory");
}
#    endif

#elif defined(__aarch64__) && defined(USE_HYPERCALL_INSTRUCTION)
/*
 * Bindings only.
 */
#    ifndef HVT_HOST
static inline void hvt_do_hypercall(int n, volatile void* arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif

    /* Store hypercall ID in x0, as hypercall immediate is more work to extract than just reading x0 */
    /* Arg pointer in x1 */
    register uint64_t x0 __asm__("x0") = (uint64_t)n;
    register void* x1 __asm__("x1") = arg;

    /* Memory barrier required before and after hypercall so that writes by us are visible to hypervisor, and writes by hypervisor are visible to us */
    __asm__ __volatile__(
            "dsb sy\n\t"
            "hvc #0\n\t"
            "dsb sy\n\t"
            : "+r"(x0)
            : "r"(x1)
            : "x2", "x3", "x4", "x5", "x6", "x7", "memory", "cc");
}
#    endif

#elif defined(__aarch64__)
/*
 * MMIO base address used to dispatch hypercalls.
 * Currently, we have limited the max guest memory size to 4GB, to guarantee
 * the 32-bit pointer used for hypercall is enough.
 *
 * MMIO start from 4GB, this value can be changed by AARCH64_MMIO_BASE.
 */
#define HVT_HYPERCALL_MMIO_BASE    (0x100000000UL)

/*
 * On aarch64, the MMIO address must be 64-bit aligned, because we configured
 * the memory attributes of MMIO space to MT_DEVICE_nGnRnE, which is not allow
 * an unaligned access. We must access this area in 64-bit.
 *
 * So the real hypercall ID will be calculated as:
 *          HVT_HYPERCALL_MMIO_BASE + (ID << 3).
 */
#define HVT_HYPERCALL_ADDRESS(x)   (HVT_HYPERCALL_MMIO_BASE + ((x) << 3))
#define HVT_HYPERCALL_NR(x)        (((x) - HVT_HYPERCALL_MMIO_BASE) >> 3)

#    ifndef HVT_HOST
/*
 * In order to keep consistency with x86_64, we limit this hypercall only
 * to support sending 32-bit pointers; raise an assertion if a bigger
 * pointer is used.
 *
 * On aarch64 the compiler-only memory barrier ("memory" clobber) is
 * sufficient across the hypercall boundary.
 */
static inline void hvt_do_hypercall(int n, volatile void *arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif
    __asm__ __volatile__("str %w0, [%1]"
            :
            : "rZ" ((uint32_t)((uint64_t)arg)),
                "r" ((uint64_t)HVT_HYPERCALL_ADDRESS(n))
            : "memory");
}
#    endif

#else
#    error Unsupported architecture
#endif

/*
 * Guest-provided pointers in all structures shared between the guest and
 * tender MUST be declared with HVT_GUEST_PTR(type), where type is the
 * desired guest-side pointer type. hvt_gpa_t will be used
 * as the tender-side type.
 *
 * This ensures that these pointers are not directly dereferencable on the
 * tender side.
 */
#ifdef HVT_HOST
    /*
    * Non-dereferencable tender-side type representing a guest physical address.
    */
    typedef uint64_t hvt_gpa_t;
#    define HVT_GUEST_PTR(T) hvt_gpa_t
#else
#    define HVT_GUEST_PTR(T) T
#endif

/*
 * Maximum size of guest command line, including the string terminator.
 */
#define HVT_CMDLINE_SIZE 8192

/*
 * A pointer to this structure is passed by the tender as the sole argument to
 * the guest entrypoint.
 */
struct hvt_boot_info {
    uint64_t mem_size;                  /* Memory size in bytes */
    uint64_t kernel_end;                /* Address of end of kernel */
    uint64_t cpu_cycle_freq;            /* CPU cycle counter frequency, Hz */
    HVT_GUEST_PTR(const char *) cmdline;/* Address of command line (C string) */
    HVT_GUEST_PTR(const void *) mft;    /* Address of application manifest */
};

/*
 * Canonical list of hypercalls supported by all tender modules. Actual calls
 * supported at run time depend on module configuration at build time.
 */
enum hvt_hypercall {
    /* HVT_HYPERCALL_RESERVED=0 */
    HVT_HYPERCALL_WALLTIME=1,
    HVT_HYPERCALL_PUTS,
    HVT_HYPERCALL_POLL,
    HVT_HYPERCALL_BLOCK_WRITE,
    HVT_HYPERCALL_BLOCK_READ,
    HVT_HYPERCALL_NET_WRITE,
    HVT_HYPERCALL_NET_READ,
    HVT_HYPERCALL_HALT,
    HVT_HYPERCALL_MAX
};

/*
 * Hypercall definitions follow.
 */

/* HVT_HYPERCALL_WALLTIME */
struct hvt_hc_walltime {
    /* OUT */
    uint64_t nsecs;
};

/* HVT_HYPERCALL_PUTS */
struct hvt_hc_puts {
    /* IN */
    HVT_GUEST_PTR(const char *) data;
    size_t len;
};

/* HVT_HYPERCALL_BLOCK_WRITE */
struct hvt_hc_block_write {
    /* IN */
    uint64_t handle;
    uint64_t offset;
    HVT_GUEST_PTR(const void *) data;
    size_t len;

    /* OUT */
    int ret;
};

/* HVT_HYPERCALL_BLOCK_READ */
struct hvt_hc_block_read {
    /* IN */
    uint64_t handle;
    uint64_t offset;
    HVT_GUEST_PTR(void *) data;

    /* IN/OUT */
    size_t len;

    /* OUT */
    int ret;
};

/* HVT_HYPERCALL_NET_WRITE */
struct hvt_hc_net_write {
    /* IN */
    uint64_t handle;
    HVT_GUEST_PTR(const void *) data;
    size_t len;

    /* OUT */
    int ret;
};

/* HVT_HYPERCALL_NET_READ */
struct hvt_hc_net_read {
    /* IN */
    uint64_t handle;
    HVT_GUEST_PTR(void *) data;

    /* IN/OUT */
    size_t len;

    /* OUT */
    int ret;
};

/* HVT_HYPERCALL_POLL */
struct hvt_hc_poll {
    /* IN */
    uint64_t timeout_nsecs;             /* Relative to time of call */

    /* OUT */
    uint64_t ready_set;
    int ret;
};

/*
 * HVT_HYPERCALL_HALT: Terminate guest execution.
 *
 * (exit_status) will be returned to the host.
 *
 * Additionally, the guest may supply a (cookie) providing a hint to the
 * tender about where e.g. a trap frame may be found in guest memory. The
 * memory area pointed to by (cookie) must allow for at least
 * HVT_HALT_COOKIE_MAX bytes.
 */
#define HVT_HALT_COOKIE_MAX 512

struct hvt_hc_halt {
    /* IN */
    HVT_GUEST_PTR(void *) cookie;

    /* IN */
    int exit_status;
};


/* Asserts to verify ABI is consitent between compilations */

_Static_assert(sizeof(struct hvt_boot_info) == 40, "hvt_boot_info - Size mismatch");
_Static_assert(offsetof(struct hvt_boot_info, mem_size) == 0, "hvt_boot_info - Offset mismatch");
_Static_assert(offsetof(struct hvt_boot_info, kernel_end) == 8, "hvt_boot_info - Offset mismatch");
_Static_assert(offsetof(struct hvt_boot_info, cpu_cycle_freq) == 16, "hvt_boot_info - Offset mismatch");
_Static_assert(offsetof(struct hvt_boot_info, cmdline) == 24, "hvt_boot_info - Offset mismatch");
_Static_assert(offsetof(struct hvt_boot_info, mft) == 32, "hvt_boot_info - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_walltime) == 8, "hvt_hc_walltime - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_walltime, nsecs) == 0, "hvt_hc_walltime - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_puts) == 16, "hvt_hc_puts - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_puts, data) == 0, "hvt_hc_puts - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_puts, len) == 8, "hvt_hc_puts - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_block_write) == 40, "hvt_hc_block_write - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_block_write, handle) == 0, "hvt_hc_block_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_write, offset) == 8, "hvt_hc_block_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_write, data) == 16, "hvt_hc_block_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_write, len) == 24, "hvt_hc_block_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_write, ret) == 32, "hvt_hc_block_write - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_block_read) == 40, "hvt_hc_block_read - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_block_read, handle) == 0, "hvt_hc_block_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_read, offset) == 8, "hvt_hc_block_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_read, data) == 16, "hvt_hc_block_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_read, len) == 24, "hvt_hc_block_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_block_read, ret) == 32, "hvt_hc_block_read - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_net_write) == 32, "hvt_hc_net_write - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_net_write, handle) == 0, "hvt_hc_net_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_write, data) == 8, "hvt_hc_net_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_write, len) == 16, "hvt_hc_net_write - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_write, ret) == 24, "hvt_hc_net_write - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_net_read) == 32, "hvt_hc_net_read - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_net_read, handle) == 0, "hvt_hc_net_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_read, data) == 8, "hvt_hc_net_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_read, len) == 16, "hvt_hc_net_read - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_net_read, ret) == 24, "hvt_hc_net_read - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_poll) == 24, "hvt_hc_poll - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_poll, timeout_nsecs) == 0, "hvt_hc_poll - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_poll, ready_set) == 8, "hvt_hc_poll - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_poll, ret) == 16, "hvt_hc_poll - Offset mismatch");

_Static_assert(sizeof(struct hvt_hc_halt) == 16, "hvt_hc_halt - Size mismatch");
_Static_assert(offsetof(struct hvt_hc_halt, cookie) == 0, "hvt_hc_halt - Offset mismatch");
_Static_assert(offsetof(struct hvt_hc_halt, exit_status) == 8, "hvt_hc_halt - Offset mismatch");

#endif /* HVT_ABI_H */
