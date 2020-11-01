# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BOARD_ARDUINO_NANO_33_BLE_SENSE)
  zephyr_library()
  zephyr_library_sources(power_pins_config.c)
  zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
endif()