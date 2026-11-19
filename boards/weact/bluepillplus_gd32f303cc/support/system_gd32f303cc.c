/*
 * Copyright The Zephyr Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * GD32F303CCT6 clock bring-up while using the in-tree gd32f403 HAL stand-in.
 *
 * Use HXTAL (8 MHz) directly as CK_SYS. F403 PLL helpers disagree with this HD
 * part (PLL1 / PLLMF bit positions), which produced wrong USART baud.
 */

#include "gd32f403.h"

#define SYSCLK_FREQ_HXTAL 8000000U

uint32_t SystemCoreClock = SYSCLK_FREQ_HXTAL;

static void system_clock_hxtal(void)
{
	uint32_t timeout = 0U;
	uint32_t stab_flag = 0U;

	RCU_CTL |= RCU_CTL_HXTALEN;

	do {
		timeout++;
		stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
	} while ((stab_flag == 0U) && (timeout != HXTAL_STARTUP_TIMEOUT));

	if ((RCU_CTL & RCU_CTL_HXTALSTB) == 0U) {
		while (1) {
		}
	}

	RCU_CFG0 &= ~RCU_CFG0_SCS;
	RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;
	while ((RCU_CFG0 & RCU_SCSS_HXTAL) == 0U) {
	}
}

void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << (10 * 2)) | (3UL << (11 * 2)));
#endif

	RCU_CTL |= RCU_CTL_IRC8MEN;
	{
		volatile uint32_t i;

		RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
		for (i = 0U; i < 20000U; i++) {
		}
	}

	RCU_CFG0 = 0U;
	RCU_CFG1 = 0U;

	RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);
	RCU_CTL &= ~RCU_CTL_HXTALBPS;
	RCU_INT = 0x00FF0000U;

	system_clock_hxtal();
	SystemCoreClock = SYSCLK_FREQ_HXTAL;
}

void SystemCoreClockUpdate(void)
{
	SystemCoreClock = SYSCLK_FREQ_HXTAL;
}
