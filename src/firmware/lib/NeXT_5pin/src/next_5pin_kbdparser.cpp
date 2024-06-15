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
    const uint8_t key = 0, mod = 1;
    static uint8_t key_packet[2];
    key_packet[0] = 0;
    key_packet[1] = 0;
    KeyEvent *event;
    uint8_t n5p_keycode = 0;
    uint8_t n5p_modifiers = 0;
    bool is_key_up;
    // Pack the first key event
    if (!m_keyboard_events.isEmpty())
    {
        event = m_keyboard_events.dequeue();
        is_key_up = event->IsKeyUp();
        if (!is_key_up)
        {
            n5p_keycode = 0x80;
        }
        n5p_keycode |= usb_keycode_to_n5p_code(event->GetKeycode());
        delete(event);
    }
    else
    {
        // return IDLE state
        return key_packet;
    }

    if (m_modifier_keys.bmLeftCtrl || m_modifier_keys.bmRightCtrl) 
                                      B_SET(key_packet[mod], N5P_MOD_KEY_CONTROL);
    if (m_modifier_keys.bmLeftShift)  B_SET(key_packet[mod], N5P_MOD_KEY_LSHIFT);
    if (m_modifier_keys.bmRightShift) B_SET(key_packet[mod], N5P_MOD_KEY_RSHIFT);
    if (m_modifier_keys.bmLeftAlt)    B_SET(key_packet[mod], N5P_MOD_KEY_LCOMMAND);
    if (m_modifier_keys.bmRightAlt)   B_SET(key_packet[mod], N5P_MOD_KEY_RCOMMAND);
    if (m_modifier_keys.bmLeftGUI)    B_SET(key_packet[mod], N5P_MOD_KEY_LALT);
    if (m_modifier_keys.bmRightGUI)   B_SET(key_packet[mod], N5P_MOD_KEY_RALT);
    
    // NeXT keycode is a non modifier key
    if (!(n5p_keycode >= N5P_KEYCODE_RALT && n5p_keycode <= N5P_MOD_KEY_CONTROL))
    {
        B_SET(key_packet[mod], N5P_MOD_NOT_ONLY);
        key_packet[key] = n5p_keycode;
    }

    return key_packet;
}
