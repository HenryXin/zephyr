.. zephyr:board:: steval_stwinkt1b

Overview
********

The STWIN (STEVAL-STWINKT1B) is a development kit and reference design that
features an Arm® Cortex®-M4 based STM32L4R9ZI MCU and simplifies prototyping
and testing of advanced industrial sensing applications in IoT contexts such as
condition monitoring and predictive maintenance.

The STEVAL-STWINKT1B kit consists of an STWIN core system, a LiPo battery, an
STLINK-V3MINI debugger, a plastic case, and a 36-pin board-to-board connector for
adapter boards.

More information about the board can be found at the `STEVAL-STWINKT1B Development kit website`_.


Supported Features
******************

The STEVAL-STWINKT1B provides motion, environmental, and audio
sensor data through either the built-in RS485 transceiver, BLE, Wi-Fi (with
expansion), and NFC or USB protocols to a host application running on a
smartphone/PC to implement applications such as:

- Multisensing wireless platform for vibration monitoring and ultrasound detection
- Barometer / environmental monitoring
- Vehicle / goods tracking
- Vibration monitoring
- Compass and inclinometer
- Sensor data logger

(see `Sensing`_ section for the complete lists of available
sensors on board)

.. zephyr:board-supported-hw::

Hardware
********

The STM32L4R9xx devices are an ultra-low-power microcontrollers family (STM32L4+
Series) based on the high-performance Arm® Cortex®-M4 32-bit RISC core.
They operate at a frequency of up to 120 MHz.

More information about STM32L4R9ZI can be found here:

- `STM32L4R9 on www.st.com`_
- `STM32L4+ reference manual`_

Connectivity
************

   - **BlueNRG-M2SA** Bluetooth® low energy v5.0 wireless technology module
     (`BlueNRG-M2 datasheet`_)
   - USB micro-B connector (power supply and data)
   - STDC14 programming connector for **STLINK-V3MINI**
     (`stlink-v3mini`_)
   - microSD card socket
   - STMod+ connector
   - RS485 interface

Sensing
*******

  - **LPS22HH** MEMS pressure sensor
    (`lps22hh datasheet`_)
  - **HTS221** capacitive digital relative humidity and temperature sensor
    (`hts221 datasheet`_)
  - **STTS751** low-voltage digital local temperature sensor
    (`stts751 datasheet`_)
  - **ISM330DHCX** iNEMO IMU, 3D accelerometer and 3D gyroscope with Machine Learning Core
    (`ism330dhcx datasheet`_)
  - **IIS3DWB** ultra-wide bandwidth (up to 6 kHz), low-noise, 3-axis digital vibration sensor
    (`iis3dwb datasheet`_)
  - **IIS2DH** ultra-low-power high performance MEMS motion sensor
    (`iis2dh datasheet`_)
  - **IIS2MDC** 3-axis magnetometer
    (`iis2mdc datasheet`_)
  - **IMP23ABSU** analog MEMS microphone
    (`imp23absu datasheet`_)
  - **IMP34DT05** digital MEMS microphone
    (`imp34dt05 datasheet`_)

Connections and IOs
*******************

- 2x user LEDs

  - **led0** (Green)
  - **led1** (Orange)

- Buttons / switch

  - **User** / **boot0** button, available to user application
    but useful to let the board enter DFU mode
    if found pressed after h/w reset (see **rst** button and
    `Programming and Debugging`_ section)
  - **RESET** button, used to reset the board
  - **PWR** button, used to Power on/off the board


For more details please refer to `STEVAL-STWINKT1B board User Manual`_.

System Clock
------------

STEVAL-STWINKT1B System Clock could be driven by an internal or external oscillator,
as well as the main PLL clock. By default the System clock is driven by the PLL clock at 80MHz,
driven by 16MHz high speed external oscillator.

Serial Port
-----------

The USART2 is connected to the on-board RS485 transceiver / expansion paths
and may be used as console when CDC ACM is disabled.

USB interface
-------------

STEVAL-STWINKT1B can be connected as a USB device to a PC host through its micro-B connector.
The final application may use it to declare STEVAL-STWINKT1B device as belonging to a
certain standard or vendor class, e.g. a CDC, a mass storage or a composite device with both
functions.

Console
-------

There are two possible options for Zephyr console output:


- through common CDC ACM UART backend configuration for all boards

- through USART2. In this case disable the board CDC ACM backend.

  To enable console and shell over UART:

  - in your prj.conf, override the board's default configuration by setting :code:`CONFIG_BOARD_SERIAL_BACKEND_CDC_ACM=n`

  - add an overlay file named ``<board>.overlay``:

