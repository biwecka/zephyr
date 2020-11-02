# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(BOARD arduino_nano_33_ble_sense)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(my-shell)

FILE(GLOB_RECURSE app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
