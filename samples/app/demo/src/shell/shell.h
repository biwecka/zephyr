#ifndef MY_SHELL_H_
#define MY_SHELL_H_

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <stdio.h>

// For "atoi" (converts strings to integers)
#include <stdlib.h>

// For "shell_print", "SHELL_CMD_ARG_REGISTER"
#include <shell/shell.h>

// For "KERNEL_VERSION_STRING"
//#include <version.h>

// For "strcmp"
#include <string.h>

// For the global GS state struct.
#include "../state/state.h"

// Provides thread entry point function for fetching accelerometer data.
#include "../sensors/accel.h"

// Driver includes
#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>

// Include LSM9DS1 header for enums.
#include <drivers/sensor/lsm9ds1.h>

////////////////////////////////////////////////////////////////////////////////
// Functions
/**
 * Everything is implemented in shell.c.
 * The shell.h does only need to be imported.
 * It doesn't have any public/external functions.
 */

////////////////////////////////////////////////////////////////////////////////
# endif /* MY_SHELL_H_ */