//----------------------------------------------------------------------------
//
//  QuokkKM5 non-ADB keyboard and mouse adapter
//     Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.
//     Copyright (C) 2022-2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokKM5.
//
//
//  This file is free software: you can redistribute it and/or modify it under
//  the terms of the GNU General Public License as published by the Free
//  Software Foundation, either version 3 of the License, or (at your option)
//  any later version.
//
//  This file is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
//  details.
//
//  You should have received a copy of the GNU General Public License along
//  with this file. If not, see <https://www.gnu.org/licenses/>.
//
//  A majority of this code were originally released under a
//  General Public License version 2 (GPL2) License.
//  See LICENSE in the root of this repository for more info.
//
//---------------------------------------------------------------------------
#include <Arduino.h>
#include "platformkbdparser.h"
#include "usb_hid_keys.h"
#include "platform_config.h"
#include "char2usbkeycode.h"
#include "flashsettings.h"
#include <tusb.h>
#include "platform_logmsg.h"
#include "platform_gpio.h"
#include "blink.h"

#define VALUE_WITHIN(v, l, h) (((v) >= (l)) && ((v) <= (h)))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

extern uint16_t modifierkeys;
extern bool set_hid_report_ready;
extern bool global_debug;

uint8_t inline findModifierKey(hid_keyboard_report_t const *report, const hid_keyboard_modifier_bm_t mod)
{
    return (mod & report->modifier) ? 1 : 0;
}

PlatformKbdParser::PlatformKbdParser()
{
    kbdLockingKeys.bLeds = 0;
}
PlatformKbdParser::~PlatformKbdParser()
{
}

void PlatformKbdParser::AddKeyboard(uint8_t dev_addr, uint8_t instance)
{
    for (size_t i = 0; i < MAX_KEYBOARDS; i++)
    {
        if (!keyboards_list[i].in_use)
        {
            keyboards_list[i].in_use = true;
            keyboards_list[i].device_addr = dev_addr;
            keyboards_list[i].instance = instance;
            keyboards_list[i].supports_leds = true;
            SetUSBkeyboardLEDs(kbdLockingKeys.kbdLeds.bmCapsLock,
                               kbdLockingKeys.kbdLeds.bmNumLock,
                               kbdLockingKeys.kbdLeds.bmScrollLock);
            break;
        }
    }
}
void PlatformKbdParser::RemoveKeyboard(uint8_t dev_addr, uint8_t instance)
{
    for (size_t i = 0; i < MAX_KEYBOARDS; i++)
    {
        if (keyboards_list[i].in_use && keyboards_list[i].device_addr == dev_addr && keyboards_list[i].instance == instance)
        {
            keyboards_list[i].in_use = false;
            break;
        }
    }
}

