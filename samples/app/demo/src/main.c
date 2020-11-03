/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <string.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <drivers/sensor.h>
#include <drivers/sensor/lsm9ds1.h>

#include "state/state.h"
#include "shell/shell.h"
#include "usb/usb.h"


/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif

void main(void)
{
	init_usb(false);

	bool led_is_on = true;
	int ret;

	const struct device *dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	////////////////////
	// const struct device *lsm9ds1_dev = device_get_binding(
    //     DT_LABEL(DT_INST(0, st_lsm9ds1)) //"LSM9DS1"
    // );

    // if (lsm9ds1_dev == NULL) {
    //     printk("[ERROR] LSM9DS1 was not found.\n");
    // } else {
    //     printk("Alles top.\n");
    // }
    // printk("Calibration started ...\n");
    // sensor_sample_fetch_chan(lsm9ds1_dev, SENSOR_CHAN_CALIBRATE_GYRO);
    // printk("Calibrated\n");
	//////////////////////

	while (1) {
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;

		// struct sensor_value data[3];
        // sensor_sample_fetch(lsm9ds1_dev);
        // sensor_channel_get(lsm9ds1_dev, SENSOR_CHAN_GYRO_XYZ, data);
		// printf("%f | %f | %f\n",
        //     sensor_value_to_double(data),
        //     sensor_value_to_double(data+1),
        //     sensor_value_to_double(data+2)
        // );

		if (get_led_is_fast()) {
			k_msleep(100);
		} else {
			k_msleep(1000);
		}
	}
}
