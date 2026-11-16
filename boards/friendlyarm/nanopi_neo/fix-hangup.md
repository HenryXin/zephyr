# NanoPi NEO — Fix silent hang after U-Boot `go`

Symptom, root causes, and SoC fixes for:

```text
=> fatload mmc 0:1 0x40000000 /boot/zephyr.bin
28796 bytes read in 3 ms (9.2 MiB/s)
=> go 0x40000000
## Starting application at 0x40000000 ...
```

with no Zephyr console output after that line.

Same hang appears with:

```text
=> setenv autostart yes
=> fatload mmc 0:1 0x42000000 /boot/zephyr.elf
=> bootelf -p 0x42000000
## Starting application at 0x40002xxx ...
```

Board howto: [howto.md](howto.md)

## What was going wrong

Zephyr **does** enter (`## Starting application` means U-Boot jumped into the
image). It dies in early Cortex-A bring-up before `printk` / the UART console
are up.

Two independent problems stacked:

### 1. Wrong vector relocation on ARMv7-A (primary hang)

Default `relocate_vector_table()` for ARMv7-A (not `CONFIG_AARCH32_ARMV8_A`):

- `memcpy`s the vector table to **address `0`**
- does **not** program `VBAR`

On sun8i the image lives in DRAM at `0x40000000`. U-Boot leaves `VBAR` aimed
at its own handlers. After the jump:

- copy-to-0 is useless for DRAM-resident vectors (and can fault / confuse)
- the first exception (SVC, IRQ, abort, …) vectors into U-Boot → silent hang

Other Cortex-A SoCs already override this (e.g. RZ/A2M, STM32MP13, SAMA5) by
writing `VBAR` to `_vector_start`.

### 2. U-Boot left SCTLR / cache / MMU state dirty

`go` / `bootelf` do **not** run `cleanup_before_linux`. At entry:

- MMU still on (U-Boot page tables)
- I/D caches still on
- strict-alignment bit (`SCTLR.A`) may still be set

Zephyr then installs its own MMU tables on top of that. Stale translations,
unaligned faults, and dirty D-cache lines vs DRAM show up as early hangs
before the console.

### 3. Arch timer clock was wrong (timing, not the silent hang)

`SYS_CLOCK_HW_CYCLES_PER_SEC` was taken from the CPU `clock-frequency`
(625 MHz). On H3 the ARM generic timer (`CNTPCT`) runs from the **24 MHz**
OSC (`CONFIG_COUNTER_FREQUENCY` in U-Boot). Wrong tick rate breaks delays and
scheduling once the console works.

The DT timer compatible is `arm,armv7-timer` (Cortex-A7), not `arm,armv8-timer`.

## Code changes

| File | Change |
|------|--------|
| `soc/allwinner/sun8i_h3/soc.c` | Override `relocate_vector_table()` → `write_vbar(_vector_start)`; map vectors at `CONFIG_SRAM_BASE_ADDRESS`; `soc_reset_hook()` clean/invalidate caches, clear `SCTLR` M/A/C/I, invalidate TLB |
| `soc/allwinner/sun8i_h3/Kconfig.defconfig` | `CONFIG_SOC_RESET_HOOK=y`; `SYS_CLOCK_HW_CYCLES_PER_SEC=24000000` |
| `soc/allwinner/sun8i_h3/CMakeLists.txt` | Always build `soc.c` (hook + MMU regions), not only when MMU Kconfig is set |
| `dts/arm/allwinner/sun8i-h3.dtsi` | `compatible = "arm,armv7-timer"` |

### `relocate_vector_table()` (must not copy to 0)

```c
void relocate_vector_table(void)
{
	write_sctlr(read_sctlr() & ~HIVECS);
	write_vbar(((uintptr_t)_vector_start) & VBAR_MASK);
	barrier_isync_fence_full();
}
```

### `soc_reset_hook()` (U-Boot handoff)

```c
void soc_reset_hook(void)
{
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
```

MMU flat regions keep the vector page at DRAM base (not `0x0`), plus GIC and
UART0 MMIO.

## Rebuild and retest

```bash
export ZEPHYR_BASE=/path/to/zephyr
west build -p always -b nanopi_neo -d build/nanopi_neo_hello samples/hello_world

sudo mount /dev/sdX1 /mnt/neo
sudo cp build/nanopi_neo_hello/zephyr/zephyr.bin /mnt/neo/boot/
sync
sudo umount /mnt/neo
```

On the board (115200 8N1 on Debug UART0):

```text
=> fatload mmc 0:1 0x40000000 /boot/zephyr.bin
=> go 0x40000000
```

Expected:

```text
*** Booting Zephyr OS v... ***
Hello World! nanopi_neo/sun8i_h3
```

ELF path (optional):

```text
=> setenv autostart yes
=> fatload mmc 0:1 0x42000000 /boot/zephyr.elf
=> bootelf -p 0x42000000
```

## Autoboot (separate from the hang fix)

Manual `fatload` + `go` working does **not** enable power-on boot. Default
U-Boot still runs `distro_bootcmd`. Set once:

```
=> setenv bootdelay 2
=> setenv bootcmd 'fatload mmc 0:1 0x40000000 /boot/zephyr.bin; go 0x40000000'
=> saveenv
=> reset
```

See [howto.md](howto.md) section 4b.

## Quick checklist if it still hangs

| Check | Notes |
|-------|--------|
| Image actually updated on SD | `ls mmc 0:1 /boot` size/date; remount + `sync` |
| Jump address | `go 0x40000000` for `.bin`; ELF entry is `z_arm_reset` via `bootelf -p` |
| Serial | 115200, TX/RX not swapped, common GND, 3.3 V TTL |
| DRAM size | U-Boot `DRAM:` line; DTS defaults to 512 MiB — use 256 MiB in DTS if that is the SKU |
| U-Boot init | SPL must have brought up DRAM/clocks; Zephyr does not |

## Related symbols (sanity)

After a good build:

- `relocate_vector_table` writes `VBAR` with `0x40000000` (not `memcpy` to `0`)
- `soc_reset_hook` is linked (`CONFIG_SOC_RESET_HOOK=y`)
- `CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC=24000000`
- `CONFIG_SRAM_BASE_ADDRESS=0x40000000`
