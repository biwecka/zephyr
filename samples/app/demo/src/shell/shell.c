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
K_THREAD_STACK_DEFINE(accel_thread_area, ACCEL_THREAD_STACK_SIZE);
struct k_thread accel_thread_data;

// Define command
static int cmd_sensor_accel(const struct shell *shell, size_t argc, char **argv) {
    // Convert "samples" argument from char to int
    int samples = atoi(argv[1]);

    // Print samples
    shell_print(shell, "-----------------------------------------------------");
    shell_print(shell, "Fetching samples: %d", samples);
    shell_print(shell, "-----------------------------------------------------");

    // Resutls
    int results[samples];

    // Start thread
    k_tid_t accel_tid = k_thread_create(
        &accel_thread_data,
        accel_thread_area,
        K_THREAD_STACK_SIZEOF(accel_thread_area),
        accel_entry_point,
        &samples, results, NULL,
        ACCEL_THREAD_PRIORITY,
        0,
        K_NO_WAIT
    );

    // Wait for thread
    k_thread_join(&accel_thread_data, K_SECONDS(60));

    // Set thread id to NULL to get rid of compiler warning that it is not used.
    accel_tid = NULL;

    // Print results
    for (int i = 0; i < samples; i++) {
        printk("%d\n", results[i]);
    }

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