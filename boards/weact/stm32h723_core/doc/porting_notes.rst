.. _weact_stm32h723_core_porting_notes:

Porting notes for WeAct STM32H723 Core
######################################

This page explains why the files added or changed for
:zephyr:board:`weact_stm32h723_core` exist, and which hardware or software
constraints drove each choice.

References used while porting
*****************************

- WeAct Studio MiniSTM32H723 hardware and examples
  (`WeActStudio.MiniSTM32H723`_)
- Existing WeAct Mini H7 boards in-tree (especially
  :zephyr:board:`mini_stm32h743` and :zephyr:board:`mini_stm32h7b0`) for the
  same PCB family pinout (LED, button, ST7735, SPI/OSPI flash, SD, DCMI)
- :zephyr:board:`stm32h735g_disco` and :zephyr:board:`nucleo_h723zg` for
  STM32H723/H735 clocking, USB OTG HS, and OctoSPI patterns

The H723 core board is the same MiniSTM32H7xx form factor as the H743/H750/H7B0
cores, with STM32H723VGT6 (1 MB flash, up to 550 MHz, USB OTG HS in FS mode,
OctoSPI instead of QuadSPI).

Board support files
*******************

Path: :zephyr_file:`boards/weact/stm32h723_core/`

``board.yml``
  Registers the board name ``weact_stm32h723_core`` and SoC ``stm32h723xx`` with
  the Zephyr board catalog (same naming pattern as
  ``weact_stm32h562_core``).

``Kconfig.weact_stm32h723_core``
  Selects ``SOC_STM32H723XX`` when this board is chosen.

``Kconfig.defconfig``
  Enables USB CDC ACM console defaults and, when LVGL is used with the onboard
  ST7735, sets 16 bpp and the historical ``LV_COLOR_16_SWAP`` default used by
  other WeAct Mini H7 boards for drawn UI. Camera preview samples override this
  (see below).

``weact_stm32h723_core_defconfig``
  Minimal board defaults: MPU, GPIO, UART/console so hello-world and blinky
  work without a project ``prj.conf``.

``weact_stm32h723_core.yaml``
  Twister metadata (RAM/flash sizes, supported features including ``video``).

``weact_stm32h723_core.dts``
  Devicetree for the board:

  - Clock tree: 25 MHz HSE, PLL at 550 MHz, ``hpre = 2`` (HCLK 275 MHz), LDO
    supply (H723 has no SMPS). Aligns with WeAct CubeMX examples and the
    H735 Discovery PLL style.
  - Console: USB OTG **HS** in FS device mode on PA11/PA12 (H723 has no OTG FS
    block; same approach as Nucleo H723 / Mini H7B0), plus CDC ACM include.
  - USART1 on PA9/PA10 for ISP / optional UART console.
  - User LED PE3, user button PC13 (WeAct Mini H7xx convention).
  - SPI4 + ST7735 160x80, SPI1 W25Q64, OctoSPI1 W25Q64, SDMMC1, DCMI + I2C1
    labels ``zephyr_camera_dvp`` / ``zephyr_camera_i2c`` so the existing
    ``weact_ov2640_cam_module`` shield can attach.

``board.cmake`` / ``support/openocd.cfg``
  Runners: DFU (ROM bootloader), OpenOCD, and J-Link (``STM32H723VG``). No
  on-board ST-LINK; SWD is via the header.

``doc/``
  User-facing board documentation and board image.

OV2640 shield board fragments
*****************************

Path: :zephyr_file:`boards/shields/weact_ov2640_cam_module/boards/`

``weact_stm32h723_core.overlay``
  Board-specific pieces the generic shield overlay cannot know:

  - **MCO1** on PA8 from HSI48 / 4 as OV2640 XCLK (same as Mini H743 shield
    overlay).
  - **DMA1 / DMAMUX1** enabled for DCMI.
  - **PWDN** via the DCMI connector GPIO and ``clock-rate-control`` for the
    sensor.

  HCLK stays at 275 MHz (550 MHz / 2). That keeps AHB / DCMI_PIXCLK above the
  2.5 minimum from AN5020 for QQVGA. The H743 shield overlay lowers SYSCLK and
  sets ``hpre = 1`` because that board otherwise runs with a lower HCLK; the
  H723 defaults already satisfy the ratio, so the clock tree is left unchanged.

``weact_stm32h723_core.conf``
  Enables ``CONFIG_VIDEO_HFLIP`` / ``CONFIG_VIDEO_VFLIP`` so the preview
  orientation matches the module mounting (same as Mini H743).

Video capture to LVGL sample
****************************

Path: :zephyr_file:`samples/subsys/video/capture_to_lvgl/`

``boards/weact_stm32h723_core.conf``
  QQVGA 160x120 capture buffers sized for the heap, boot delay for USB CDC,
  and ``CONFIG_LV_COLOR_16_SWAP=n``. Camera frames from OV2640 over DCMI are
  already high-byte-first RGB565 in memory; the legacy LVGL flush swap would
  corrupt colors if left enabled.

``boards/weact_stm32h723_core.overlay``
  Sets ST7735 ``madctl = 176`` (0xb0): 180° rotation with RGB order (BGR bit
  clear). Together with the board's ``rgb-is-inverted``, the display reports
  ``PIXEL_FORMAT_RGB_565X``, which matches DCMI byte order for this panel.

``src/main.c``
  Image descriptors use ``lv_display_get_color_format()`` instead of
  ``LV_COLOR_FORMAT_NATIVE``. ``NATIVE`` is a compile-time default (often plain
  RGB565) and does not follow a runtime byte-swapped display format, so LVGL
  was converting OV2640 buffers incorrectly. Tagging frames with the display's
  actual format fixes the preview colors on this board (and any similar
  RGB565X panel).

``README.rst`` / ``tests.yaml``
  Document and twister-allow ``weact_stm32h723_core`` with
  ``SHIELD=weact_ov2640_cam_module``.

LVGL demos board conf
*********************

Path: :zephyr_file:`samples/modules/lvgl/demos/boards/weact_stm32h723_core.conf`

Optional memory/stack tuning for the upstream LVGL widgets demo on the small
160x80 panel. Not required for camera preview.

How to rebuild the camera preview
*********************************

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/video/capture_to_lvgl
   :board: weact_stm32h723_core
   :shield: weact_ov2640_cam_module
   :goals: build flash
   :gen-args: -DCONFIG_BOOT_DELAY=2000

Flash with an external probe, for example::

   west flash --runner jlink

or enter the ROM DFU bootloader and use ``west flash --runner dfu-util``.

.. _WeActStudio.MiniSTM32H723:
   https://github.com/WeActStudio/WeActStudio.MiniSTM32H723
