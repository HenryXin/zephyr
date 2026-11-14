/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#include <stdio.h>

#ifdef CONFIG_IIS2MDC_TRIGGER
static int iis2mdc_trig_cnt;

static void iis2mdc_trigger_handler(const struct device *dev,
				    const struct sensor_trigger *trig)
{
	sensor_sample_fetch_chan(dev, SENSOR_CHAN_ALL);
	iis2mdc_trig_cnt++;
}
#endif

#ifdef CONFIG_ISM330DHCX_TRIGGER
static int ism330dhcx_acc_trig_cnt;
static int ism330dhcx_gyr_trig_cnt;

static void ism330dhcx_acc_trigger_handler(const struct device *dev,
					   const struct sensor_trigger *trig)
{
	sensor_sample_fetch_chan(dev, SENSOR_CHAN_ACCEL_XYZ);
	ism330dhcx_acc_trig_cnt++;
}

static void ism330dhcx_gyr_trigger_handler(const struct device *dev,
					   const struct sensor_trigger *trig)
{
	sensor_sample_fetch_chan(dev, SENSOR_CHAN_GYRO_XYZ);
	ism330dhcx_gyr_trig_cnt++;
}
#endif

static void stts751_config(const struct device *stts751)
{
	struct sensor_value odr_attr;

	/* set STTS751 sampling frequency to 8 Hz */
	odr_attr.val1 = 8;
	odr_attr.val2 = 0;

	if (sensor_attr_set(stts751, SENSOR_CHAN_AMBIENT_TEMP,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for STTS751\n");
		return;
	}
}

static void iis2dh_config(const struct device *iis2dh)
{
	struct sensor_value odr_attr, fs_attr;

	odr_attr.val1 = 200;
	odr_attr.val2 = 0;

	if (sensor_attr_set(iis2dh, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for IIS2DH accel\n");
		return;
	}

	sensor_g_to_ms2(16, &fs_attr);

	if (sensor_attr_set(iis2dh, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_FULL_SCALE, &fs_attr) < 0) {
		printk("Cannot set full scale for IIS2DH accel\n");
		return;
	}
}

static void iis2mdc_config(const struct device *iis2mdc)
{
	struct sensor_value odr_attr;

	odr_attr.val1 = 100;
	odr_attr.val2 = 0;

	if (sensor_attr_set(iis2mdc, SENSOR_CHAN_ALL,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for IIS2MDC\n");
		return;
	}

#ifdef CONFIG_IIS2MDC_TRIGGER
	struct sensor_trigger trig;

	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_MAGN_XYZ;
	sensor_trigger_set(iis2mdc, &trig, iis2mdc_trigger_handler);
#endif
}

static void ism330dhcx_config(const struct device *ism330dhcx)
{
	struct sensor_value odr_attr, fs_attr;

	odr_attr.val1 = 416;
	odr_attr.val2 = 0;

	if (sensor_attr_set(ism330dhcx, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for ISM330DHCX accel\n");
		return;
	}

	sensor_g_to_ms2(16, &fs_attr);

	if (sensor_attr_set(ism330dhcx, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_FULL_SCALE, &fs_attr) < 0) {
		printk("Cannot set full scale for ISM330DHCX accel\n");
		return;
	}

	odr_attr.val1 = 208;
	odr_attr.val2 = 0;

	if (sensor_attr_set(ism330dhcx, SENSOR_CHAN_GYRO_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for ISM330DHCX gyro\n");
		return;
	}

	sensor_degrees_to_rad(250, &fs_attr);

	if (sensor_attr_set(ism330dhcx, SENSOR_CHAN_GYRO_XYZ,
			    SENSOR_ATTR_FULL_SCALE, &fs_attr) < 0) {
		printk("Cannot set fs for ISM330DHCX gyro\n");
		return;
	}

#ifdef CONFIG_ISM330DHCX_TRIGGER
	struct sensor_trigger trig;

	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_ACCEL_XYZ;
	sensor_trigger_set(ism330dhcx, &trig, ism330dhcx_acc_trigger_handler);

	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_GYRO_XYZ;
	sensor_trigger_set(ism330dhcx, &trig, ism330dhcx_gyr_trigger_handler);
#endif
}

static void lps22hh_config(const struct device *lps22hh)
{
	struct sensor_value odr_attr;

	odr_attr.val1 = 50;
	odr_attr.val2 = 0;

	if (sensor_attr_set(lps22hh, SENSOR_CHAN_ALL,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for LPS22HH\n");
		return;
	}
}

static int led_pattern_out(void)
{
	const struct gpio_dt_spec led0_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
	const struct gpio_dt_spec led1_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
	int i;

	if (!gpio_is_ready_dt(&led0_gpio)) {
		printk("%s: device not ready.\n", led0_gpio.port->name);
		return -1;
	}
	gpio_pin_configure_dt(&led0_gpio, GPIO_OUTPUT_INACTIVE);

	if (!gpio_is_ready_dt(&led1_gpio)) {
		printk("%s: device not ready.\n", led1_gpio.port->name);
		return -1;
	}
	gpio_pin_configure_dt(&led1_gpio, GPIO_OUTPUT_INACTIVE);

	for (i = 0; i < 8; i++) {
		gpio_pin_set_dt(&led0_gpio, ((i % 2) == 0) ? 1 : 0);
		gpio_pin_set_dt(&led1_gpio, ((i % 2) == 1) ? 1 : 0);
		k_msleep(100);
	}

	for (i = 0; i < 6; i++) {
		gpio_pin_set_dt(&led0_gpio, ((i % 2) == 0) ? 1 : 0);
		gpio_pin_set_dt(&led1_gpio, ((i % 2) == 0) ? 1 : 0);
		k_msleep(250);
	}

	gpio_pin_set_dt(&led0_gpio, 0);
	gpio_pin_set_dt(&led1_gpio, 0);

	return 0;
}

int main(void)
{
	int cnt = 1;

	if (led_pattern_out() < 0) {
		return -1;
	}

	printk("STWIN board sensor test\n");

	const struct device *const stts751 = DEVICE_DT_GET_ONE(st_stts751);
	const struct device *const iis2mdc = DEVICE_DT_GET_ONE(st_iis2mdc);
	const struct device *const ism330dhcx = DEVICE_DT_GET_ONE(st_ism330dhcx);
	const struct device *const iis2dh = DEVICE_DT_GET_ONE(st_iis2dh);
	const struct device *const lps22hh = DEVICE_DT_GET_ONE(st_lps22hh);
	const struct device *const hts221 = DEVICE_DT_GET_ONE(st_hts221);

	if (!device_is_ready(stts751)) {
		printk("%s: device not ready.\n", stts751->name);
		return 0;
	}
	if (!device_is_ready(iis2mdc)) {
		printk("%s: device not ready.\n", iis2mdc->name);
		return 0;
	}
	if (!device_is_ready(ism330dhcx)) {
		printk("%s: device not ready.\n", ism330dhcx->name);
		return 0;
	}
	if (!device_is_ready(iis2dh)) {
		printk("%s: device not ready.\n", iis2dh->name);
		return 0;
	}
	if (!device_is_ready(lps22hh)) {
		printk("%s: device not ready.\n", lps22hh->name);
		return 0;
	}
	if (!device_is_ready(hts221)) {
		printk("%s: device not ready.\n", hts221->name);
		return 0;
	}

	stts751_config(stts751);
	iis2mdc_config(iis2mdc);
	ism330dhcx_config(ism330dhcx);
	iis2dh_config(iis2dh);
	lps22hh_config(lps22hh);

	while (1) {
		struct sensor_value stts751_temp;
		struct sensor_value iis2dh_accel[3];
		struct sensor_value iis2mdc_magn[3];
		struct sensor_value iis2mdc_temp;
		struct sensor_value ism330dhcx_accel[3];
		struct sensor_value ism330dhcx_gyro[3];
		struct sensor_value lps22hh_press, lps22hh_temp;
		struct sensor_value hts221_hum, hts221_temp;

		if (sensor_sample_fetch(stts751) < 0) {
			printf("STTS751 Sensor sample update error\n");
			return 0;
		}
#ifndef CONFIG_IIS2MDC_TRIGGER
		if (sensor_sample_fetch(iis2mdc) < 0) {
			printf("IIS2MDC Magn Sensor sample update error\n");
			return 0;
		}
#endif
#ifndef CONFIG_ISM330DHCX_TRIGGER
		if (sensor_sample_fetch(ism330dhcx) < 0) {
			printf("ISM330DHCX IMU Sensor sample update error\n");
			return 0;
		}
#endif
		if (sensor_sample_fetch(iis2dh) < 0) {
			printf("IIS2DH Sensor sample update error\n");
			return 0;
		}
		if (sensor_sample_fetch(lps22hh) < 0) {
			printf("LPS22HH Sensor sample update error\n");
			return 0;
		}
		if (sensor_sample_fetch(hts221) < 0) {
			printf("HTS221 Sensor sample update error\n");
			return 0;
		}

		sensor_channel_get(stts751, SENSOR_CHAN_AMBIENT_TEMP, &stts751_temp);
		sensor_channel_get(iis2dh, SENSOR_CHAN_ACCEL_XYZ, iis2dh_accel);
		sensor_channel_get(iis2mdc, SENSOR_CHAN_MAGN_XYZ, iis2mdc_magn);
		sensor_channel_get(iis2mdc, SENSOR_CHAN_DIE_TEMP, &iis2mdc_temp);
		sensor_channel_get(ism330dhcx, SENSOR_CHAN_ACCEL_XYZ, ism330dhcx_accel);
		sensor_channel_get(ism330dhcx, SENSOR_CHAN_GYRO_XYZ, ism330dhcx_gyro);
		sensor_channel_get(lps22hh, SENSOR_CHAN_AMBIENT_TEMP, &lps22hh_temp);
		sensor_channel_get(lps22hh, SENSOR_CHAN_PRESS, &lps22hh_press);
		sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &hts221_temp);
		sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &hts221_hum);

		printf("\0033\014");

		printf("STWIN dashboard\n\n");

		printf("STTS751: Temperature: %.1f C\n",
		       sensor_value_to_double(&stts751_temp));

		printf("IIS2DH: Accel (m/s^2): x: %.3f, y: %.3f, z: %.3f\n",
		       sensor_value_to_double(&iis2dh_accel[0]),
		       sensor_value_to_double(&iis2dh_accel[1]),
		       sensor_value_to_double(&iis2dh_accel[2]));

		printf("IIS2MDC: Magn (G): x: %.3f, y: %.3f, z: %.3f\n",
		       sensor_value_to_double(&iis2mdc_magn[0]),
		       sensor_value_to_double(&iis2mdc_magn[1]),
		       sensor_value_to_double(&iis2mdc_magn[2]));

		printf("IIS2MDC: Temperature: %.1f C\n",
		       sensor_value_to_double(&iis2mdc_temp));

		printf("ISM330DHCX: Accel (m/s^2): x: %.3f, y: %.3f, z: %.3f\n",
		       sensor_value_to_double(&ism330dhcx_accel[0]),
		       sensor_value_to_double(&ism330dhcx_accel[1]),
		       sensor_value_to_double(&ism330dhcx_accel[2]));

		printf("ISM330DHCX: Gyro (rad/s): x: %.3f, y: %.3f, z: %.3f\n",
		       sensor_value_to_double(&ism330dhcx_gyro[0]),
		       sensor_value_to_double(&ism330dhcx_gyro[1]),
		       sensor_value_to_double(&ism330dhcx_gyro[2]));

		printf("LPS22HH: Temperature: %.1f C\n",
		       sensor_value_to_double(&lps22hh_temp));

		printf("LPS22HH: Pressure: %.3f kPa\n",
		       sensor_value_to_double(&lps22hh_press));

		printf("HTS221: Temperature: %.1f C\n",
		       sensor_value_to_double(&hts221_temp));

		printf("HTS221: Relative Humidity: %.1f%%\n",
		       sensor_value_to_double(&hts221_hum));

#if defined(CONFIG_IIS2MDC_TRIGGER)
		printk("%d:: iis2mdc trig %d\n", cnt, iis2mdc_trig_cnt);
#endif

#ifdef CONFIG_ISM330DHCX_TRIGGER
		printk("%d:: ism330dhcx acc trig %d\n", cnt, ism330dhcx_acc_trig_cnt);
		printk("%d:: ism330dhcx gyr trig %d\n", cnt, ism330dhcx_gyr_trig_cnt);
#endif

		cnt++;
		k_msleep(2000);
	}
}
