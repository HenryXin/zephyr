# NanoPi NEO — How to Run Tests

Practical steps to build Zephyr for the FriendlyARM NanoPi NEO
(`nanopi_neo/sun8i_h3`), load it via U-Boot, and verify on hardware.

Board docs: [doc/index.rst](doc/index.rst)  
Wiki: https://wiki.friendlyelec.com/wiki/index.php/NanoPi_NEO

## Hardware checklist

- NanoPi NEO (Allwinner H3, 256MB or 512MB DDR3)
- MicroSD card (8GB+ recommended)
- USB serial adapter wired to the **Debug UART0** header (115200 8N1)
  - Pin order on the 4-pin header: GND, 5V, UART0_TX, UART0_RX
  - Connect adapter RX to board TX, adapter TX to board RX, common GND
- 5V power via MicroUSB (or 5V on the debug header if your adapter supplies it)

Zephyr does not initialize DRAM/clocks itself. U-Boot SPL must run first.

## 1. Host environment

From a west workspace (parent of this `zephyr` tree):

```bash
export ZEPHYR_BASE=/path/to/zephyr
# Optional in a git worktree:
# export ZEPHYR_BASE=/path/to/this/worktree

source ${ZEPHYR_BASE}/../.venv/bin/activate   # if you use a venv
west zephyr-export
```

Confirm the board is visible:

```bash
west boards | grep nanopi_neo
```

## 2. Build and install U-Boot on the SD card

```bash
git clone -b v2024.01 https://source.denx.de/u-boot/u-boot.git
cd u-boot
make distclean
make nanopi_neo_defconfig
export PATH=/path/to/zephyr-sdk/arm-zephyr-eabi/bin:$PATH
export CROSS_COMPILE=arm-zephyr-eabi-
make -j$(nproc)
```

(sudo apt-get install -y swig if missing swig)

Write SPL+U-Boot to the card (**replace** `/dev/sdX` with your device;
double-check with `lsblk` first):

```bash
sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/sdX bs=1024 seek=8 conv=fsync
```

Create a FAT partition if needed, mount it, and make a `boot` directory:

```bash
sudo mkdir -p /mnt/neo
sudo mount /dev/sdX1 /mnt/neo
sudo mkdir -p /mnt/neo/boot
```

## 3. Build Zephyr (smoke test)

```bash
cd ${ZEPHYR_BASE}
west build -p always -b nanopi_neo -d build/nanopi_neo_hello samples/hello_world
```

Copy the image to the SD card:

```bash
sudo cp build/nanopi_neo_hello/zephyr/zephyr.bin /mnt/neo/boot/zephyr.bin
sync
sudo umount /mnt/neo
```

## 4. Boot and check the console

1. Insert the SD card and power the board.
2. Open a serial terminal on the host:

   ```bash
   # replace ttyUSB0 if needed
   picocom -b 115200 /dev/ttyUSB0
   # or: minicom -D /dev/ttyUSB0 -b 115200
   # or: screen /dev/ttyUSB0 115200
   ```

3. At the U-Boot prompt:

   ```
   => fatload mmc 0:1 0x40000000 /boot/zephyr.bin
   => go 0x40000000
   ```

Expected output:

```text
*** Booting Zephyr OS v... ***
Hello World! nanopi_neo/sun8i_h3
```

## 4b. Autoboot Zephyr (replace distro/EFI boot)

Mainline U-Boot defaults to `run distro_bootcmd` (EFI → PXE → …). That does
**not** load `/boot/zephyr.bin`. If you let the countdown finish without
changing `bootcmd`, you will see stock distro noise such as:

```text
Scanning mmc 0:1...
No EFI system partition
No EFI system partition
Failed to persist EFI variables
BootOrder not defined
EFI boot manager: Cannot load any image

Device 0: unknown device
sun8i_emac_eth_start: Timeout
missing environment variable: pxeuuid
Retrieving file: pxelinux.cfg/...
sun8i_emac_eth_start: Timeout
Config file not found
```

That is expected until Zephyr’s `bootcmd` is saved. It is not a Zephyr failure.

### Set autoboot once

1. Reset the board and **hit a key** as soon as the U-Boot countdown starts so
   you get the `=>` prompt (do not let distro boot finish).

