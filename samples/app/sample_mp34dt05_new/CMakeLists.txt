cmake_minimum_required(VERSION 3.13.1)

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
#set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD arduino_nano_33_ble_sense)

# # Find Zephyr. This also loads Zephyr's build system.
# find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE}) #find_package(Zephyr)
# project(sensor_test)

# # Add your source file to the "app" target. This must come after
# # find_package(Zephyr) which defines the target.
# #target_sources(app PRIVATE src/main.c src/MadgwickAHRS.c)
# FILE(GLOB_RECURSE app_sources src/*.c)
# target_sources(app PRIVATE ${app_sources})

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(thingy52_nrf52832_pdm)

# Add your source file to the "app" target. This must come after
# the boilerplate code, which defines the target.
target_sources(app PRIVATE src/usb/usb.c)
target_sources(app PRIVATE src/main.c)