void PlatformKbdParser::Parse(uint8_t dev_addr, uint8_t instance, hid_keyboard_report_t const *report)
{
    union
    {
        KBDINFO kbdInfo;
        uint8_t bInfo[sizeof(KBDINFO)];
    } current_state;

    KBDINFO *cur_kbd_info = &(current_state.kbdInfo);

    cur_kbd_info->bmLeftCtrl = findModifierKey(report, KEYBOARD_MODIFIER_LEFTCTRL);
    cur_kbd_info->bmLeftShift = findModifierKey(report, KEYBOARD_MODIFIER_LEFTSHIFT);
    cur_kbd_info->bmRightCtrl = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTCTRL);
    cur_kbd_info->bmRightShift = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTSHIFT);

    if (setting_storage.settings()->swap_modifiers)
    {
        cur_kbd_info->bmLeftGUI = findModifierKey(report, KEYBOARD_MODIFIER_LEFTALT);
        cur_kbd_info->bmLeftAlt = findModifierKey(report, KEYBOARD_MODIFIER_LEFTGUI);
        cur_kbd_info->bmRightGUI = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTGUI);
        cur_kbd_info->bmRightAlt = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTALT);
    }
    else
    {
        cur_kbd_info->bmLeftAlt = findModifierKey(report, KEYBOARD_MODIFIER_LEFTALT);
        cur_kbd_info->bmLeftGUI = findModifierKey(report, KEYBOARD_MODIFIER_LEFTGUI);
        cur_kbd_info->bmRightAlt = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTALT);
        cur_kbd_info->bmRightGUI = findModifierKey(report, KEYBOARD_MODIFIER_RIGHTGUI);
    }
    memcpy(cur_kbd_info->Keys, report->keycode, 6);
    cur_kbd_info->bReserved = report->reserved;

    if (PlatformKbdParser::SpecialKeyCombo(cur_kbd_info))
    {
        return;
    }

    // provide event for changed control key state
    if (prevState.bInfo[0x00] != current_state.bInfo[0x00])
    {
        OnControlKeysChanged(prevState.bInfo[0x00], current_state.bInfo[0x00]);
    }

    for (uint8_t i = 2; i < 8; i++)
    {
        bool down = false;
        bool up = false;

        for (uint8_t j = 2; j < 8; j++)
        {
            if (current_state.bInfo[i] == prevState.bInfo[j] && current_state.bInfo[i] != 1)
                down = true;
            if (current_state.bInfo[j] == prevState.bInfo[i] && prevState.bInfo[i] != 1)
                up = true;
        }
        if (!down)
        {
            OnKeyDown(current_state.bInfo[0], current_state.bInfo[i]);
        }
        if (!up)
        {
            OnKeyUp(current_state.bInfo[0], prevState.bInfo[i]);
        }
    }

    // store current buttons to test against next keyboard action
    for (uint8_t i = 0; i < sizeof(prevState.bInfo); i++)
        prevState.bInfo[i] = current_state.bInfo[i];
}
// bool tuh_hid_set_report(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, void* report, uint16_t len);

void PlatformKbdParser::SetUSBkeyboardLEDs(bool capslock, bool numlock, bool scrollock)
{
    // Send LEDs statuses to USB keyboard
    kbdLockingKeys.kbdLeds.bmCapsLock = capslock ? 1 : 0;
    kbdLockingKeys.kbdLeds.bmNumLock = numlock ? 1 : 0;
    kbdLockingKeys.kbdLeds.bmScrollLock = scrollock ? 1 : 0;
    usb_set_leds = true;
}

