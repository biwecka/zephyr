#ifndef MY_SHELL_H_
#define MY_SHELL_H_

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <stdio.h>

// For "shell_print", "SHELL_CMD_ARG_REGISTER"
#include <shell/shell.h>

// For "KERNEL_VERSION_STRING"
//#include <version.h>

// For "strcmp"
#include <string.h>

// For the global GS state struct.
#include "../state/state.h"

////////////////////////////////////////////////////////////////////////////////
// Functions
/**
 * Everything is implemented in shell.c.
 * The shell.h does only need to be imported.
 * It doesn't have any public/external functions.
 */

////////////////////////////////////////////////////////////////////////////////
# endif /* MY_SHELL_H_ */