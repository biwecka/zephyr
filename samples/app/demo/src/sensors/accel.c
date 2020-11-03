////////////////////////////////////////////////////////////////////////////////
// Includes
#include "accel.h"

// Defaults
#include <zephyr.h>
#include <sys/printk.h>
#include <stdio.h>

#include <drivers/gpio.h>

// Driver includes
#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>

// Include LSM9DS1 header for enums.
#include <drivers/sensor/lsm9ds1.h>

////////////////////////////////////////////////////////////////////////////////
// Get LSM9DS1


////////////////////////////////////////////////////////////////////////////////
// Thread entry point
void accel_entry_point(void *_num_samples, void *_results, void *_v3) {
    // const struct device *pin = NULL;
    // pin = device_get_binding(DT_GPIO_LABEL(DT_NODELABEL(vdd_env), gpios));
    // if (pin == NULL) {
    //     printk("Pin nicht gefunden.\n");
    // } else {
    //     printk("Pin da!\n");
    // }

    // int x = gpio_pin_get_raw(pin, DT_GPIO_PIN(DT_NODELABEL(vdd_env), gpios));
    // printk(">>> %d\n", x);

    printk("1\n");
    const struct device *lsm9ds1_dev = device_get_binding(
        DT_LABEL(DT_INST(0, st_lsm9ds1)) //"LSM9DS1"
    );
    printk("1\n");

    if (lsm9ds1_dev == NULL) {
        printk("[ERROR] LSM9DS1 was not found.\n");
    } else {
        printk("Alles top.\n");
    }
    printk("Calibration started ...\n");
    sensor_sample_fetch_chan(lsm9ds1_dev, SENSOR_CHAN_CALIBRATE_ACCL);
    printk("Calibrated\n");



    // Create variables with the correct data types
    int *num_samples = _num_samples;
    int *results = _results;

    // Do stuff...
    for (int i = 0; i < *num_samples; i++) {
        // Get
        struct sensor_value data[3];
        sensor_sample_fetch(lsm9ds1_dev);
        sensor_channel_get(lsm9ds1_dev, SENSOR_CHAN_ACCL_XYZ, data);

        // // X-val
        // results[i * 3 + 0] = sensor_value_to_double(data); //i + 100;

        // // Y-val
        // results[i * 3 + 1] = sensor_value_to_double(data+1); //i + 200;

        // // Z-val
        // results[i * 3 + 2] = sensor_value_to_double(data+2); //i + 300;

        printf("%d \t\t %f \t\t %f \t\t %f\n",
            i,
            sensor_value_to_double(data),
            sensor_value_to_double(data+1),
            sensor_value_to_double(data+2)
        );

        k_msleep(100);
    }

    return;
}
////////////////////////////////////////////////////////////////////////////////