bool PlatformKbdParser::SpecialKeyCombo(KBDINFO *cur_kbd_info)
{
    // Special keycombo actions
    const char ON_STRING[] = "On";
    const char OFF_STRING[] = "Off";
    uint8_t special_key_count = 0;
    uint8_t special_key = 0;
    uint8_t special_keys[] = {
        USB_KEY_V, USB_KEY_K, USB_KEY_T, USB_KEY_L, USB_KEY_P, 
        USB_KEY_EQUAL, USB_KEY_MINUS, USB_KEY_KPPLUS, USB_KEY_KPMINUS,
        USB_KEY_LEFTBRACE, USB_KEY_RIGHTBRACE, USB_KEY_W, 
        USB_KEY_S, USB_KEY_R
        };
    uint8_t caps_lock_down = false;
    char print_buf[2048];
    for (uint8_t i = 0; i < 6; i++)
    {
        if (cur_kbd_info->Keys[i] == USB_KEY_CAPSLOCK)
        {
            caps_lock_down = true;
        }
        for (size_t j = 0; j < sizeof(special_keys); j++)
        {
            if (special_keys[j] == cur_kbd_info->Keys[i])
            {
                special_key_count++;
                special_key = cur_kbd_info->Keys[i];
            }
        }
    }

    bool enter_shortcut1 = caps_lock_down &&
            (cur_kbd_info->bmLeftCtrl  || cur_kbd_info->bmRightCtrl) &&
            (cur_kbd_info->bmLeftShift   || cur_kbd_info->bmRightShift);

    bool enter_shortcut2 =
             (cur_kbd_info->bmLeftCtrl  || cur_kbd_info->bmRightCtrl) &&
             (cur_kbd_info->bmLeftGUI || cur_kbd_info->bmRightGUI)&&
             (cur_kbd_info->bmLeftAlt   || cur_kbd_info->bmRightAlt);


    if (special_key_count == 1 && (enter_shortcut1 || enter_shortcut2))

    {
        switch (special_key)
        {
        case USB_KEY_V:
            SendString(PLATFORM_FW_VER_STRING);
            break;
        case USB_KEY_P:
            snprintf(print_buf, sizeof(print_buf),
                    "Current Settings\n"
                    "================\n"
                    "Command <-> Alt key swap: %s\n"
                    "CapsLock as Control: %s \n"
                    "LED: %s\n"
                    "Mouse Sensitivity Divisor: %u\n"
                    "(higher = less sensitive)\n"
                    "Mouse wheel count: %d\n"
                    "Flip mouse wheel axis: %s\n"
                    "\n"
                    "Special Keys = CAPS + Ctrl + Shift + (Key)\n"
                    "Alternate Keys = Ctrl + Cmd + Alt + (Key)\n"
                    "------------------------------------------\n"
                    "(V): print firmware version\n"
                    "(S): save settings to flash - LED blinks %d times\n"
                    "(R): remove settings from flash - LED blinks %d times\n"
                    "(K): swap alt and command key positions - LED blinks thrice\n"
                    "(T): switch CapsLock to CapsLock or Control - LED blinks thrice\n"
                    "(L): toggle status LED On/Off\n"
                    "(+): increase sensitivity - LED blinks twice\n"
                    "(-): decrease sensitivity - LED blink once\n"
                    "\n"
                    "Change mouse wheel count 'x' by one with '[' or ']'\n"
                    "If positive press the up/down arrow 'x' times for each wheel movement\n"
                    "If negative divide the mouse wheel movement by 'abs(x)'\n"
                    "(]): increase the mouse wheel count - LED blinks twice\n"
                    "([): decrease the mouse wheel count - LED blink once\n"
                    "(W): flip mouse wheel axis - LED blinks thrice\n"
                    "Note: not all mice support the mouse wheel in HID boot protocol\n"
                    ,
                    setting_storage.settings()->swap_modifiers ? ON_STRING : OFF_STRING,
                    setting_storage.settings()->caps_as_control ? ON_STRING : OFF_STRING,
                    setting_storage.settings()->led_enabled ? ON_STRING : OFF_STRING,
                    setting_storage.settings()->sensitivity_divisor,
                    setting_storage.settings()->mouse_wheel_count,
                    setting_storage.settings()->swap_mouse_wheel_axis ? ON_STRING : OFF_STRING,
                    SAVE_TO_FLASH_BLINK_COUNT,
                    CLEAR_FLASH_BLINK_COUNT);
            SendString(print_buf);
            break;
        case USB_KEY_S:
            setting_storage.save();
            blink_led.blink(SAVE_TO_FLASH_BLINK_COUNT);
            break;
        case USB_KEY_R:
            setting_storage.clear();
            blink_led.blink(CLEAR_FLASH_BLINK_COUNT);
            break;
        case USB_KEY_K:
            setting_storage.settings()->swap_modifiers ^= 1;
            blink_led.blink(3);
            break;
        case USB_KEY_T:
            setting_storage.settings()->caps_as_control ^= 1;
            blink_led.blink(3);
            break;
        case USB_KEY_L:
            setting_storage.settings()->led_enabled ^= 1;
            break;
        case USB_KEY_KPPLUS:
        case USB_KEY_EQUAL:
            if (setting_storage.settings()->sensitivity_divisor <= 1)
                setting_storage.settings()->sensitivity_divisor = 1;
            else
                setting_storage.settings()->sensitivity_divisor--;
            blink_led.blink(2);
            break;
        case USB_KEY_KPMINUS:
        case USB_KEY_MINUS:
            if (setting_storage.settings()->sensitivity_divisor >= 32)
                setting_storage.settings()->sensitivity_divisor = 32;
            else
                setting_storage.settings()->sensitivity_divisor++;
            blink_led.blink(1);
            break;
        case USB_KEY_LEFTBRACE:
            if (setting_storage.settings()->mouse_wheel_count <= -8)
            {
                setting_storage.settings()->mouse_wheel_count = -8;
            }
            else
            {
                setting_storage.settings()->mouse_wheel_count--;
                blink_led.blink(1);
            }
            break;
        case USB_KEY_RIGHTBRACE:
            if (setting_storage.settings()->mouse_wheel_count >= 10)
            {
                setting_storage.settings()->mouse_wheel_count = 10;
            }
            else
            {
                setting_storage.settings()->mouse_wheel_count++;
                blink_led.blink(2);
            }
            break;
        case USB_KEY_W:
            setting_storage.settings()->swap_mouse_wheel_axis ^= 1;
            blink_led.blink(3);
            break;
        }

        return true;
    }
    return false;
}

