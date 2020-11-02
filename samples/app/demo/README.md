# Description
The firmware of this sample provides a "shell" over UART (USB CDC ACM)
and is a DFU-capable device which is accessible via "dfu-util" on Linux.

# Building this sample
## Using Windows + Git Bash
-   Make sure you can execute `python` in Git Bash.
    If it throws an error (e.g. permission denied) try the following:
    -   Windows-Search "Aliase für App-Ausführung verwalten" or
        "App execution aliases"
    -   Disable "python.exe" and leave "python3.exe" enabled.
    -   Open Git Bash and try execution `python --version` for example.
        If it works you'r good to go.

-   Execute the `build-firmware.sh` script in the this directory.
    After it finishes you should have these files in the
    `build/zephyr` directory:
    -   bootloader.hex: The bootloader image
    -   firmware.hex: The firmware image (for initial flash with bootloader)
    -   update.bin: The firmware as an update image (to be flashed via DFU)

## Using Windows + CMD
Just execute the `build-firmware.cmd` via CMD.

After it finishes you should have these files in the
    `build/zephyr` directory:
    -   bootloader.hex: The bootloader image
    -   firmware.hex: The firmware image (for initial flash with bootloader)
    -   update.bin: The firmware as an update image (to be flashed via DFU)

# Updating through DFU
Updating the firmware is currently not possible with Windows 10 because
of driver issues/lack of drivers.

Updating through DFU works fine on Linux though.
See [Zephyr's documentation](https://docs.zephyrproject.org/1.12.0/samples/subsys/usb/dfu/README.html)
for the exact commands.