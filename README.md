
# QuokKM5
QuokKM5 is an USB keyboard and mouse to 5-pin non-ADB NeXT input interface. It bridges
 a USB keyboard and mouse to a NeXT machine via a Soundbox or NeXT monitor as an input alternative 
 to a native NeXT keyboard and mouse.

# How to use
 1. Attach any USB keyboards/mice to your QuokKM5.
 2. The QuokKM5 can be powered two ways. From the NeXT machine or via the USB-C port.
    - When powered by the USB-C port, the button assigned to power on the keyboard can turn on the NeXT machine.
    - When powered via the NeXT machine, the power button on the QuokKM5 must be used to turn on the NeXT machine.
 3. Once the power is turned on LEDs on the QuokkADB will blink once for power and then blink for all devices found on the USB bus.

# Background
The code is largely based upon the [QuokkABD](https://github.com/rabbitholecomputing/QuokkADB-firmware) and uses the same micro processor. The QuokkADB inturn uses a large amount of code from [adbuino](https://github.com/akuker/adbuino) project.

# Mouse Wheel
The mouse wheel only works on some mice that support mouse wheel when the mouse in boot protocol mode.

# Special Key-combos
The special keyboard commands is CTRL + SHIFT + CAPS LOCK and a letter key.
There is an alternate keyboard command, CTRL + CMD + OPTION and a letter key, if the previous key combination isn't compatible with the keyboard.

Example: CTRL + SHIFT + CAPS LOCK + V ghost types the firmware version.

The letter keys are as follows:

 - P - Ghost types the current settings and a list of the special keys
 - V - Ghost types firmware version
 - K - Swap the Alt key with the Command key positions - blinks thrice
 - L - Led status toggle on/off. This controls whether the LED blinks or remains off for light sensitive situations
 - + - Increases the sensitivity of the mouse - blinks twice
 - - - Decreases the sensitivity of the mouse - blinks once
 - ] - Increases the number of times the arrow key is typed for each movement of the scroll wheel- blinks twice
 - [ - Decreases the number of times the arrow is type. If negative, multiple scroll movements are required to type the arrow key
 - W - Invert the scroll axis - blinks thrice 
 - S - Saves the current settings to flash so they stick on future power ups
 - R - Restore factory settings and clear the settings in flash

# Upgrading firmware
Download the latest `.uf2` firmware from here:
https://github.com/rabbitholecomputing/QuokKM5-firmware/releases

 1. Switch the QuokKM5 to USB device mode
 2. Hold down the Bootloader button down and plug in a USB cable to the USB-C port
 3. On your computer a drive labeled `RPI-RP2` should show up
 4. Let go of the Bootloader button
 5. Copy the `.uf2` firmware file to the `RPI-RP2` drive
 6. Once it is finished copying, unplug the USB-C cable and switch the board back to USB host mode
 7. The board should now be updated and read to use with your NeXT machine
