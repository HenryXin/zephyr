.. zephyr:code-sample:: stwinkt1b_sensors
   :name: STWIN sensors

   Read sensor data from the various STWIN SensorTile wireless industrial node sensors.

Overview
********
This sample provides an example of how to read sensors data
from the STWIN board.

This sample enables all sensors of STWIN board, and then
periodically reads and displays data on the console from the following
sensors:

- STTS751: Digital temperature sensor
- IIS2MDC: 3-axis magnetometer
- ISM330DHCX: IMU, 3D accelerometer and 3D gyroscope with Machine Learning Core
- IIS2DH: ultra-low-power high performance 3-axis accelerometer
- LPS22HH: pressure and temperature sensor
- HTS221: humidity and temperature sensor

Requirements
************

The application requires a STWIN board connected to the PC
through USB. The board shows up as a USB CDC class standard device.

References
**********

- :zephyr:board:`steval_stwinkt1b`

Building and Running
********************

Build and flash the sample in the following way:

.. zephyr-app-commands::
    :zephyr-app: samples/boards/st/board_specific/steval_stwinkt1b/sensors
    :board: steval_stwinkt1b
    :goals: build flash

Please note that flashing the board requires a few preliminary steps described
in :zephyr:board:`steval_stwinkt1b`.

Then, power cycle the board by disconnecting and reconnecting the USB cable.
Run your favorite terminal program to listen for output.

.. code-block:: console

   $ minicom -D <tty_device> -b 115200

Replace :code:`<tty_device>` with the correct device path automatically created on
the host after the STWIN board gets connected to it,
usually :code:`/dev/ttyUSBx` or :code:`/dev/ttyACMx` (with x being 0, 1, 2, ...).
The ``-b`` option sets baud rate ignoring the value from config.

Sample Output
=============

The sample code outputs sensors data on the STWIN console.

 .. code-block:: console

    STWIN dashboard

    STTS751: Temperature: 24.4 C
    IIS2DH: Accel (m/s^2): x: -5.590, y: -0.536, z: 8.040
    IIS2MDC: Magn (G): x: 0.420, y: -0.116, z: -0.103
    IIS2MDC: Temperature: 21.0 C
    ISM330DHCX: Accel (m/s^2): x: 0.000, y: 5.704, z: 7.982
    ISM330DHCX: Gyro (rad/s): x: 0.026, y: -0.006, z: -0.008
    LPS22HH: Temperature: 26.4 C
    LPS22HH: Pressure: 100.539 kPa
    HTS221: Temperature: 25.1 C
    HTS221: Relative Humidity: 45.0%

    <repeats endlessly every 2s>
