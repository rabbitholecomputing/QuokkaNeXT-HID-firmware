//---------------------------------------------------------------------------
//
//	ADBuino & QuokkADB ADB keyboard and mouse adapter
//
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of the ADBuino and the QuokkADB projects.
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
//  with file. If not, see <https://www.gnu.org/licenses/>.
//
//----------------------------------------------------------------------------
#include <Arduino.h>
#include "usbkbdparser.h"
#include "bithacks.h"
#include "usb_hid_keys.h"
#include "platform_logmsg.h"

extern "C" uint32_t millis();
extern bool global_debug;

#define ENQUEUE_RETRY_TIMEOUT 3000

KbdRptParser::KbdRptParser()
{
}

KbdRptParser::~KbdRptParser(){}

bool KbdRptParser::PendingKeyboardEvent()
{
    return !m_keyboard_events.isEmpty();
}

void KbdRptParser::Reset(void)
{
    while(PendingKeyboardEvent())
    {
        
        delete(m_keyboard_events.dequeue());
    }

    kbdLockingKeys.bLeds = 0x00;
    SetUSBkeyboardLEDs(false, false, false);

}

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
    MODIFIERKEYS mod;
    *((uint8_t *)&mod) = m;
    Logmsg.print((mod.bmLeftCtrl == 1) ? "C" : " ");
    Logmsg.print((mod.bmLeftShift == 1) ? "S" : " ");
    Logmsg.print((mod.bmLeftAlt == 1) ? "A" : " ");
    Logmsg.print((mod.bmLeftGUI == 1) ? "G" : " ");

    Logmsg.print(" >");
    Logmsg.print(key, fmtHEX);
    Logmsg.print("< ");

    Logmsg.print((mod.bmRightCtrl == 1) ? "C" : " ");
    Logmsg.print((mod.bmRightShift == 1) ? "S" : " ");
    Logmsg.print((mod.bmRightAlt == 1) ? "A" : " ");
    Logmsg.println((mod.bmRightGUI == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{

    if (key == USB_KEY_NONE || key == USB_KEY_ERR_OVF)
    return;

    if (global_debug)
    {
        Logmsg.print("DN ");
        PrintKey(mod, key);
    }
    uint8_t c = OemToAscii(mod, key);

    if (c)
        OnKeyPressed(c);

    if (key == USB_KEY_F15 || key == USB_KEY_INSERT || key == USB_KEY_HELP)
    {
        PowerButton(true);
    }
    else if (!m_keyboard_events.enqueue(new KeyEvent(key, KeyEvent::KeyDown, mod)))
    {
        if (global_debug)
            Logmsg.println("Warning! unable to enqueue new KeyDown");

        uint32_t start = millis();
        bool success = false;
        while((uint32_t)(millis() - start) < ENQUEUE_RETRY_TIMEOUT)
        {
            if (m_keyboard_events.enqueue(new KeyEvent(key, KeyEvent::KeyDown, mod)))
            {
                success = true;
                break;
            }
        }
        if (!success)
        {
            Logmsg.println("Warning! Timeout, unable to enqueue new KeyDown after retrying");
        }
    }
}


void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    if (key == USB_KEY_NONE || key == USB_KEY_ERR_OVF)
        return;

    if (global_debug)
    {
        Logmsg.print("UP ");
        PrintKey(mod, key);
    }

    if (key == USB_KEY_F15 || key == USB_KEY_INSERT || key == USB_KEY_HELP)
    {
        PowerButton(false);
    }
    else if (!m_keyboard_events.enqueue(new KeyEvent(key, KeyEvent::KeyUp, mod)))
    {
        if (global_debug)
            Logmsg.println("Warning! unable to enqueue new KeyUp, retrying");

        uint32_t start = millis();
        bool success = false;
        while((uint32_t)(millis() - start) < ENQUEUE_RETRY_TIMEOUT)
        {
            if (m_keyboard_events.enqueue(new KeyEvent(key, KeyEvent::KeyUp, mod)))
            {
                success = true;
                break;
            }
        }
        if (!success)
        {
            Logmsg.println("Warning! Timeout, unable to enqueue new KeyUp after retrying");
        }
    }
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after)
{

    MODIFIERKEYS beforeMod;
    *((uint8_t *)&beforeMod) = before;

    MODIFIERKEYS afterMod;
    *((uint8_t *)&afterMod) = after;

    m_modifier_keys = *((MODIFIERKEYS *)&after);
    uint8_t mod = after;

    if (global_debug)
    {
        Logmsg.print("Before: ");
        Logmsg.print(before,  fmtHEX);
        Logmsg.print(" after: ");
        Logmsg.print(after,  fmtHEX);
        Logmsg.print(" ");
        Logmsg.print(*((uint8_t *)&m_modifier_keys), fmtHEX);
    }
    if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl)
    {
        if (global_debug)
        {
            Logmsg.println("LeftCtrl changed");
        }
        if (afterMod.bmLeftCtrl)
        {
            OnKeyDown(mod, USB_KEY_LEFTCTRL);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_LEFTCTRL);
        }
    }
    if (beforeMod.bmLeftShift != afterMod.bmLeftShift)
    {
        if (global_debug)
        {
            Logmsg.println("LeftShift changed");
        }
        if (afterMod.bmLeftShift)
        {
            OnKeyDown(mod, USB_KEY_LEFTSHIFT);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_LEFTSHIFT);
        }
    }
    if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt)
    {
        if (global_debug)
        {
            Logmsg.println("LeftAlt changed");
        }
        if (afterMod.bmLeftAlt)
        {
            OnKeyDown(mod, USB_KEY_LEFTALT);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_LEFTALT);
        }
    }
    if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI)
    {
        if (global_debug)
        {
            Logmsg.println("LeftGUI changed");
        }
        if (afterMod.bmLeftGUI)
        {
            OnKeyDown(mod, USB_KEY_LEFTMETA);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_LEFTMETA);
        }
    }

    if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl)
    {
        if (global_debug)
        {
            Logmsg.println("RightCtrl changed");
        }
        if (afterMod.bmRightCtrl)
        {
            OnKeyDown(mod, USB_KEY_RIGHTCTRL);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_RIGHTCTRL);
        }
    }
    if (beforeMod.bmRightShift != afterMod.bmRightShift)
    {
        if (global_debug)
        {
            Logmsg.println("RightShift changed");
        }
        if (afterMod.bmRightShift)
        {
            OnKeyDown(mod, USB_KEY_RIGHTSHIFT);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_RIGHTSHIFT);
        }
    }
    if (beforeMod.bmRightAlt != afterMod.bmRightAlt)
    {
        if (global_debug)
        {
            Logmsg.println("RightAlt changed");
        }
        if (afterMod.bmRightAlt)
        {
            OnKeyDown(mod, USB_KEY_RIGHTALT);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_RIGHTALT);
        }
    }
    if (beforeMod.bmRightGUI != afterMod.bmRightGUI)
    {
        if (global_debug)
        {
            Logmsg.println("RightGUI changed");
        }
        if (afterMod.bmRightGUI)
        {
            OnKeyDown(mod, USB_KEY_RIGHTMETA);
        }
        else
        {
            OnKeyDown(mod, USB_KEY_RIGHTMETA);
        }
    }
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
    if (global_debug)
    {

        Logmsg.print("ASCII: ");
        Logmsg.println((char)key);
    }
};
