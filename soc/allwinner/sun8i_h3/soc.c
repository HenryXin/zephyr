/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/arch/cpu.h>
#include <zephyr/kernel.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/sys/barrier.h>
#include <zephyr/sys/util.h>

#include <cmsis_core.h>
#include <zephyr/arch/arm/mmu/arm_mmu.h>

static const struct arm_mmu_region mmu_regions[] = {
	/* Vector table lives at the start of DRAM, not at address 0. */
	MMU_REGION_FLAT_ENTRY("vectors", CONFIG_SRAM_BASE_ADDRESS, 0x1000,
			      MT_STRONGLY_ORDERED | MPERM_R | MPERM_X),

	MMU_REGION_FLAT_ENTRY("gic", 0x01c81000, 0x3000, MT_STRONGLY_ORDERED | MPERM_R | MPERM_W),

	MMU_REGION_FLAT_ENTRY("uart", 0x01c28000, 0x1000, MT_DEVICE | MPERM_R | MPERM_W),
};

const struct arm_mmu_config mmu_config = {
	.num_regions = ARRAY_SIZE(mmu_regions),
	.mmu_regions = mmu_regions,
};

/*
 * Default ARMv7-A relocate_vector_table() memcpy's the vectors to address 0.
 * On sun8i the image runs from DRAM (0x40000000) and U-Boot leaves VBAR aimed
 * at its own handlers, so that path either bus-faults or takes the first
 * exception into U-Boot. Point VBAR at Zephyr's table in DRAM instead.
 */
void relocate_vector_table(void)
{
	write_sctlr(read_sctlr() & ~HIVECS);
	write_vbar(((uintptr_t)_vector_start) & VBAR_MASK);
	barrier_isync_fence_full();
}

#if defined(CONFIG_SOC_RESET_HOOK)
void soc_reset_hook(void)
{
	uint32_t sctlr;

	/*
	 * U-Boot's go/bootelf leaves the MMU and caches enabled. Clean the
	 * D-cache before clearing SCTLR.C/M so DRAM matches what the CPU saw,
	 * then drop back to a known SCTLR state before Zephyr builds its own
	 * page tables.
	 */
	L1C_CleanInvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();

	sctlr = __get_SCTLR();
	sctlr &= ~(SCTLR_M_Msk | SCTLR_A_Msk | SCTLR_C_Msk | SCTLR_I_Msk);
	__set_SCTLR(sctlr);

	__set_TLBIALL(0);
	barrier_dsync_fence_full();
	barrier_isync_fence_full();
}
#endif