void PlatformKbdParser::SendString(const char *message)
{
    int i = 0;
    usbkey_t key;

    // force key up on modifier keys
    while (PendingKeyboardEvent());
    OnKeyUp(0, USB_KEY_LEFTSHIFT);
    OnKeyUp(0, USB_KEY_RIGHTSHIFT);
    OnKeyUp(0, USB_KEY_LEFTCTRL);
    OnKeyUp(0, USB_KEY_RIGHTCTRL);
    OnKeyUp(0, USB_KEY_LEFTALT);
    OnKeyUp(0, USB_KEY_RIGHTALT);
    OnKeyUp(0, USB_KEY_CAPSLOCK);
    OnKeyUp(0, USB_KEY_LEFTMETA);
    OnKeyUp(0, USB_KEY_RIGHTMETA);
    
    while (message[i] != '\0')
    {
        uint8_t mod = 0;
        while (PendingKeyboardEvent());

        key = char_to_usb_keycode(message[i++]);

        if (key.shift_down)
        {
            mod = USB_KEY_MOD_LSHIFT;
            OnKeyDown(mod, USB_KEY_LEFTSHIFT);
        }

        OnKeyDown(mod, key.keycode);

        OnKeyUp(mod, key.keycode);

        if (key.shift_down)
        {
            mod = 0;
            OnKeyUp(mod, USB_KEY_LEFTSHIFT);
        }
    }
}
void PlatformKbdParser::SendCapsLock()
{
    uint8_t mod = 0;
    while (PendingKeyboardEvent());
    OnKeyUp(0, USB_KEY_LEFTSHIFT);
    OnKeyUp(0, USB_KEY_RIGHTSHIFT);
    OnKeyUp(0, USB_KEY_LEFTCTRL);
    OnKeyUp(0, USB_KEY_RIGHTCTRL);
    OnKeyUp(0, USB_KEY_LEFTALT);
    OnKeyUp(0, USB_KEY_RIGHTALT);
    OnKeyUp(0, USB_KEY_CAPSLOCK);
    OnKeyUp(0, USB_KEY_LEFTMETA);
    OnKeyUp(0, USB_KEY_RIGHTMETA);

    while (PendingKeyboardEvent());
    if (setting_storage.settings()->swap_modifiers)
    {
        mod = USB_KEY_MOD_LMETA;
        OnKeyDown(mod, USB_KEY_LEFTMETA);
    }
    else
    {
        mod = USB_KEY_MOD_LALT;
        OnKeyDown(mod, USB_KEY_LEFTALT);
    }
    mod |= USB_KEY_MOD_LSHIFT;
    OnKeyDown(mod, USB_KEY_LEFTSHIFT);
    mod &= ~ USB_KEY_MOD_LSHIFT;
    OnKeyUp(mod, USB_KEY_LEFTSHIFT);
    if (setting_storage.settings()->swap_modifiers)
        OnKeyUp(0, USB_KEY_LEFTMETA);
    else
        OnKeyUp(0, USB_KEY_LEFTALT);
}
void PlatformKbdParser::ChangeUSBKeyboardLEDs(void)
{
    const uint8_t max_retries = 3;
    const uint32_t report_ready_timeout = 3000;
    if (usb_set_leds == false)
        return;

    size_t i = 0;
    static uint8_t usb_kbd_leds = 0;
    // USB HID Keyboard LED bit location 0x1 - numlock, 0x2 - capslock, 0x4 - scrollock
    usb_kbd_leds = kbdLockingKeys.kbdLeds.bmNumLock ? 0x1 : 0;
    usb_kbd_leds |= kbdLockingKeys.kbdLeds.bmCapsLock ? 0x2 : 0;
    usb_kbd_leds |= kbdLockingKeys.kbdLeds.bmScrollLock ? 0x4 : 0;
    uint8_t try_again = 0;
    bool ready_to_report = false;

    while(true)
    {
        uint32_t start = millis();
        while ((uint8_t)(millis() - start) < report_ready_timeout)
        {
            if (set_hid_report_ready)
            {
                ready_to_report = true;
                set_hid_report_ready = false;
                break;
            }
            else
            {
                tuh_task();
            }
        }

        if (ready_to_report)
        {
                if (!tuh_hid_set_report(
                keyboards_list[i].device_addr,
                keyboards_list[i].instance,
                0,
                HID_REPORT_TYPE_OUTPUT,
                &(usb_kbd_leds),
                sizeof(usb_kbd_leds)
                ))
            {
                // USB disconnected
                keyboards_list[i].supports_leds = false;
                set_hid_report_ready = true;
                try_again++;
            }
            else
            {
                ready_to_report = false;
                try_again = max_retries;
            }

        }
        else
        {
            ready_to_report = false;
            try_again = max_retries;
        }

        if (try_again >= max_retries)
        {
            try_again = 0;
            i++;
        }

        if (i >= MAX_KEYBOARDS)
        {
            usb_set_leds = false;
            break;
        }
    }
}


