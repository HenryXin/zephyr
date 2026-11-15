.. zephyr:board:: weact_stm32h723_core

Overview
********

The ``weact_stm32h723_core`` board is a compact development board equipped with
an STM32H723VGT6 microcontroller. It features a variety of peripherals,
including a user LED and button, a display, and external SPI and OctoSPI
NOR flash memory.

Key Features

- STM32 microcontroller in LQFP100 package
- USB OTG HS in full-speed device mode
- 1 user LED
- User, boot, and reset push-buttons
- 32.768 kHz and 25 MHz HSE crystal oscillators
- External NOR Flash memories: 64-Mbit OctoSPI and 64-Mbit SPI
- Board connectors:

  - Camera (8 bit) connector (OV2640 via ``weact_ov2640_cam_module`` shield)
  - ST7735 TFT-LCD 160 x 80 pixels (RGB565 3-SPI)
  - microSD™ card
  - USB Type-C Connector
  - SWD header for external debugger
  - 2x 22-pin GPIO connector

More information about the board can be found on the `WeAct MiniSTM32H723 website`_.

Hardware
********

The ``weact_stm32h723_core`` board provides the following hardware components:

- STM32H723VGT6 in LQFP100 package
- ARM 32-bit Cortex-M7 CPU with FPU
- 550 MHz max CPU frequency
- VDD from 1.62 V to 3.6 V
- 1 MB Flash
- 564 KB SRAM
- SPI(6)
- I2C(4)
- USART/UART
- USB OTG HS (FS device mode on this board)
- SD/MMC interface
- OctoSPI memory interface
- Camera Interface (DCMI)
- GPIO with external interrupt capability
- True Random Number Generator (RNG)

More information about STM32H723 can be found here:

- `STM32H723VG on www.st.com`_
- `STM32H723 reference manual`_
- `STM32H723 datasheet`_

Supported Features
==================

.. zephyr:board-supported-hw::

Pin Mapping
===========

Default Zephyr Peripheral Mapping:
----------------------------------

The ``weact_stm32h723_core`` board is configured as follows

- USER_LED : PE3
- USER_PB : PC13
- USART1 TX/RX : PA9/PA10
- SPI1 SCK/MISO/MOSI/NSS : PB3/PB4/PD7/PD6 (NOR Flash memory)
- SPI4 SCK/MOSI/NSS : PE12/PE14/PE11 (LCD)
- OctoSPI CLK/NCS/IO0/IO1/IO2/IO3 : PB2/PB6/PD11/PD12/PE2/PD13 (NOR Flash memory)
- SDMMC1 CLK/CMD/CD/D0/D1/D2/D3 : PC12/PD2/PD4/PC8/PC9/PC10/PC11 (microSD card)
- USB DM/DP : PA11/PA12 (USB CDC ACM)

System Clock
============

The STM32H723VG System Clock can be driven by an internal or external oscillator,
as well as by the main PLL clock. By default, the System clock is driven
by the PLL clock at 550 MHz. PLL clock is fed by a 25 MHz high speed external clock.

Serial Port (USB CDC ACM)
=========================

The Zephyr console output is assigned to the USB CDC ACM virtual serial port.
Virtual COM port interface. Default communication settings are 115200 8N1.

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

The ``weact_stm32h723_core`` board facilitates firmware flashing via the USB DFU
bootloader. This method simplifies the process of updating images, although
it doesn't provide debugging capabilities. However, the board provides header
pins for the Serial Wire Debug (SWD) interface, which can be used to connect
an external debugger, such as ST-Link.

Flashing
========

To activate the bootloader, follow these steps:

1. Press and hold the BOOT0 key.
2. While still holding the BOOT0 key, press and release the RESET key.
3. Wait for 0.5 seconds, then release the BOOT0 key.

Upon successful execution of these steps, the device will transition into
bootloader mode and present itself as a USB DFU Mode device. You can program
the device using the west tool or the STM32CubeProgrammer.

Flashing an application to ``weact_stm32h723_core``
---------------------------------------------------

Here is an example for the :zephyr:code-sample:`hello_world` application.

First, put the board in bootloader mode as described above. Then build and flash
the application in the usual way. Just add ``CONFIG_BOOT_DELAY=5000`` to the
configuration, so that USB CDC ACM is initialized before any text is printed,
as below:

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: weact_stm32h723_core
   :goals: build flash
   :gen-args: -DCONFIG_BOOT_DELAY=5000

Run a serial host program to connect with your board:

.. code-block:: console

   $ minicom -D <tty_device> -b 115200

Then, press the RESET button, you should see the following message after few seconds:

.. code-block:: console

   Hello World! weact_stm32h723_core

Replace :code:`<tty_device>` with the port where the board can be found.
For example, under Linux, :code:`/dev/ttyACM0`.

Debugging
---------

Debugging is supported via an external SWD probe connected to the SWD header
using OpenOCD or J-Link.

Testing the LEDs in the ``weact_stm32h723_core``
************************************************

There is a sample that allows to test that LED on the board are working
properly with Zephyr:

.. zephyr-app-commands::
   :zephyr-app: samples/basic/blinky
   :board: weact_stm32h723_core
   :goals: build flash
   :gen-args: -DCONFIG_BOOT_DELAY=5000

You can build and flash the examples to make sure Zephyr is running correctly on
your board. The LED definitions can be found in
:zephyr_file:`boards/weact/stm32h723_core/weact_stm32h723_core.dts`.

Testing shell over USB in the ``weact_stm32h723_core``
******************************************************

There is a sample that allows to test shell interface over USB CDC ACM interface
with Zephyr:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/shell/shell_module
   :board: weact_stm32h723_core
   :goals: build flash
   :gen-args: -DCONFIG_BOOT_DELAY=5000

Camera preview on LCD
*********************

With an OV2640 module on the DVP connector and the ST7735 LCD installed, capture
frames and show them on the display:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/video/capture_to_lvgl
   :board: weact_stm32h723_core
   :shield: weact_ov2640_cam_module
   :goals: build flash
   :gen-args: -DCONFIG_BOOT_DELAY=2000

Porting notes
*************

For a file-by-file explanation of this port (clock/USB/DCMI choices, OV2640
shield fragments, and LVGL camera color handling), see
:ref:`weact_stm32h723_core_porting_notes`.

.. toctree::
   :maxdepth: 1
   :hidden:

   porting_notes

.. _WeAct MiniSTM32H723 website:
   https://github.com/WeActStudio/WeActStudio.MiniSTM32H723

.. _STM32H723VG on www.st.com:
   https://www.st.com/en/microcontrollers-microprocessors/stm32h723vg.html

.. _STM32H723 reference manual:
   https://www.st.com/resource/en/reference_manual/dm00603761-stm32h723733-stm32h725735-and-stm32h730-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf

.. _STM32H723 datasheet:
   https://www.st.com/resource/en/datasheet/stm32h723vg.pdf
