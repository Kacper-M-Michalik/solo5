#ifndef __SEL4_BINDINGS_H__
#define __SEL4_BINDINGS_H__

#include "../bindings.h"
#include "sel4_abi.h"

void time_init(void);
void console_init(void);
void block_init(struct sel4_boot_info *arg);
void net_init(struct sel4_boot_info *arg);

#endif /* __SEL4_BINDINGS_H__ */

/* solo5_app_main linked by user program */

/* solo5_exit is in exit.c */

/* solo5_abort is in abort.c */

/* solo5_tls_size is in tls.c */

/* solo5_tls_tp_offset is in tls.c */

/* _solo5_tls_data_offset is in tls.c */

/* solo5_tls_init is in tls.c */

/* solo5_set_tls_base is in tls.c */

/* solo5_clock_monotonic is in sel4/time.c */

/* solo5_clock_wall is in sel4/time.c */

/* solo5_yield is in sel4/time.c */

/* solo5_console_write is in sel4/console.c */

/* solo5_block_acquire is in sel4/block.c */

/* solo5_block_write is in sel4/block.c */

/* solo5_block_read is in sel4/block.c */

/* solo5_net_acquire is in sel4/net.c */

/* solo5_net_write is in sel4/net.c */

/* solo5_net_read is in sel4/net.c */
