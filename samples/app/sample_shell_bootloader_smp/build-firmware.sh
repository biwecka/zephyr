#!/bin/bash

echo "-------------------------------------------------------------------------"
echo "Build application:"
echo "------------------"
echo ""

west build

echo ""
echo "-------------------------------------------------------------------------"
echo "Copy and rename files:"
echo "----------------------"
echo ""

# go to build directory
cd build/zephyr

echo "Copy: zephyr.hex -> firmware.hex"
cp zephyr.hex firmware.hex

echo "Copy: zephyr.bin -> update.bin"
cp zephyr.bin update.bin

echo ""

echo "Copy: firmware.hex -> /bootloader/mcuboot-custom/"
cp firmware.hex ../../../../../../bootloader/mcuboot-custom/

echo "Copy: update.bin -> /bootloader/mcuboot-custom/"
cp update.bin ../../../../../../bootloader/mcuboot-custom/

# exit build directory
cd ../../

# now we're in the application directory again

echo ""
echo "-------------------------------------------------------------------------"
echo "Build bootloader:"
echo "-----------------"
echo ""

# go to bootloader directory
cd ../../../../bootloader/mcuboot-custom/

# go to boot/zephyr -> build -> return from subdirectories
cd boot/zephyr
west build -b arduino_nano_33_ble_sense
cd ../..

echo ""
echo "-------------------------------------------------------------------------"
echo "Rename bootloader's HEX file and copy it to application's build folder:"
echo "-----------------------------------------------------------------------"
echo ""

cd boot/zephyr/build/zephyr/
echo "Copy: zephyr.hex -> application/build/zephyr/bootloader.hex"
cp zephyr.hex ../../../../../../zephyr/samples/app/sample_shell_bootloader_smp/build/zephyr/bootloader.hex
cd ../../../../

echo ""
echo "-------------------------------------------------------------------------"
echo "Sign firmware.hex and update.bin and move them to app's build dir:"
echo "------------------------------------------------------------------"
echo ""

echo "Signing images..."
python scripts/imgtool.py sign --key root-rsa-2048.pem --header-size 0x200 --align 8 --version 1.2 --slot-size 0x60000 firmware.hex firmware-signed.hex
python scripts/imgtool.py sign --key root-rsa-2048.pem --header-size 0x200 --align 8 --version 1.2 --slot-size 0x60000 update.bin update-signed.bin

echo ""
echo "Removing unsigned images from bootloader directory..."
rm firmware.hex
rm update.bin

echo ""
echo "Move signed images back to application's build directory..."
mv firmware-signed.hex ../../zephyr/samples/app/sample_shell_bootloader_smp/build/zephyr
mv update-signed.bin ../../zephyr/samples/app/sample_shell_bootloader_smp/build/zephyr

echo ""
echo "-------------------------------------------------------------------------"
echo "DONE !!!"
echo "--------"
echo ""
