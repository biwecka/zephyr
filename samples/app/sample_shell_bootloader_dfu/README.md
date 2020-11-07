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

# Initial firmware installation
1.  Build the **sample_shell_bootloader_dfu** as described above.
2.  With a command line opened in the sample's directory execute:  
    `west flash --hex-file build/zephyr/bootloader.hex` and  
    `west flash --hex-file build/zephyr/firmware-signed.hex`.  
    After these commands the bootloader and the firmware are installed on the
    Arduino Nano 33 BLE Sense.
3.  To verify the installation connect to the serial connection:  
    `picocom /dev/ttyACM0 -b 115200`  
    Alternatives to `picocom` are for example `minicom`, `cu` or `screen`.
    On Windows you can use `PuTTY`.

# Updating through DFU
Updating the firmware is currently only possible via Linux because Windows 10
does not provide the necessary drivers.

1.  Connect the Arduino Nano 33 BLE Sense via USB with your Linux computer.
    You can also disconnect any debugger/programmer you may have connected
    to verify the update process is acutally done via the
    Arduino's USB port.
2.  Execute:  
    `dfu-util -l`  
    This command lists all conncted devices that are DFU-capable.
    The Arduino Nano 33 BLE Sense should appear in this list (maybe with
    a cryptic name; usually there is only one device in the list).
    The Arduino appearing as a DFU-capable device is a requirement for
    performing the firmware update.
3.  Execute:  
    `dfu-util --alt 1 --download build/zephyr/update-signed.bin`  
    *alt 1* refers to the partition/slot the upadte is written to. 
    *download* specifies that an update is performed and not a backup of
    an already installed image (for this case there is a *upload* flag).  
    After the command exits sucessfully the old firmware is running in
    the slot0 partition and the updated firmware is saved in slot1.
    The Board needs to be restarted for the bootloader to perform the swap
    of these two partitions.
4.  Restart the board. You should notice the board taking a second to start
    up. This is because the MCUboot bootloader performs an image swap on the
    fist startup after an update. This swap is made persistent if the
    updated firmware image confirms itself against the bootloader
    (it does so with the `boot_write_img_confirmed();` function call in the
    code).
5.  Connect to the serial port and verify that the new version is running
    (e.g. by accessing newly introduced features).
6.  Restart the board and again connect to the serial port. Check if the
    update was persisted by trying to access new features again.

See [Zephyr's documentation](https://docs.zephyrproject.org/1.12.0/samples/subsys/usb/dfu/README.html)
for further information.