.. zephyr:board:: nanopi_neo

Overview
********

`NanoPi NEO`_ is an open-source single-board computer from FriendlyARM
(FriendlyElec). It uses the Allwinner H3 SoC and comes with 256MB or 512MB of
DDR3 SDRAM. The Allwinner H3 SoC is based on a quad-core ARM Cortex-A7
processor.

Hardware
********

- Allwinner H3 (quad-core Cortex-A7, up to 1.2 GHz)
- 256MB or 512MB DDR3 SDRAM
- 10/100M Ethernet
- MicroSD slot
- USB Type-A host and MicroUSB OTG/power
- Debug UART0 (4-pin header, 115200 8N1)
- Status LED on PA10

Supported Features
==================

.. zephyr:board-supported-hw::

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

The Allwinner H3 SoC needs to be initialized prior to running a Zephyr
application. This can be achieved in a number of ways (e.g. Das U-Boot Secondary
Program Loader (SPL), ...).

The instructions here use the U-Boot SPL. For further details and instructions
for using Das U-Boot with Allwinner H3 SoCs, see the following documentation:

- `Das U-Boot Website`_
- `Using U-Boot With Allwinner SoCs`_

Building Das U-Boot
===================

Clone and build Das U-Boot for the NanoPi NEO:

.. code-block:: console

   git clone -b v2024.01 https://source.denx.de/u-boot/u-boot.git
   cd u-boot
   make distclean
   make nanopi_neo_defconfig
   export PATH=/path/to/zephyr-sdk/arm-zephyr-eabi/bin/:$PATH
   export CROSS_COMPILE=arm-zephyr-eabi-
   make
   sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/mmcblkX bs=1024 seek=8

Building and Flashing
=====================

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :host-os: unix
   :board: nanopi_neo
   :goals: build

Copy the compiled ``zephyr.bin`` to the boot directory of the SD card and plug
it into the board.

.. code-block:: console

   => fatload mmc 0:1 0x40000000 /boot/zephyr.bin
   => go 0x40000000

You should see the following output on the serial console:

.. code-block:: text

   *** Booting Zephyr OS vx.x.x-xxx-gxxxxxxxxxxxx ***
   Hello World! nanopi_neo/sun8i_h3

To autoboot on power-on (U-Boot otherwise runs distro/EFI boot and never
loads ``zephyr.bin``), set and save the environment once:

.. code-block:: console

   => setenv bootdelay 2
   => setenv bootcmd 'fatload mmc 0:1 0x40000000 /boot/zephyr.bin; go 0x40000000'
   => saveenv
   => reset

Practical bring-up notes (SD layout, samples, hang checklist) are in
``boards/friendlyarm/nanopi_neo/howto.md``.

.. _NanoPi NEO:
   https://wiki.friendlyelec.com/wiki/index.php/NanoPi_NEO

.. _Das U-Boot Website:
   https://docs.u-boot.org/en/latest/

.. _Using U-Boot With Allwinner SoCs:
   https://docs.u-boot.org/en/stable/board/allwinner/sunxi.html
