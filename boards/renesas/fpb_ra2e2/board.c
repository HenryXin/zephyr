/*
 * Copyright (c) 2026 Renesas Electronics Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <zephyr/init.h>

/*
 * SCI9 (P109/P110) is shared with the on-board E2. After reset the debugger
 * briefly contends those lines and corrupts early UART TX. Spin after the
 * clocks are running (not in board_early_init_hook — that is pre-HOCO and the
 * same loop can take minutes). Avoid k_busy_wait(); the cycle counter has
 * hung on this SoC during bring-up.
 */
static int fpb_ra2e2_uart_settle(void)
{
	for (volatile uint32_t i = 0U; i < 8000000U; i++) {
	}

	return 0;
}

SYS_INIT(fpb_ra2e2_uart_settle, POST_KERNEL, 0);
