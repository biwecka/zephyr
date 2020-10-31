# Development of a firmware for wireless sensor nodes
The goal of this project is to provide a software plattform
for implementing a wireless sensor node based on the
Arduino Nano 33 BLE Sense development board.

The project provides:
-   **board definition** for the Arduino Nano 33 BLE Sense to the Zephyr RTOS
-   **drivers** for the four sensors and the digital microphone of the Arduino
    board
-   a **bootloader** (*MCUboot*) for flashing and updating the Arduino's
    frimware through its USB connection
-   **samples** on how to use all these components together
-   **documentation** on hot to use it yourself

# Project structure
This project is managed in two repositories -
one for Zephyr and one for MCUboot.  
The Zephyr repository is a fork of
https://github.com/zephyrproject-rtos/zephyr.  
The MCUboot repository is a fork of
https://github.com/zephyrproject-rtos/mcuboot.


## Zephyr Fork
The Zephyr repository contains the following parts of the firmware:
-   **board definition** in `boards/arm/arduino_nano_33_ble_sense_nrf52840`
-   **drivers** in `drivers/...`
-   **samples** in `samples/applications/...`
-   **documentation** in `README.md` (the file you're reading right now)

To be able to later contribute the board definition and the drivers
to the upstream Zephyr repository the `master`/`main` branch of
this fork should only contain code which is ready to be commited
and therefore is left untouched for now.

For contributing all development done in this project to the upstream
Zephyr repository later each of the above mentioned parts of the firmware
is developed in different branches:
-   **board definition**: `board_arduino_nano_33_ble_sense_nrf52840` branch
-   **drivers**         : `driver_*` branches
-   **samples**         : `firmware` branch
-   **documentation**   : `firmware` branch

## MCUboot Fork
The MCUboot repository contains the following parts of the firmware:
-   **bootloader** in `boot/zephyr`

For better contribution to the upstream MCUboot repository the development
of the custom bootloader configuration for the firmware is done in the
`development_firmware` branch.

## How these two repositories are connected with each other
Zephyr uses a tool called "west" for repository/dependency management.
This tool parses the `west.yml` file in the Zephyr repository
and fetches the dependencies according to the content of this file.

In the `firmware` branch of the Zephyr fork the `west.yml` got modified
to include the MCUboot fork. The code looks like this:
```yml
manifest:
  defaults:
    remote: upstream

  remotes:
    - name: upstream
      url-base: https://github.com/zephyrproject-rtos

    - name: mcuboot-custom
      url-base: https://github.com/biwecka/mcuboot

  projects:
    ...

    - name: mcuboot
      revision: a5d79cf8ccb2c71e68ef32a71d6a2716e831d12e
      path: bootloader/mcuboot

    - name: .
      remote: mcuboot-custom
      revision: 4effd0f61bc7a53e9156034219011120f2b1b82f
      path: bootloader/mcuboot-custom

    ...
```

The first entry under "projects" references Zephyr's original MCUboot version
(which is a fork too).
The second entry points to the custom MCUboot version.
When `west update` is executed both (the original and the custom bootloader)
are fetched and saved in the `bootloader` directory.

# How this project was set up
-   ...
-   ...

# Development Guide

## Making changes to the bootloader
Because of this the workflow of making changes to the bootloader is as follows:
-   Make changes to the bootloader in the MCUboot fork
    (e.g. in the `development_firmware` branch).
-   Commit these changes to the MCUboot repository and copy the hast
    of this commit.
-   Modify the custom bootloader's entry in the `west.yml` in the Zephyr
    repository (e.g. in the `firmware` branch).
    Therefore the previously copied hash (of the commit to the bootloader)
    has to be entered unter the "revision" property of the
    custom MCUboot entry.
-   Execute `west update` to fetch the changes.

After that the changes made to the bootloader are available in the
`bootloader` folder.