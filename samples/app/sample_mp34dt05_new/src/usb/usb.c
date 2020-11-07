////////////////////////////////////////////////////////////////////////////////
// Includes

#include "usb.h"

////////////////////////////////////////////////////////////////////////////////

int init_usb(bool wait) {
    if (wait) {
        // Get uart_console device.
        const struct device *uart_console = device_get_binding(
            CONFIG_UART_CONSOLE_ON_DEV_NAME
        );

        // Check if device was found.
        if (uart_console == NULL) { return 1;}

        // Enable USB
        if (usb_enable(NULL)) { return 2;}

        // Poll if the DTR (Data Terminal Ready) flag was set.
        uint32_t dtr = 0;
        while (!dtr) {
            uart_line_ctrl_get(uart_console, UART_LINE_CTRL_DTR, &dtr);
        }

        // ...
        if (strlen(CONFIG_UART_CONSOLE_ON_DEV_NAME) != strlen("CDC_ACM_0") ||
	        strncmp(
                CONFIG_UART_CONSOLE_ON_DEV_NAME,
                "CDC_ACM_0",
		        strlen(CONFIG_UART_CONSOLE_ON_DEV_NAME))
            ) {
        ////
		    printk("Error: Console device name is not USB ACM\n");
		    return 3;
	    }

    } else {
        // Enable USB
        if (usb_enable(NULL)) { return 2;}
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////