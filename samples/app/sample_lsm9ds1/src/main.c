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
#include <string.h>

#include <drivers/sensor/lsm9ds1.h>

//#include "MadgwickAHRS.h"

#include "usb/usb.h"

////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    init_usb(false);

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
    // Get LSM9DS1 and calibrate it
    const struct device *lsm9ds1_dev = device_get_binding(
        DT_LABEL(DT_INST(0, st_lsm9ds1))
    );

    if (lsm9ds1_dev == NULL) {
        printk("lsm9ds1_dev == NULL; => Programm abgebrochen.\n");
    }

    // Trigger sensor calibration
    sensor_attr_set(
        lsm9ds1_dev,
        SENSOR_CHAN_ACCEL_XYZ,
        SENSOR_ATTR_CALIB_TARGET,
        NULL
    );

    k_msleep(100);

    ////////////////////////////////////////////////////////////////////////////
    //Turn on orange LED
    const struct device *led_builtin_dev = device_get_binding(
        DT_GPIO_LABEL(DT_NODELABEL(led_builtin), gpios)
    );
    if (led_builtin_dev == NULL) {
        return;
    }
    ret = gpio_pin_configure(
        led_builtin_dev,
        DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios), GPIO_OUTPUT_HIGH //GPIO_OUTPUT_LOW
    );
    if (ret < 0) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    while (1) {
        // Fetch values from sensor into the driver
        if (sensor_sample_fetch_chan(lsm9ds1_dev, SENSOR_CHAN_ACCEL_XYZ) < 0) {
            printf("Sensor sample update error\n");
            return;
        }

        // Read the fetched values from the driver
        struct sensor_value data[3];
        sensor_channel_get(lsm9ds1_dev, SENSOR_CHAN_ACCEL_XYZ, data);

        // Print out the values
        printf("%f\t\t%f\t\t%f\n",
            sensor_value_to_double(data),
            sensor_value_to_double(data+1),
            sensor_value_to_double(data+2)
        );

        // Wait before getting new values
        k_msleep(50);
    }
    ////////////////////////////////////////////////////////////////////////////
}
