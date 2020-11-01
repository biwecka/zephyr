# Development of a firmware for wireless sensor nodes
The goal of this project is to provide a software plattform
for implementing a wireless sensor node based on the
Arduino Nano 33 BLE Sense development board.

The project provides:
-   **board definition** for the Arduino Nano 33 BLE Sense to make it
    compatible with the Zephyr RTOS
-   **drivers** for the four sensors and the digital microphone of the Arduino
    board
-   a **bootloader** (MCUboot) for flashing and updating the Arduino's
    frimware through its USB connection
-   **samples** on how to use all these components together
-   **documentation** on how everything was set up and how to use this
    project yourself

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

## How this project structure is achieved
-   Forked **zephyrproject-rtos/zephyr** and **zephyrproject-rtos/mcuboot**.

-   Cloned the Zephyr repository with "west" and initialize a west workspace:  
    `west init [zephyr-dirname] -m https://github.com/biwecka/zephyr`

-   Created the firmware branch based on a stable version of zephyr:  
    `git checkout zephyr-v2.4.0`  
    `git checkout -b firmware`

-   Modify `west.yml` in the **firmware** branch to include custom version
    of MCUboot.

-   Execute `west update` to pull all the Zephyr dependencies.
    Check if `bootloader/mcuboot-custom` exists afterwards.

-   Clone the MCUboot repository. **Not** in `[zephyr-dirname]`:  
    `git clone https://github.com/biwecka/mcuboot`

Other branches like **board_arduino_nano_33_ble_sense** or **driver_\***
are based on the same branch as the **firmware** branch.

# Development Guide
## Prerequisites
The following hardware is needed:
-   **Arduino Nano 33 BLE Sense** development board
-   Programmer to flash the development board (e.g. **J-Link EDU Mini**)
-   Two Micro-USB to USB (Type A) cables to connect both, the Arduino and
    the programmer, to a computer.

## Set up the development environment
Zephyr has a [getting started guide](https://docs.zephyrproject.org/latest/getting_started/index.html)
on how to install all necessary build tools
for zephyr development.
Following this guide your computer gets set up to build and flash
this project.
The last steps of the guide teach you how to
build and flash a sample application. For these steps you can use
`arduino_nano_33_ble_sense_nrf52840` as board name.

## Creating new branches
Create a new branch for every new feature you want to contribute
to the upstream Zephyr or MCUboot repository.

In this project all of my own branches in the Zephyr repository
(e.g. **board_arduino_nano_33_ble_sense_nrf52840**, **firmware**, **driver_\***)
are based on the stable zephyr branch `zephyr-v2.4.0`.
It is important to not derive branches from the **firmware** branch
because this branch contains code which is not ment to be commited
(like the modified `west.yml` file).

My own branches in the MCUboot fork are based on commit
`a5d79cf8ccb2c71e68ef32a71d6a2716e831d12e` because this is the version of
MCUboot that `zephyr-2.4.0` is using
(this information can be obtained by checking the mcuboot entry in `west.yml`).

New branches can be created like that:
-   Checkout the branch you want the new branch to be based off:  
    `git checkout [branch or commit]`
    
-   Create the new branch:  
    `git checkout -b [new branch name]`


## Making changes to the bootloader
Because of how the Zephyr repository depends on the MCUboot repository the
following workflow is necessary to make changes to the bootloader:
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
`bootloader/mcuboot-custom` folder.

## Build and flash the bootloader and firmware
...

## Update the firmware (over USB)
...