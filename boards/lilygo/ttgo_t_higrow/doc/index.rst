.. zephyr:board:: ttgo_t_higrow

Overview
********

LILYGO® TTGO T-Higrow is an ESP32 plant-monitoring stake with a capacitive soil
probe, ambient light sensor, and either a DHT11 or BME280 environment sensor.
This board package targets the **DHT11** variant.

It features the following integrated components:

- ESP32 (Xtensa LX6 dual-core, Wi-Fi, Bluetooth)
- 4MB flash
- USB Type-C with USB-UART bridge (CH9102 on current SKUs)
- Capacitive soil moisture (GPIO32) and fertility/salt (GPIO34) ADCs
- Battery voltage ADC (GPIO33)
- DHT11 on GPIO16
- BH1750 ambient light on I2C (SDA GPIO25, SCL GPIO26)
- Sensor power enable on GPIO4
- BOOT (GPIO0) and USER/Wake (GPIO35) buttons
- Optional JST battery connector

Hardware
********

Pin assignments follow LilyGO's TTGO-HiGrow ``configuration.h`` and the DHT11
pin diagram on the product page.

.. include:: ../../../espressif/common/soc-esp32-features.rst
   :start-after: espressif-soc-esp32-features

Supported Features
==================

.. zephyr:board-supported-hw::

System Requirements
*******************

.. include:: ../../../espressif/common/system-requirements.rst
   :start-after: espressif-system-requirements

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

.. include:: ../../../espressif/common/building-flashing.rst
   :start-after: espressif-building-flashing

.. include:: ../../../espressif/common/board-variants.rst
   :start-after: espressif-board-variants

Debugging
=========

.. include:: ../../../espressif/common/openocd-debugging.rst
   :start-after: espressif-openocd-debugging

Sample Applications
*******************

Build hello_world:

.. zephyr-app-commands::
   :tool: west
   :app: samples/hello_world
   :board: ttgo_t_higrow/esp32/procpu
   :goals: build

There is no evidenced onboard user LED suitable for the blinky sample.

References
**********

.. target-notes::

.. _`LilyGO T-Higrow product page`: https://lilygo.cc/products/t-higrow
.. _`LilyGO TTGO-HiGrow GitHub`: https://github.com/Xinyuan-LilyGO/TTGO-HiGrow
