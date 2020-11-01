cmake_minimum_required(VERSION 3.13.1)

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
#set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD arduino_nano_33_ble_sense)

# Add out of tree drivers
# list(APPEND ZEPHYR_EXTRA_MODULES
#   ${CMAKE_CURRENT_SOURCE_DIR}/drivers/sensor/lsm9ds1
# )

# This sample is only compatible with arduino_nano_33_ble_sense because
# it has re-directed the 'boards' directory to a directory that only
# contains this board.
# set(BOARD arduino_nano_33_ble_sense)

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE}) #find_package(Zephyr)
project(sensor_test)

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
FILE(GLOB_RECURSE app_sources src/*.c)
target_sources(app PRIVATE ${app_sources}) #target_sources(app PRIVATE src/main.c)
