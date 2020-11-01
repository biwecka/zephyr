/*
* Copyright (c) 2016 Intel Corporation
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <sys/printk.h>
#include <stdio.h>

#include "usb/usb.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void process_sample(const struct device *dev)
{
	static unsigned int obs;
	struct sensor_value temp, hum;
	if (sensor_sample_fetch(dev) < 0) {
		printf("Sensor sample update error\n");
		return;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
		printf("Cannot read HTS221 temperature channel\n");
		return;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum) < 0) {
		printf("Cannot read HTS221 humidity channel\n");
		return;
	}

	++obs;
	printf("Observation:%u\n", obs);

	/* display temperature */
	printf("Temperature:%.1f C\n", sensor_value_to_double(&temp));

	/* display humidity */
	printf("Relative Humidity:%.1f%%\n",
	       sensor_value_to_double(&hum));
}

static void hts221_handler(const struct device *dev,
			   struct sensor_trigger *trig)
{
	process_sample(dev);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    init_usb(true);

    int ret = 0;

    ////////////////////////////////////////////////////////////////////////////
    // Turn on led_pwr
    const struct device *led_pwr_dev = device_get_binding(
        DT_GPIO_LABEL(DT_NODELABEL(led_pwr), gpios)
    );
    if (led_pwr_dev == NULL) {
        return;
    }
    ret = gpio_pin_configure(
        led_pwr_dev,
        DT_GPIO_PIN(DT_NODELABEL(led_pwr), gpios), GPIO_OUTPUT_HIGH
    );
    if (ret < 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Turn off led_builtin
    const struct device *led_builtin_dev = device_get_binding(
        DT_GPIO_LABEL(DT_NODELABEL(led_builtin), gpios)
    );
    if (led_builtin_dev == NULL) {
        return;
    }
    ret = gpio_pin_configure(
        led_builtin_dev,
        DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios), GPIO_OUTPUT_LOW
    );
    if (ret < 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Get ... device
    const struct device *dev = device_get_binding("HTS221");

	if (dev == NULL) {
		printf("Could not get HTS221 device\n");
		return;
	}

	if (IS_ENABLED(CONFIG_HTS221_TRIGGER)) {
		struct sensor_trigger trig = {
			.type = SENSOR_TRIG_DATA_READY,
			.chan = SENSOR_CHAN_ALL,
		};
		if (sensor_trigger_set(dev, &trig, hts221_handler) < 0) {
			printf("Cannot configure trigger\n");
			return;
		};
	}

    ////////////////////////////////////////////////////////////////////////////
    bool led_is_on = true;

    while (1) {
        gpio_pin_set(
            led_builtin_dev,
            DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios),
            (int)led_is_on
        );

        led_is_on = !led_is_on;
        k_msleep(SLEEP_TIME_MS);

        // Sensor values - START
        process_sample(dev);
        // Sensor values - END
    }
    ////////////////////////////////////////////////////////////////////////////
}
