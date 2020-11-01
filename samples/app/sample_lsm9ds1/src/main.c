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

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// u8_t mpu_vals[100];
// static struct device* dev_lsm9ds1;

// // Sensor data
// static void sensor_update(void)
// {

//     float accel[3], gyro[3], mag[3], gyro_rad[3], q[4];
//     float temp = 0.0f;
//     float time_stamp = 0.0f;
    
//     struct lsm9ds1_api* dev_api = (struct lsm9ds1_api *)dev_lsm9ds1->driver_api;
//     dev_api->sample_fetch(dev_lsm9ds1);
    
//     dev_api->channel_get(dev_lsm9ds1, SENSOR_CHAN_ACCEL_XYZ, accel);
//     dev_api->channel_get(dev_lsm9ds1, SENSOR_CHAN_GYRO_XYZ,  gyro);
//     dev_api->channel_get(dev_lsm9ds1, SENSOR_CHAN_MAGN_XYZ,  mag);
//     dev_api->channel_get(dev_lsm9ds1, SENSOR_CHAN_AMBIENT_TEMP,  &temp);
    
//     time_stamp = k_uptime_get() * 1.0f;
    
    
//     for(int i = 0; i < 3; i ++){
//         gyro_rad[i] = gyro[i] * 3.1415926535 / 180;
//     }

//     MadgwickAHRSupdate(gyro_rad[0], gyro_rad[1], gyro_rad[2],
//                        accel[0], accel[1], accel[2],
//                        mag[0], mag[1], mag[2]);

//     printf("acc:   x: %.6f    y: %.6f    z: %.6f\n", accel[0], accel[1], accel[2]);
//     printf("gyr:   x: %.6f    y: %.6f    z: %.6f\n", gyro[0], gyro[1], gyro[2]);
//     printf("mag:   x: %.6f    y: %.6f    z: %.6f\n", mag[0], mag[1], mag[2]);
//     printf("temp:     %.2f\n", temp);
//     // printf("qua:   x: %.6f    y: %.6f    z: %.6f    w: %.6f\n", qua.x, qua.y, qua.z, qua.w);

//     q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;

//     memcpy(&mpu_vals[sizeof(float)*0], accel, sizeof(accel));
//     memcpy(&mpu_vals[sizeof(float)*3], gyro, sizeof(gyro));
//     memcpy(&mpu_vals[sizeof(float)*6], mag, sizeof(mag));
//     memcpy(&mpu_vals[sizeof(float)*9], q, sizeof(q));
//     memcpy(&mpu_vals[sizeof(float)*13], &temp, sizeof(temp));
//     memcpy(&mpu_vals[sizeof(float)*14], &time_stamp, sizeof(time_stamp));

//     printf("temp:     %.2f\n", temp);
//     printf("stamp:     %.1f\n", time_stamp);

// }

////////////////////////////////////////////////////////////////////////////////

// static void received_cb(struct bt_conn *conn,
//                           const u8_t *const data, u16_t len)
// {

//     if(len > 1){
//         return;
//     }
    
//     u8_t value = 0;
//     memcpy(&value, data, sizeof(u8_t));
    
//     lsm9ds1_perform performance;

//     switch(value){
//        case 0:
//          performance = LOW;
//          break;
//        case 1:
//          performance = MID;
//          break;
//        case 2:
//          performance = HIGH;
//          break;
//        default:
//          performance = LOW;
//          break;
//     }

//     struct lsm9ds1_api* dev_api = (struct lsm9ds1_api *)dev_lsm9ds1->driver_api;
//     dev_api->sensor_performance(dev_lsm9ds1, performance);
// }

////////////////////////////////////////////////////////////////////////////////

// static void sensor_init(void)
// {
//     dev_lsm9ds1 = device_get_binding("LSM9DS1");

//     struct lsm9ds1_api* dev_api = (struct lsm9ds1_api *)dev_lsm9ds1->driver_api;
//     dev_api->init_done(dev_lsm9ds1);
//     printk("TADAAAAAAAAAAAAAAAAAAAAAAAAA\n");
// }

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
    const struct device *lsm9ds1_dev = device_get_binding(
        DT_LABEL(DT_INST(0, st_lsm9ds1))
    );

    if (lsm9ds1_dev == NULL) {
        printk("lsm9ds1_dev == NULL; => Programm abgebrochen.\n");
    }

    printk("Calibration ACCL - Start\n");
    sensor_sample_fetch_chan(lsm9ds1_dev, SENSOR_CHAN_CALIBRATE_GYRO);
    printk("Calibration ACCL - End\n");

    k_msleep(300);

    ////////////////////////////////////////////////////////////////////////////
    bool led_is_on = true;

    while (1) {
        gpio_pin_set(
            led_builtin_dev,
            DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios),
            (int)led_is_on
        );

        led_is_on = !led_is_on;
        k_msleep(1000);

        // Sensor values - START
        if (sensor_sample_fetch(lsm9ds1_dev) < 0) {
            printf("Sensor sample update error\n");
            return;
        }

        struct sensor_value data[3];
        sensor_channel_get(lsm9ds1_dev, SENSOR_CHAN_GYRO_XYZ, data);

        printf("%f | %f | %f\n",
            sensor_value_to_double(data),
            sensor_value_to_double(data+1),
            sensor_value_to_double(data+2)
        );

        // Sensor values - END
    }
    ////////////////////////////////////////////////////////////////////////////
}
