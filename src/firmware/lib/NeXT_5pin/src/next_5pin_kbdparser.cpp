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
#include "next_5pin_kbdparser.h"
#include "bithacks.h"
#include "next_5pin_keys.h"
#include "usb_hid_keys.h"

#ifdef QUOKKADB
// #include "platform_logmsg.h"
//extern rp2040_serial::RPSerial Logmsg;
#endif
#include <platform_logmsg.h>

extern bool global_debug;

N5PKbdRptParser::N5PKbdRptParser()
{
}

N5PKbdRptParser::~N5PKbdRptParser()
{

}

uint8_t* N5PKbdRptParser::GetKey()
{
    static uint8_t key_packet[2];
    key_packet[N5P_KEYCODE_IDX] = 0;
    key_packet[N5P_MOD_KEY_IDX] = 0;
    KeyEvent *event;
    uint8_t n5p_keycode = 0;
    bool is_key_up;
    uint8_t modifiers_usb;
    // Pack the first key event
    if (!m_keyboard_events.isEmpty())
    {
        event = m_keyboard_events.dequeue();
        is_key_up = event->IsKeyUp();
        if (is_key_up)
        {
            // key break (key up) bit
            n5p_keycode = 0x80;
        }
        n5p_keycode |= usb_keycode_to_n5p_code(event->GetKeycode());
        modifiers_usb = event->GetModifiers();
        delete(event);
    }
    else
    {
        return key_packet;
    }
    MODIFIERKEYS modifier_keys = *((MODIFIERKEYS*)(&modifiers_usb));

    if (modifier_keys.bmLeftCtrl || modifier_keys.bmRightCtrl) 
                                     key_packet[N5P_MOD_KEY_IDX] |=  N5P_MOD_KEY_CONTROL;
    if (modifier_keys.bmLeftShift)  key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_LSHIFT;
    if (modifier_keys.bmRightShift) key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_RSHIFT;
    if (modifier_keys.bmLeftAlt)    key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_LCOMMAND;
    if (modifier_keys.bmRightAlt)   key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_RCOMMAND;
    if (modifier_keys.bmLeftGUI)    key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_LALT;
    if (modifier_keys.bmRightGUI)   key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_RALT;

    if ((0x7F & n5p_keycode) == N5P_KEYCODE_MUTE)
    {
        n5p_keycode &= ~0x7F;
        n5p_keycode |= N5P_KEYCODE_VOLDOWN;
        if (!(key_packet[N5P_MOD_KEY_IDX] & (N5P_MOD_KEY_LCOMMAND | N5P_MOD_KEY_RCOMMAND)))
        {
            key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_KEY_LCOMMAND;
        }
    }

    // NeXT keycode is a modifier key
    if ((n5p_keycode & 0x7F) >= N5P_KEYCODE_RALT && (n5p_keycode & 0x7F) <= N5P_KEYCODE_CONTROL)
    {
        key_packet[N5P_KEYCODE_IDX] = 0x80;
    }
    else
    {
        key_packet[N5P_MOD_KEY_IDX] |= N5P_MOD_NOT_ONLY;
        key_packet[N5P_KEYCODE_IDX] = n5p_keycode;
    }

    return key_packet;
}