void PlatformKbdParser::PowerButton(bool down)
{
    if (down)
        POWER_BUTTON_DOWN();
    else
        POWER_BUTTON_UP();
}

const uint8_t PlatformKbdParser::numKeys[10] = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
const uint8_t PlatformKbdParser::symKeysUp[12] = {'_', '+', '{', '}', '|', '~', ':', '"', '~', '<', '>', '?'};
const uint8_t PlatformKbdParser::symKeysLo[12] = {'-', '=', '[', ']', '\\', ' ', ';', '\'', '`', ',', '.', '/'};
const uint8_t PlatformKbdParser::padKeys[5] = {'/', '*', '-', '+', '\r'};

uint8_t PlatformKbdParser::OemToAscii(uint8_t mod, uint8_t key)
{
    uint8_t shift = (mod & 0x22);

    // [a-z]
    if (VALUE_WITHIN(key, 0x04, 0x1d))
    {
        // Upper case letters
        if ((kbdLockingKeys.kbdLeds.bmCapsLock == 0 && shift) ||
            (kbdLockingKeys.kbdLeds.bmCapsLock == 1 && shift == 0))
            return (key - 4 + 'A');

        // Lower case letters
        else
            return (key - 4 + 'a');
    } // Numbers
    else if (VALUE_WITHIN(key, 0x1e, 0x27))
    {
        if (shift)
            return ((uint8_t)pgm_read_byte(&getNumKeys()[key - 0x1e]));
        else
            return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
    } // Keypad Numbers
    else if (VALUE_WITHIN(key, 0x59, 0x61))
    {
        if (kbdLockingKeys.kbdLeds.bmNumLock == 1)
            return (key - 0x59 + '1');
    }
    else if (VALUE_WITHIN(key, 0x2d, 0x38))
        return ((shift) ? (uint8_t)pgm_read_byte(&getSymKeysUp()[key - 0x2d]) : (uint8_t)pgm_read_byte(&getSymKeysLo()[key - 0x2d]));
    else if (VALUE_WITHIN(key, 0x54, 0x58))
        return (uint8_t)pgm_read_byte(&getPadKeys()[key - 0x54]);
    else
    {
        switch (key)
        {
        case UHS_HID_BOOT_KEY_SPACE:
            return (0x20);
        case UHS_HID_BOOT_KEY_ENTER:
            return ('\r'); // Carriage return (0x0D)
        case UHS_HID_BOOT_KEY_ZERO2:
            return ((kbdLockingKeys.kbdLeds.bmNumLock == 1) ? '0' : 0);
        case UHS_HID_BOOT_KEY_PERIOD:
            return ((kbdLockingKeys.kbdLeds.bmNumLock == 1) ? '.' : 0);
        }
    }
    return (0);
}