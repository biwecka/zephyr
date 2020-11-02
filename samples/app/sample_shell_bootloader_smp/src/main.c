/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <string.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include "state/state.h"
#include "shell/shell.h"
#include "usb/usb.h"

////////////////////////////////////////////////////////////////////////////////
// SMP Server
#include <stats/stats.h>

#ifdef CONFIG_MCUMGR_CMD_FS_MGMT
#include <device.h>
#include <fs/fs.h>
#include "fs_mgmt/fs_mgmt.h"
#include <fs/littlefs.h>
#endif
#ifdef CONFIG_MCUMGR_CMD_OS_MGMT
#include "os_mgmt/os_mgmt.h"
#endif
#ifdef CONFIG_MCUMGR_CMD_IMG_MGMT
#include "img_mgmt/img_mgmt.h"
#endif
#ifdef CONFIG_MCUMGR_CMD_STAT_MGMT
#include "stat_mgmt/stat_mgmt.h"
#endif

#define LOG_LEVEL LOG_LEVEL_DBG
#include <logging/log.h>
LOG_MODULE_REGISTER(smp_sample);

//#include "common.h"

/* Define an example stats group; approximates seconds since boot. */
STATS_SECT_START(smp_svr_stats)
STATS_SECT_ENTRY(ticks)
STATS_SECT_END;

/* Assign a name to the `ticks` stat. */
STATS_NAME_START(smp_svr_stats)
STATS_NAME(smp_svr_stats, ticks)
STATS_NAME_END(smp_svr_stats);

/* Define an instance of the stats group. */
STATS_SECT_DECL(smp_svr_stats) smp_svr_stats;

#ifdef CONFIG_MCUMGR_CMD_FS_MGMT
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(cstorage);
static struct fs_mount_t littlefs_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &cstorage,
	.storage_dev = (void *)FLASH_AREA_ID(storage),
	.mnt_point = "/lfs"
};
#endif
////////////////////////////////////////////////////////////////////////////////


/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif

void main(void)
{
	int rc = STATS_INIT_AND_REG(smp_svr_stats, STATS_SIZE_32,
				"smp_svr_stats");

	if (rc < 0) {
		LOG_ERR("Error initializing stats system [%d]", rc);
	}

	/* Register the built-in mcumgr command handlers. */
	#ifdef CONFIG_MCUMGR_CMD_FS_MGMT
		rc = fs_mount(&littlefs_mnt);
		if (rc < 0) {
			LOG_ERR("Error mounting littlefs [%d]", rc);
		}

		fs_mgmt_register_group();
	#endif
	#ifdef CONFIG_MCUMGR_CMD_OS_MGMT
		os_mgmt_register_group();
	#endif
	#ifdef CONFIG_MCUMGR_CMD_IMG_MGMT
		img_mgmt_register_group();
	#endif
	#ifdef CONFIG_MCUMGR_CMD_STAT_MGMT
		stat_mgmt_register_group();
	#endif
	#ifdef CONFIG_MCUMGR_SMP_BT
		start_smp_bluetooth();
	#endif
	#ifdef CONFIG_MCUMGR_SMP_UDP
		start_smp_udp();
	#endif

	if (IS_ENABLED(CONFIG_USB)) {
		rc = usb_enable(NULL);
		if (rc) {
			LOG_ERR("Failed to enable USB");
			return;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//init_usb(false);

	bool led_is_on = true;
	int ret;

	const struct device *dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	while (1) {
		gpio_pin_set(dev, PIN, (int)led_is_on);
		led_is_on = !led_is_on;

		if (get_led_is_fast()) {
			k_msleep(100);
		} else {
			k_msleep(1000);
		}
	}
}
