#include "shell.h"

////////////////////////////////////////////////////////////////////////////////
// "del_history" command
static int cmd_del_history(const struct shell *shell, size_t argc, char **argv) {
    // Delete history
    shell_history_purge(shell->history);

    // Print information
    shell_print(shell,"History was deleted.\n");

    return 0;
}

// Register "del_history"
SHELL_CMD_ARG_REGISTER(
    _del_history,
    NULL,
    "Command for deleting the history. Use if history contains a password.",
    cmd_del_history,
    1,
    0
);

////////////////////////////////////////////////////////////////////////////////
// "setpwd" command
static int cmd_setpwd(const struct shell *shell, size_t argc, char **argv) {
    // Print information
    shell_print(
        shell,
        "Functionality not implemented yet.\nOutput serves demonstration purposes only."
    );
    shell_print(shell, "");

    // Print all arguments
    for (size_t cnt = 0; cnt < argc; cnt++) {
        shell_print(shell, "argv[%d] = %s", cnt, argv[cnt]);
    }

    shell_print(shell, "");
    return 0;
}

// Register "setpwd" command
SHELL_CMD_ARG_REGISTER(
    _setpwd,
    NULL,
    "Command for setting the application password.",
    cmd_setpwd,
    2,
    0
);

////////////////////////////////////////////////////////////////////////////////
// "init" command
static int cmd_set_led_speed(const struct shell *shell, size_t argc, char **argv) {
    // Print old state
    shell_print(shell, "Old state: %d", get_led_is_fast());

    // Change state
    char accept_fast[] = "fast";
    char accept_slow[] = "slow";

    if (strcmp(argv[1], accept_fast) == 0) {
        set_led_is_fast(true);
    } else if (strcmp(argv[1], accept_slow) == 0) {
        set_led_is_fast(false);
    } else {
        shell_print(shell, "Invalid parameter (valid: \"fast\", \"slow\")");
    }

    // Print new state
    shell_print(shell, "New state: %d", get_led_is_fast());

    shell_print(shell, "");
    return 0;
}

// Register "init" command
SHELL_CMD_ARG_REGISTER(
    _set_led_speed,
    NULL,
    "Command for initializing the sensor node.",
    cmd_set_led_speed,
    2,
    0
);

////////////////////////////////////////////////////////////////////////////////
// "_sensor_accel"

// Create variables for thread
// K_THREAD_STACK_DEFINE(accel_thread_area, ACCEL_THREAD_STACK_SIZE);
// struct k_thread accel_thread_data;

// Driver includes
#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>

// Include LSM9DS1 header for enums.
#include <drivers/sensor/lsm9ds1.h>

// Define command
static int cmd_sensor_accel(const struct shell *shell, size_t argc, char **argv) {
    // Convert "samples" argument from char to int
    int samples = atoi(argv[1]);

    // Print samples
    shell_print(shell, "-----------------------------------------------------");
    shell_print(shell, "Fetching samples: %d", samples);
    shell_print(shell, "-----------------------------------------------------");

    // Resutls (3-times the length of "samples" for X,Y,Z values)
    double results[samples * 3];

    const struct device *lsm9ds1_dev = device_get_binding(
        "LSM9DS1"//DT_LABEL(DT_INST(0, st_lsm9ds1)) //"LSM9DS1"
    );

    if (lsm9ds1_dev == NULL) {
        printk("[ERROR] LSM9DS1 was not found.\n");
        return 1;
    } else {
        printk("Alles top.\n");
    }

    printk("Calibration started ...\n");
    //sensor_sample_fetch_chan(lsm9ds1_dev, SENSOR_CHAN_CALIBRATE_ACCL);
    sensor_attr_set(lsm9ds1_dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_CALIB_TARGET, NULL);
    printk("Calibrated\n");



    // Create variables with the correct data types
    //int *num_samples = _num_samples;
    //int *results = _results;

    // Do stuff...
    for (int i = 0; i < samples; i++) {
        // Get
        struct sensor_value data[3];
        sensor_sample_fetch(lsm9ds1_dev);
        sensor_channel_get(lsm9ds1_dev, SENSOR_CHAN_ACCEL_XYZ, data);

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

    printk("Thread created.\n");

    printk("Wait for thread to finish.\n");
    // Wait for thread
    //k_thread_join(&accel_thread_data, K_SECONDS(60));

    printk("Thread finished.\n");

    // Set thread id to NULL to get rid of compiler warning that it is not used.
    //accel_tid = NULL;

    // Print results
    // for (int i = 0; i < samples; i++) {
    //     // "demultiplex" dimensions
    //     int x_index = i * 3 + 0;
    //     int y_index = i * 3 + 1;
    //     int z_index = i * 3 + 2;

    //     double x = results[x_index];
    //     double y = results[y_index];
    //     double z = results[z_index];

    //     printf("%f\t\t%f\t\t%f\n", x, y, z);
    //     k_msleep(100);
    // }

    shell_print(shell, "-----------------------------------------------------");

    return 0;
}

// Register "_sensor_accel"
SHELL_CMD_ARG_REGISTER(
    _sensor_accel,
    NULL,
    "Get data of accelerometer",
    cmd_sensor_accel,
    2,
    0
);

////////////////////////////////////////////////////////////////////////////////
// "version" command
// static int cmd_version(const struct shell *shell, size_t argc, char **argv) {
// 	ARG_UNUSED(argc);
// 	ARG_UNUSED(argv);

// 	shell_print(shell, "Zephyr version %s\n", KERNEL_VERSION_STRING);

// 	return 0;
// }

// // Register "version" command
// SHELL_CMD_ARG_REGISTER(
//     version,                    // Command
//     NULL,                       // Subcommand
//     "Show kernel version",      // Helptext
//     cmd_version,                // Handler
//     1,                          // Parameter count (command itself is first parameter)
//     0                           // Optional parameter count
// );

////////////////////////////////////////////////////////////////////////////////