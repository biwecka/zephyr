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
//#include <nrfx/drivers/include/nrfx_pdm.h>
//#include <nrfx_pdm.h>
//#include <audio/dmic.h>

// #include "lsm9ds1.h"
// #include "MadgwickAHRS.h"

#include "usb/usb.h"
//#include "pdm.h"

#define MIC_PWR_ON 1
#define MAX_ITERATIONS 1
#define MAX_SAMPLES 50

#define PDM_NODE   DT_NODELABEL(pdm)
#define PDM_LABEL  DT_LABEL(PDM_NODE)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static struct device *pdm_dev;
static uint16_t samples = 0, iterations;

int16_t avg = 0;

// static void pdm_data_handler(int16_t *data, uint16_t data_size)
// {
// 	samples++;

// 	if (samples == MAX_SAMPLES) {
// 		samples = 0;
// 		iterations++;
// 		printk("[ITERATION %u/%d] Received %d samples of size %u\n",
// 			iterations, MAX_ITERATIONS, MAX_SAMPLES, data_size);
// 		int result = pdm_stop(pdm_dev);

// 		if (result != 0) {
// 			printk("Failed to start PDM sampling, error %d\n",
// 				result);
// 			return;
// 		}
// 		if (iterations != MAX_ITERATIONS) {
// 			result = pdm_start(pdm_dev);
// 			if (result != 0) {
// 				printk("Failed to start PDM sampling, error %d\n",
// 					result);
// 				return;
// 			}
// 		} else {
// 			printk("Program finished.\n");
// 		}
// 	}
// }

// #define PDM_BUF_ADDRESS 0x20000000 // buffer address in RAM
// #define PDM_BUF_SIZE 256 //size in 32 bit words --> 40 bytes

// int16_t pdm_buf[PDM_BUF_SIZE];

// void nrfx_pdm_event_handler(nrfx_pdm_evt_t const *const p_evt)
// {
//     printk("PDM Handler called gets executed.\n");

//     if (p_evt->buffer_requested) {
//         nrfx_pdm_buffer_set(pdm_buf, PDM_BUF_SIZE);
//     }
//     if (p_evt->buffer_released != 0) {
//         printk("Out: %.2x %2x\r\n", (uint16_t)pdm_buf[0],
//                 (uint16_t)pdm_buf[1]);
//     }
// }

// #define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
// static void pdm_init(void)
// {
//     printk("NRFX_PDM_ENABLED: %d\n", NRFX_PDM_ENABLED);
//     printk("CLK Pin: %d\n", NRF_GPIO_PIN_MAP(0, 26));
//     printk("DIN Pin: %d\n", NRF_GPIO_PIN_MAP(0, 25));
//     nrfx_pdm_config_t pdm_config = NRFX_PDM_DEFAULT_CONFIG(
//         NRF_GPIO_PIN_MAP(0, 26),
//         NRF_GPIO_PIN_MAP(0, 25)
//     );

//     nrfx_pdm_init(&pdm_config, nrfx_pdm_event_handler);
//     //irq_enable(29);
//     nrfx_pdm_buffer_set(pdm_buf, PDM_BUF_SIZE);
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
    struct device *led_pwr_dev = device_get_binding(
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
    struct device *led_builtin_dev = device_get_binding(
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
    // struct device *mp34dt05_dev = device_get_binding(
    //     DT_LABEL(DT_INST(0, st_mp34dt05))
    // );

    // if (mp34dt05_dev == NULL) {
    //     printk("Mikrofon nicht gefunden. Programmabbruch...\n");
    // } else {
    //     printk("Mikrofon gefunden!\n");
    // }

    // pdm_init();
    // printk("[after init] nrfx_pdm_enable_check() => %d\n", nrfx_pdm_enable_check());

    int result;

	samples = 0;
	iterations = 0;

	pdm_dev = device_get_binding(PDM_LABEL);
	if (!pdm_dev) {
		printk("Cannot find %s!\n", PDM_LABEL);
		return -ENOENT;
	}
	printk("Successfully bound PDM driver.\n");

    dmic_trigger(pdm_dev, DMIC_TRIGGER_START);

    k_msleep(2000);

    dmic_trigger(pdm_dev, DMIC_TRIGGER_STOP);

    // result = pdm_set_data_handler(pdm_dev, pdm_data_handler);
	// if (result != 0) {
	// 	printk("Failed to set the data handler, error %d\n", result);
	// 	return result;
	// }

	// result = pdm_start(pdm_dev);
	// if (result != 0) {
	// 	printk("Failed to start PDM sampling, error %d\n", result);
	// 	return result;
	// }

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
        // nrfx_pdm_start();
        // printk("[loop] nrfx_pdm_enable_check() => %d\n", nrfx_pdm_enable_check());

        // k_msleep(2000);

        // nrfx_pdm_stop();

        // Sensor values - END
    }
    ////////////////////////////////////////////////////////////////////////////
}
