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

/*
 * ABI version. This must be incremented before cutting a release of Solo5 if
 * any material changes are made to the interfaces or data structures defined
 * in this file.
 */

#define SEL4_ABI_VERSION 2

/*
 * Lowest virtual address at which guests can be loaded.
 */
//TODO CHANGE ARGS NEED TO BE PASSED IN SEE HOW LARGE
#define SEL4_GUEST_MIN_BASE 0

/*
 * A pointer to this structure is passed by the tender as the sole argument to
 * the guest entrypoint.
 */
struct sel4_boot_info {
    uint64_t mem_size;                  /* Memory size in bytes */
    uint64_t kernel_end;                /* Address of end of kernel */ //Not sure what this represents, max high memory?
    const char *cmdline;                /* Address of command line (C string) */ //Change to capability?
    //Add capaibility fields
};

#endif /* SEL4_ABI_H */