2. At the prompt:

   ```
   => setenv bootdelay 2
   => setenv bootcmd 'fatload mmc 0:1 0x40000000 /boot/zephyr.bin; go 0x40000000'
   => saveenv
   => printenv bootcmd
   => boot
   ```

3. `printenv bootcmd` must show the `fatload`…`go` line, **not**
   `run distro_bootcmd`. Then `boot` should load Zephyr. After that, `reset`
   or a power-cycle should autoboot the same way.

After a successful save, U-Boot counts down `bootdelay` seconds and then loads
and `go`s Zephyr with no keypress. Hit a key during the countdown only when you
want the `=>` prompt again.

Also useful:

```
=> printenv bootdelay
```

If `saveenv` fails, the FAT partition is missing or read-only; recreate /
remount it writable (see section 2). Env lives in FAT on this build
(`CONFIG_ENV_IS_IN_FAT`). Paste the `saveenv` error if it does not stick.

To restore stock distro boot later:

```
=> setenv bootcmd 'run distro_bootcmd'
=> saveenv
```

## 5. Run more samples on hardware

Same load address and U-Boot commands as above; only the build changes.

| Sample | Command |
|--------|---------|
| Shell | `west build -p always -b nanopi_neo -d build/neo_shell samples/subsys/shell/shell_module` |
| Philosophers | `west build -p always -b nanopi_neo -d build/neo_phil samples/philosophers` |
| Synchronization | `west build -p always -b nanopi_neo -d build/neo_sync samples/synchronization` |

After each build:

```bash
sudo mount /dev/sdX1 /mnt/neo
sudo cp build/<dir>/zephyr/zephyr.bin /mnt/neo/boot/zephyr.bin
sync
sudo umount /mnt/neo
```

Then reset the board and `fatload` / `go` again (or use the saved `bootcmd`).

## 6. Twister (build-only CI-style checks)

There is no on-target runner wired for this board yet. Use twister to
**compile** samples/tests:

```bash
cd ${ZEPHYR_BASE}

# Build-only for this board (example: hello_world)
west twister --build-only -p nanopi_neo \
  -T samples/hello_world

# Broader smoke (still build-only; can take a while)
west twister --build-only -p nanopi_neo \
  -T samples/hello_world \
  -T samples/philosophers \
  -T samples/synchronization \
  -T samples/subsys/shell/shell_module
```

Useful flags:

- `-i` — print logs for failures
- `--inline-logs` — show failing build logs inline
- `-c` — clobber previous twister output
- `--build-dir /tmp/twister-neo` — keep artifacts out of the tree

## 7. Twister on hardware (manual device testing)

When you want twister to flash/run on a connected board, you need a working
runner and device map. This H3 bring-up path is U-Boot + SD (not west flash).
Typical approach:

1. Keep using SD + U-Boot as in sections 3–4 for interactive tests.
2. Or script a custom handler that copies `zephyr.bin` to the SD (or TFTP) and
   resets the board while twister watches the serial port.

Example serial-only observation while you load the image yourself:

```bash
west twister -p nanopi_neo --device-testing \
  --device-serial /dev/ttyUSB0 \
  -T samples/hello_world
```

This only works if a flash/runner path exists for the platform. Until then,
prefer **build-only twister** plus **manual U-Boot load** for on-target checks.

## 8. Quick failure checklist

| Symptom | Things to check |
|---------|-----------------|
| No U-Boot banner | SD image/`dd` offset (`seek=8`), card seating, 5V power |
| U-Boot OK, no Zephyr | Path `/boot/zephyr.bin`, `fatload` address `0x40000000`, correct `zephyr.bin` |
| Garbage on serial | Baud 115200, TX/RX not swapped, common ground |
| Hang after `go` | Wrong binary/SoC, DRAM size mismatch (256MB vs 512MB SKU) |
| `west build` board not found | `ZEPHYR_BASE` points at this tree; run from west workspace |

## 9. Rebuild after board changes

```bash
cd ${ZEPHYR_BASE}
west build -p always -b nanopi_neo -d build/nanopi_neo_hello samples/hello_world
```

`-p always` forces a pristine rebuild so DTS/Kconfig edits are picked up.
