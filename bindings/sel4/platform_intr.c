#include "bindings.h"

/*
 * Interrupts are not used on the sel4 platform, hence these are stubs.
 */

void platform_intr_ack_irq(unsigned irq __attribute__((unused)))
{
}

void platform_intr_mask_irq(unsigned irq __attribute__((unused)))
{
}

void platform_intr_clear_irq(unsigned irq __attribute__((unused)))
{
}
