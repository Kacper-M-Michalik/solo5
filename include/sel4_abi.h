/*
 * sel4_abi.h: sel4 guest interface definitions.
 *
 * This header file must be kept self-contained with no external dependencies
 * other than C99 headers.
 */

#ifndef SEL4_ABI_H
#define SEL4_ABI_H

#include <stddef.h>
#include <stdint.h>
#include "elf_abi.h"
#include "hvt_abi.h"

/*
 * A pointer to this structure is passed by the tender as the sole argument to
 * the guest entrypoint.
 */
//struct sel4_boot_info {
//    uint64_t mem_size;                  /* Memory size in bytes */
//    uint64_t kernel_end;                /* Address of end of kernel */ //Not sure what this represents, max high memory?
//    const char *cmdline;                /* Address of command line (C string) */ //Change to capability?
//    //Add capaibility fields
//};
#undef __x86_64__

#ifdef __x86_64__

static inline void sel4_do_hypercall(int n, volatile void *arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif

    register uint64_t rax __asm__("rax") = n;
    register void* rbx __asm__("rbx") = arg;

    __asm__ __volatile__(
        "mfence\n\t"
        "vmcall\n\t"
        "mfence\n\t"
        : "+r"(rax)
        : "r"(rbx)
        : "rcx", "rdx", "rsi", "rdi", "memory"
    );

}

#elif defined(__aarch64__)

static inline void sel4_do_hypercall(int n, volatile void* arg)
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

#else
#    error Unsupported architecture
#endif

#endif /* SEL4_ABI_H */
