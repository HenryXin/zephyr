.. zephyr:code-sample:: stwinkt1b_wifi
   :name: STWIN Wi-Fi (ISM43362)
   :relevant-api: net_stats

   Exercise the STEVAL-STWINWFV1 Inventek ISM43362 Wi-Fi expansion with the Wi-Fi shell.

Overview
********

This sample enables the optional STEVAL-STWINWFV1 Wi-Fi expansion board on the
STWIN (STEVAL-STWINKT1B) CN3 connector and provides the Wi-Fi and networking
shell commands (``wifi scan``, ``wifi connect``, ``net``) to exercise the
Inventek ISM43362-M3G-L44-E module over SPI1.

Requirements
************

- :zephyr:board:`steval_stwinkt1b`
- STEVAL-STWINWFV1 Wi-Fi expansion plugged into CN3
- Default CN3 solder bridges routing SPI1 (PG2/PG3/PG4) to the connector

References
**********

- :zephyr:board:`steval_stwinkt1b`
- `STEVAL-STWINWFV1`_

Building and Running
********************

Build and flash the sample:

.. zephyr-app-commands::
    :zephyr-app: samples/boards/st/board_specific/steval_stwinkt1b/wifi
    :board: steval_stwinkt1b
    :goals: build flash

Flashing requires the preliminary steps described in
:zephyr:board:`steval_stwinkt1b`.

Open a serial console on the USB CDC ACM port (115200 8N1), then use the Wi-Fi
shell:

.. code-block:: console

   uart:~$ wifi scan
   Scan requested
   uart:~$
   Num  | SSID                             (len) | Chan | RSSI | Sec
   ...
   ----------
   Scan request done

   uart:~$ wifi connect --ssid "my_ssid" --key-mgmt 1 --passphrase "secret"
   Connection requested
   uart:~$
   Connected

.. _STEVAL-STWINWFV1:
   https://www.st.com/en/evaluation-tools/steval-stwinwfv1.html
