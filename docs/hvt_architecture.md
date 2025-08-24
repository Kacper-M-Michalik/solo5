# General overview
The HVT bindings provide a uniform way to build solo5 unikernels per architecture, which then can be easily ported to new hypervisors/platforms by implementing 
platform specfic Virtual Manchine Monitors, this document provides the exact setup your VMM must perform to run a HVT unikernel.

Depending on your HVT build, you will have a different memory layout and hypercall handling method that your VMM must follow. 
HVT unikernels need to be virtualised, and will run in EL1 (Ring 0 in x86_64), memory mapping (aka. pagetables and entries) 
and initial registers need to be setup by your VMM, there are 4 different memory layouts possible, 
defined by (ARCH, USE_HYPERCALL_INSTRUCTION), the HVT interface defines several hypercalls your VMM must handle, namely:

HVT_HYPERCALL_WALLTIME,
HVT_HYPERCALL_PUTS,
HVT_HYPERCALL_POLL,
HVT_HYPERCALL_BLOCK_WRITE,
HVT_HYPERCALL_BLOCK_READ,
HVT_HYPERCALL_NET_WRITE,
HVT_HYPERCALL_NET_READ,
HVT_HYPERCALL_HALT

# Layouts

Aarch64, Default:

/*
 * As the design of hvt_do_hypercall, the guest memory has been limited
 * to the area of 0 ~ 4GB. So we can design an address space layout for
 * guest as follow:
 *
 * Guest address space layout
 * ---------------------------------------------------------------
 * | 0 ~ 0xFFFFFFFF for RAM | 0x100000000 ~ 0x13FFFFFFF for MMIO |
 * ---------------------------------------------------------------
 * 0x100000000 MMIO space start
 * 0x0FFFFFFFF End of RAM space
 * 0x100000    loaded elf file (linker script dictates location)
 *   ...       unused ram
 * 0x010000    hvt_boot_info starts
 * 0x007000    PTE
 * 0x006000    PMD3
 * 0x005000    PMD2
 * 0x004000    PMD1
 * 0x003000    PMD0, 4 pages to cover max 4GB RAM
 * 0x002000    PUD
 * 0x001000    PGD, memory start for page table
 * 0x000000    unused ram
 */

Aarch64, USE_HYPERCALL_INSTRUCTION:

/*
 * As the design of hvt_do_hypercall, the guest memory has been limited
 * to the area of 0 ~ 4GB. So we can design an address space layout for
 * guest as follow:
 *
 * Guest address space layout
 * --------------------------
 * | 0 ~ 0xFFFFFFFF for RAM |
 * --------------------------
 * 0x0FFFFFFFF End of RAM space
 * 0x100000    loaded elf file (linker script dictates location)
 *   ...       unused ram
 * 0x010000    hvt_boot_info starts
 * 0x007000    PTE
 * 0x006000    PMD3
 * 0x005000    PMD2
 * 0x004000    PMD1
 * 0x003000    PMD0, 4 pages to cover max 4GB RAM
 * 0x002000    PUD
 * 0x001000    PGD, memory start for page table
 * 0x000000    unused ram
 */
 
 