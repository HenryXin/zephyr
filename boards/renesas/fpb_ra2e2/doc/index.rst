.. zephyr:board:: fpb_ra2e2

Overview
********

The FPB-RA2E2 Fast Prototyping Board provides an entry point for evaluation and
prototyping with the RA2E2 MCU Group. It includes an on-board E2 emulator
circuit, Arduino Uno R3 and Pmod connectors, and through-hole access to MCU pins.

**MCU**

- R7FA2E2A72DNK (or A74CNK per board batch; UM allows either for BSP selection)
- 48 MHz Arm Cortex-M23, 64 KB code flash, 8 KB SRAM, 24-pin HWQFN
- On-board 24 MHz and 32.768 kHz crystals
- E2 Emulator On-Board (SWD)

Supported Features
==================

.. zephyr:board-supported-hw::

Programming and Debugging
*************************

Flash with the on-board E2 emulator via Renesas Flash Programmer:

.. code-block:: console

   west flash -r rfp

Console UART is SCI9 on Arduino D1/D0 (P109 TXD9 / P110 RXD9) at 115200 8N1.
Those pins are shared with the on-board E2 debugger (UM E25/E26/E28/E29); a
short post-kernel busy-wait lets the debugger release the lines before console
TX.

References
**********

.. _FPB-RA2E2 product page:
   https://www.renesas.com/en/design-resources/boards-kits/fpb-ra2e2
