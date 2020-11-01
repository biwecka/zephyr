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

#include "usb/usb.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

#ifdef CONFIG_APDS9960_TRIGGER
K_SEM_DEFINE(sem, 0, 1);

static void trigger_handler(struct device *dev, struct sensor_trigger *trigger)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(trigger);

	k_sem_give(&sem);
}
#endif

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
    // Get APDS9960 device
    const struct device *dev;
	struct sensor_value intensity, pdata;

	printk("APDS9960 sample application\n");
	dev = device_get_binding(DT_LABEL(DT_INST(0, avago_apds9960)));
	if (!dev) {
		printk("sensor: device not found.\n");
		return;
	}

#ifdef CONFIG_APDS9960_TRIGGER
	struct sensor_value attr = {
		.val1 = 127,
		.val2 = 0,
	};

	if (sensor_attr_set(dev, SENSOR_CHAN_PROX,
			    SENSOR_ATTR_UPPER_THRESH, &attr)) {
		printk("Could not set threshold\n");
		return;
	}

	struct sensor_trigger trig = {
		.type = SENSOR_TRIG_THRESHOLD,
		.chan = SENSOR_CHAN_PROX,
	};

	if (sensor_trigger_set(dev, &trig, trigger_handler)) {
		printk("Could not set trigger\n");
		return;
	}
#endif

    ////////////////////////////////////////////////////////////////////////////
    bool led_is_on = true;

    while (1) {
        gpio_pin_set(
            led_builtin_dev,
            DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios),
            (int)led_is_on
        );

        led_is_on = !led_is_on;
        //k_msleep(SLEEP_TIME_MS);

        // Sensor values - START
#ifdef CONFIG_APDS9960_TRIGGER
		printk("Waiting for a threshold event\n");
		k_sem_take(&sem, K_FOREVER);
#else
		k_sleep(K_MSEC(1000));
#endif
		if (sensor_sample_fetch(dev)) {
			printk("sensor_sample fetch failed\n");
		}

		sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &intensity);
		sensor_channel_get(dev, SENSOR_CHAN_PROX, &pdata);

		printk("ambient light intensity %d, proximity %d\n",
		       intensity.val1, pdata.val1);

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
		u32_t p_state;

		p_state = DEVICE_PM_LOW_POWER_STATE;
		device_set_power_state(dev, p_state, NULL, NULL);
		printk("set low power state for 2s\n");
		k_sleep(K_MSEC(100));
		p_state = DEVICE_PM_ACTIVE_STATE;
		device_set_power_state(dev, p_state, NULL, NULL);
#endif
        // Sensor values - END
    }
    ////////////////////////////////////////////////////////////////////////////
}
