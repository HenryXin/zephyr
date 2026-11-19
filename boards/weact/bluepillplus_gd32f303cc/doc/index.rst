.. zephyr:board:: bluepillplus_gd32f303cc

Overview
********

The WeAct Studio BluePill Plus with GD32F303CCT6 is a BluePill-form-factor
board with a GigaDevice GD32F303CCT6 Cortex-M4 MCU (120 MHz, 256 KB flash,
48 KB SRAM), an 8 MHz HSE, 32.768 kHz LSE, user LED on PB2, user key on PA0,
and USB-C on PA11/PA12.

.. note::

   Zephyr does not yet include a dedicated GD32F303 SoC port. This board
   package temporarily selects the in-tree ``gd32f403`` SoC support (patterned
   after :zephyr:board:`gd32f403z_eval`) and overrides flash/SRAM sizes and
   CPU frequency for the CCT6 part. Treat clocking and peripherals as
   experimental until a real ``gd32f30x`` SoC is added.

Hardware
********

- GD32F303CCT6 (LQFP48), Cortex-M4 up to 120 MHz
- 256 KB flash, 48 KB SRAM
- 8 MHz system crystal, 32.768 kHz RTC crystal
- Blue user LED on PB2 (active high)
- User key on PA0
- USART0 on PA9 (TX) / PA10 (RX) for console and ISP
- USB-C: PA11 (DN), PA12 (DP)
- SWD: PA13 / PA14

Supported Features
==================

.. zephyr:board-supported-hw::

Connections and IOs
===================

LED
---

* LED0 = Blue user LED (PB2)

Button
------

* SW0 = User KEY (PA0)

Serial Port
===========

Default console is USART0 at 115200 baud on PA9 (TX) and PA10 (RX).

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

An external SWD adapter (CMSIS-DAP, J-Link, or similar) is required. Connect:

* VCC / 3V3, GND
* SWDIO = PA13
* SWCLK = PA14

The board ships with a HID IAP bootloader; hold KEY while resetting to enter
it. USART ISP: hold BOOT0 while resetting; use PA9/PA10 at 115200 baud.

Flashing
========

Build and flash as usual, for example:

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: bluepillplus_gd32f303cc
   :goals: build flash

References
**********

.. target-notes::

.. _WeActStudio BluePill-Plus: https://github.com/WeActStudio/BluePill-Plus
.. _GD32F303CCT6: https://www.gigadevice.com/product/mcu/mcus-product-selector/gd32f303cct6
