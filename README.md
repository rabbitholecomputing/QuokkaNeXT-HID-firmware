
# QuokkaNeXT HID
QuokkaNeXT HID is an USB keyboard and mouse to 5-pin non-ADB NeXT input interface. It bridges
 a USB keyboard and mouse to a NeXT machine via a Soundbox or NeXT monitor as an input alternative 
 to a native NeXT keyboard and mouse.

# How to use
 1. Attach any USB keyboards/mice to your QuokkaNeXT HID.
 2. The QuokkaNeXT HID can be powered two ways. From the NeXT machine or via the USB-C port.
    - When powered by the USB-C port, the button assigned to power on the keyboard can turn on the NeXT machine.
    - When powered via the NeXT machine, the power button on the QuokkaNeXT HID must be used to turn on the NeXT machine.
 3. Once the power is turned on LEDs on the QuokkaNeXT HID will blink once for power and then blink for all devices found on the USB bus.

# Key mapping
The following keys are mapped to their similar positions on the the NeXT keyboard, with the 'END' key being a macro button to mute.
```
[INS] [HOME] [PGUP]  => [VOLUP]  [PWR]  [BRIGHTUP]
[DEL] [END]  [PGDN]  => [VOLDN]  [MUTE] [BRIGHTDN]
```

The following F keys have also been mapped to the special NeXT keys
 - F1 => Mute
 - F2 => Volume Down (VOLDN)
 - F3 => Volume Up   (VOLUP)
 - F5 and F15 => Power (PWR)
 - F11 => Brightness Down (BRIGHTDN)
 - F12 => Brightness Up   (BRIGHTUP)

Both Tilde/Grave and NumLock act as the NeXT Tilde/Grave key  (~/`)

The Capslock key can be mapped to either a Capslock marco or the NeXT Control Key. See [Special Key-combos](#special-key-combos) below.

The CapsLock and NumLock LEDs have been mapped to left and right Shift key LEDs respectively.

# Background
The code is largely based upon the [QuokkABD](https://github.com/rabbitholecomputing/QuokkADB-firmware) and uses the same micro processor. The QuokkADB inturn uses a large amount of code from [adbuino](https://github.com/akuker/adbuino) project.

# Mouse Wheel
The mouse wheel only works on some mice that support mouse wheel when the mouse in boot protocol mode.

# Special Key-combos
The special keyboard commands is CTRL + SHIFT + CAPS LOCK and a letter key.
There is an alternate keyboard command, CTRL + CMD + OPTION or CTRL + META + ALT and a letter key, if the previous key combination isn't compatible with the keyboard.

Example: CTRL + SHIFT + CAPS LOCK + V ghost types the firmware version.

The letter keys are as follows:

 - P - Ghost types the current settings and a list of the special keys
 - V - Ghost types firmware version
 - K - Swap the Alt key with the Command key positions - blinks thrice
 - T - Toggles whether the Caps Lock key acts as Control or Caps Lock
 - L - LED status toggle on/off. This controls whether the LED blinks or remains off for light sensitive situations
 - + - Increases the sensitivity of the mouse - blinks twice
 - - - Decreases the sensitivity of the mouse - blinks once
 - ] - Increases the number of times the arrow key is typed for each movement of the scroll wheel- blinks twice
 - [ - Decreases the number of times the arrow is typed. If negative, multiple scroll movements are required to type the arrow key
 - W - Invert the scroll axis - blinks thrice 
 - S - Saves the current settings to flash so they stick on future power ups
 - R - Restore factory settings and clear the settings in flash

# Upgrading firmware
Download the latest `.uf2` firmware from here:
https://github.com/rabbitholecomputing/QuokkaNeXT-HID-firmware/releases

 1. Move the switch to "UF2" mode and plug in a USB cable to the USB-C port next to the switch, labeled "USB-C PWR/UF2 ONLY" and the other end into your computer
 2. On your computer a drive labeled `RPI-RP2` should show up
 3. Copy the `.uf2` firmware file to the `RPI-RP2` drive
 4. Once it is finished copying, unplug the USB-C cable and move the switch back to "RUN"
 5. The board should now be updated and ready to use with your NeXT machine
