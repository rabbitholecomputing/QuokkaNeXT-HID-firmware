//----------------------------------------------------------------------------
//
//  QuokkADB ADB keyboard and mouse adapter
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2007 Peter H Anderson
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of QuokkADB.
//
//  This file is free software: you can redistribute it and/or modify it under 
//  the terms of the GNU General Public License as published by the Free 
//  Software Foundation, either version 3 of the License, or (at your option) 
// any later version.
//
//  This file is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
//  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
//  details.
//
//  You should have received a copy of the GNU General Public License along 
//  with this file. If not, see <https://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

#pragma once

#include "hidinputclasses.h"
#include "scqueue.h"

using simple_circular_queue::SCQueue;

#define KEYBOARD_QUEUE_CAPACITY (20)

extern uint8_t usb_keycode_to_adb_code(uint8_t usb_code);


struct MODIFIERKEYS {
        uint8_t bmLeftCtrl : 1;
        uint8_t bmLeftShift : 1;
        uint8_t bmLeftAlt : 1;
        uint8_t bmLeftGUI : 1;
        uint8_t bmRightCtrl : 1;
        uint8_t bmRightShift : 1;
        uint8_t bmRightAlt : 1;
        uint8_t bmRightGUI : 1;
};

class KeyEvent
{
public:
    static const uint8_t NoKey = 0xFF;
    static const uint8_t KeyDown = 0x01;
    static const uint8_t KeyUp = 0x02;
    inline uint8_t GetKeycode() { return m_keycode; }
    inline bool IsKeyUp() { return m_key_updown == KeyUp; }
    inline bool IsKeyDown() { return m_key_updown == KeyDown; }
    KeyEvent(uint8_t KeyCode, uint8_t KeyUpDown)
    {
        m_key_updown = KeyUpDown;
        m_keycode = KeyCode;
    }

protected:
    uint8_t m_keycode;
    uint8_t m_key_updown;
};

class KbdRptParser : public KeyboardReportParser
{
public:
    KbdRptParser();
    void PrintKey(uint8_t mod, uint8_t key);

    uint16_t GetAdbRegister0();
    uint16_t GetAdbRegister2();

    KeyEvent GetKeyEvent();
    bool PendingKeyboardEvent();

protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown(uint8_t mod, uint8_t key);
    void OnKeyUp(uint8_t mod, uint8_t key);
    
    uint8_t m_last_key_pressed;
    uint8_t m_last_key_up_or_down;

    uint8_t m_previous_key_pressed;
    MODIFIERKEYS m_modifier_keys;

    // Flag to indicated that the key was released
    const uint16_t KeyReleasedFlag = 0x80;

    // Flags for special modifier keys that are used in the ADB
    // protocol, but not handled automatically by the Arduino
    // USB library
    uint16_t m_custom_mod_keys;
    // Flags for the custom ADB-specific modifier keys
    static const int DeleteFlag = 0;
    static const int CapsLockFlag = 1;
    static const int ResetFlag = 2;
    static const int NumLockFlag = 3;
    static const int ScrollLockFlag = 4;
    static const int Led3ScrollLockFlag = 5;
    static const int Led2CapsLockFlag = 6;
    static const int Led1NumLockFlag = 7;

    SCQueue<KeyEvent*, KEYBOARD_QUEUE_CAPACITY> m_keyboard_events;

};
