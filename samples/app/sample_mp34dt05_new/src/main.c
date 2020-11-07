/*
* Copyright (c) 2016 Intel Corporation
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <sys/printk.h>

#include <drivers/audio/pdm.h>

#include "usb/usb.h"

#define THINGY52_MIC_PWR_PIN 9
#define MIC_PWR_ON 1
#define MAX_ITERATIONS 4
#define MAX_SAMPLES 100
#define PDM_BUFFER_SIZE CONFIG_PDM_BUFFER_SIZE
#define MEM_SLAB_BLOCKS 4

#define PDM_NODE   DT_NODELABEL(pdm)
#define PDM_LABEL  DT_LABEL(PDM_NODE)
#define GPIO_NODE  DT_NODELABEL(sx1509b)
#define GPIO_LABEL DT_LABEL(GPIO_NODE)

static struct device *pdm_dev;
static struct k_mem_slab pdm_mem_slab;
static __aligned(4) int16_t pdm_buffers[PDM_BUFFER_SIZE][MEM_SLAB_BLOCKS];
static uint16_t samples, iterations;


static void pdm_data_handler(int16_t *data, uint16_t data_size)
{
	samples++;
	if (samples == MAX_SAMPLES) {
		samples = 0;
		iterations++;
		printk("[ITERATION %u/%d] Received %d samples of size %u\n",
			iterations, MAX_ITERATIONS, MAX_SAMPLES, data_size);
		int result = pdm_stop(pdm_dev);

		if (result != 0) {
			printk("Failed to start PDM sampling, error %d\n",
				result);
			return;
		}
		if (iterations != MAX_ITERATIONS) {
			result = pdm_start(pdm_dev);
			if (result != 0) {
				printk("Failed to start PDM sampling, error %d\n",
					result);
				return;
			}
		} else {
			printk("Program finished.\n");
		}
	}
	k_mem_slab_free(&pdm_mem_slab, (void *)&data);
}

static struct pdm_config pdm_cfg = {
	.data_handler	= pdm_data_handler,
	.mem_slab	= &pdm_mem_slab,
};

void main(void)
{
    init_usb(true);

    int ret;

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

    int result;

	samples = 0;
	iterations = 0;

	result = k_mem_slab_init(
        &pdm_mem_slab,
        &pdm_buffers,
	    sizeof(int16_t) * PDM_BUFFER_SIZE,
		MEM_SLAB_BLOCKS
    );

	if (result != 0) {
		printk("Failed to initialize the mem slab! Error %d\n",
			result);
		return result;
	}

    pdm_dev = device_get_binding(PDM_LABEL);
	if (!pdm_dev) {
		printk("Cannot find %s!\n", PDM_LABEL);
		return -ENOENT;
	}
	printk("Successfully bound PDM driver.\n");

    result = pdm_configure(pdm_dev, &pdm_cfg);
	if (result != 0) {
		printk("Failed to set the data handler, error %d\n", result);
		return result;
	}

	result = pdm_start(pdm_dev);
	if (result != 0) {
		printk("Failed to start PDM sampling, error %d\n", result);
		return result;
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

    // int result;

	// samples = 0;
	// iterations = 0;

    // printk("%d\n", CLK_PIN);

	// pdm_dev = device_get_binding(DT_NODELABEL(pdm));
	// if (!pdm_dev) {
	// 	printk("Cannot find %s!\n", pdm);
	// 	return -ENOENT;
	// }
	// printk("Successfully bound PDM driver.\n");

    //dmic_trigger(pdm_dev, DMIC_TRIGGER_START);

    //k_msleep(2000);

    //dmic_trigger(pdm_dev, DMIC_TRIGGER_STOP);

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

    // k_msleep(300);

    // ////////////////////////////////////////////////////////////////////////////
    // bool led_is_on = true;

    // while (1) {
    //     gpio_pin_set(
    //         led_builtin_dev,
    //         DT_GPIO_PIN(DT_NODELABEL(led_builtin), gpios),
    //         (int)led_is_on
    //     );

    //     led_is_on = !led_is_on;
    //     k_msleep(1000);

    //     // Sensor values - START
    //     // nrfx_pdm_start();
    //     // printk("[loop] nrfx_pdm_enable_check() => %d\n", nrfx_pdm_enable_check());

    //     // k_msleep(2000);

    //     // nrfx_pdm_stop();

    //     // Sensor values - END
    // }
    ////////////////////////////////////////////////////////////////////////////

    return 0;
}
