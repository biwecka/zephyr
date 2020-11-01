/*
 * Copyright (c) 2020 Christopher Funk
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <errno.h>
#include <init.h>

static int power_pins_init(const struct device *dev) {
	ARG_UNUSED(dev);

	#if defined(CONFIG_I2C)
    /* Turn on power to I2C pullup resistors */
	const struct device *i2c_pullup_dev = NULL;
	int i2c_pullup_ret = 0;
	i2c_pullup_dev = device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(pullup_pwr), gpios));
	if (i2c_pullup_dev == NULL) 
	{
		return EIO;
	}
	i2c_pullup_ret = gpio_pin_configure(i2c_pullup_dev, DT_GPIO_PIN(DT_NODELABEL(pullup_pwr), gpios), GPIO_OUTPUT_HIGH);
	if (i2c_pullup_ret < 0) 
	{
		return i2c_pullup_ret;
	}
	#endif

	#if defined(CONFIG_APDS9960) || defined(CONFIG_HTS221) || defined(CONFIG_LPS22HB) || defined(CONFIG_LSM9DS1)
    /* Turn on power to APDS9960, HTS221, and LPS22HB */
	const struct device *vdd_env_dev = NULL;
	int vdd_env_ret = 0;
	vdd_env_dev = device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(vdd_env), gpios));
	if (vdd_env_dev == NULL) 
	{
		return EIO;
	}
	vdd_env_ret = gpio_pin_configure(vdd_env_dev, DT_GPIO_PIN(DT_NODELABEL(vdd_env), gpios), GPIO_OUTPUT_HIGH | GPIO_DS_ALT_HIGH);
	if (vdd_env_ret < 0) 
	{
		return vdd_env_ret;
	}
	#endif

	#if defined(CONFIG_MP34DT05) || defined(CONFIG_PDM)
	/* Turn on power to MP34DT05 */
	struct device *mic_pwr_dev = NULL;
	int mic_pwr_ret = 0;
	mic_pwr_dev = device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(mic_pwr), gpios));
	if (mic_pwr_dev == NULL) 
	{
		return EIO;
	}
	mic_pwr_ret = gpio_pin_configure(mic_pwr_dev, DT_GPIO_PIN(DT_NODELABEL(mic_pwr), gpios), GPIO_OUTPUT_HIGH | GPIO_DS_ALT_HIGH);
	if (mic_pwr_ret < 0) 
	{
		return mic_pwr_ret;
	}
	#endif

	return 0;
}

SYS_INIT(power_pins_init, POST_KERNEL, CONFIG_SENSOR_POWER_PINS_INIT_PRIORITY);