.. code-block:: dts

   / {
       chosen {
          zephyr,console = &usart2;
          zephyr,shell-uart = &usart2;
        };
     };


Console default settings are 115200 8N1.

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

There are two alternative methods of flashing STWIN board:

1. Using DFU software tools

   This method requires to enter STM32L4R9 ROM bootloader DFU mode
   by powering up (or reset) the board while keeping the USER (BOOT0) button pressed.
   No additional hardware is required except a USB cable. This method is fully
   supported by :ref:`flash-debug-host-tools`.
   You can read more about how to enable and use the ROM bootloader by checking
   the application note `AN2606`_ (STM32L4R9xx section).

2. Using SWD hardware tools

   The STEVAL-STWINKT1B does not include an on-board debug probe.
   It requires to connect additional hardware, like a ST-LINK/V3
   embedded debug tool, to the board STDC14 connector labeled ``MCU-/SWD``.


Install dfu-util
----------------

.. note::
   Required only to use dfu-util runner.

It is recommended to use at least v0.9 of dfu-util. The package available in
Debian and Ubuntu can be quite old, so you might have to build dfu-util from source.
Information about how to get the source code and how to build it can be found
at the `DFU-UTIL website`_

Install STM32CubeProgrammer
---------------------------

.. note::
   Required to program over DFU (default) or SWD.

It is recommended to use the latest version of `STM32CubeProgrammer`_


Flash an Application to STEVAL-STWINKT1B
----------------------------------------

There are two ways to enter DFU mode:

1. USB cable not connected

   While pressing the USER button, connect the USB cable to the STEVAL-STWINKT1B
   port and to your computer.

2. USB cable connected

   While pressing the USER button, press the RESET button and release it.

With both methods, the board should be forced to enter DFU mode.

Check that the board is indeed in DFU mode:

.. code-block:: console

   $ sudo dfu-util -l
   dfu-util 0.9

   Found DFU: [0483:df11] ver=2200, devnum=..., cfg=1, intf=0, path="...", alt=0,
   name="@Internal Flash  /0x08000000/...", serial="..."

You can build and flash the provided sample application
(:zephyr:code-sample:`stwinkt1b_sensors`) that reads sensors data and outputs
values on the console.

.. _STEVAL-STWINKT1B Development kit website:
   https://www.st.com/en/evaluation-tools/steval-stwinkt1b.html

.. _STEVAL-STWINKT1B board User Manual:
   https://www.st.com/resource/en/user_manual/um2777-how-to-use-the-stevalstwinkt1b-sensortile-wireless-industrial-node-for-condition-monitoring-and-predictive-maintenance-applications-stmicroelectronics.pdf

.. _STM32L4R9 on www.st.com:
   https://www.st.com/en/microcontrollers-microprocessors/stm32l4r9-s9.html

.. _STM32L4+ reference manual:
   https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf

.. _STM32CubeProgrammer:
   https://www.st.com/en/development-tools/stm32cubeprog.html

.. _DFU-UTIL website:
   http://dfu-util.sourceforge.net/

.. _AN2606:
   http://www.st.com/content/ccc/resource/technical/document/application_note/b9/9b/16/3a/12/1e/40/0c/CD00167594.pdf/files/CD00167594.pdf/jcr:content/translations/en.CD00167594.pdf

.. _BlueNRG-M2 datasheet:
   https://www.st.com/en/product/BlueNRG-M2

.. _stlink-v3mini:
   https://www.st.com/en/development-tools/stlink-v3mini.html

.. _lps22hh datasheet:
   https://www.st.com/en/mems-and-sensors/lps22hh.html

.. _hts221 datasheet:
   https://www.st.com/en/mems-and-sensors/hts221.html

.. _stts751 datasheet:
   https://www.st.com/en/mems-and-sensors/stts751.html

.. _ism330dhcx datasheet:
   https://www.st.com/en/mems-and-sensors/ism330dhcx.html

.. _iis3dwb datasheet:
   https://www.st.com/en/mems-and-sensors/iis3dwb.html

.. _iis2dh datasheet:
   https://www.st.com/en/mems-and-sensors/iis2dh.html

.. _iis2mdc datasheet:
   https://www.st.com/en/mems-and-sensors/iis2mdc.html

.. _imp23absu datasheet:
   https://www.st.com/en/mems-and-sensors/imp23absu.html

.. _imp34dt05 datasheet:
   https://www.st.com/en/mems-and-sensors/imp34dt05.html
