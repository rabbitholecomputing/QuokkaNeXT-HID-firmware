//---------------------------------------------------------------------------
//
//  QuokkaNeXT HID - a USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input
//
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokkaNeXT HID and is derived from projects below.
//
//	ADBuino & QuokkADB ADB keyboard and mouse adapter
//
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of the  ADBuino and the QuokkADB projects.
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


#include "next_5pin_mouseparser.h"
#include <platform_logmsg.h>
#include <platform_interface.h>
#include <limits.h>
extern bool global_debug;

N5PMouseRptParser::N5PMouseRptParser(N5PKbdRptParser &kbd_parser) : 
    m_moved{false}
{
    m_keyboard = &kbd_parser;
}

uint8_t *N5PMouseRptParser::GetMouseData()
{
    static uint8_t data[2];
    data[N5P_MOUSE_X_IDX] = 0;
    data[N5P_MOUSE_Y_IDX] = 0;
    static bool button_left_last;
    static bool button_right_last;
    bool button_left = button_left_last;
    bool button_right = button_right_last;
    MOUSE_CLICK* click = nullptr;

    if (!m_click_events.isEmpty())
    {
        click = m_click_events.dequeue();
        button_left = click->bmLeftButton;
        button_right = click->bmRightButton;
    }

    // Bit 0 = Left Button Status; 0=down
    if (!button_left)
    {
        data[N5P_MOUSE_X_IDX] = 0x01;
    }
    // Bit 0 = Right Button Status
    if (!button_right)
    {
        data[N5P_MOUSE_Y_IDX] = 0x01;
    }
    uint8_t move_x, move_y;
    move_x = AdjustMovement(m_x);
    move_y = AdjustMovement(m_y);

    if (move_x == 0 && move_y == 0)
        m_moved = false;
    else
        m_moved = true;

    // Bits 7-1 = Y move Counts (Two's compliment. Negative = up, positive = down)
    data[N5P_MOUSE_X_IDX] |= move_x << 1;

    // Bits 7-1 = X move counts (Two's compliment. Negative = left, positive = right)
    data[N5P_MOUSE_Y_IDX] |= move_y << 1;

    if (click != nullptr)
    {
        button_left_last = click->bmLeftButton;
        button_right_last = click->bmRightButton;
        delete click;
    }
    // no click, no movement
    else if(move_x == 0 && move_y == 0)
        return nullptr;

    m_processed = true;
    return data;
}

bool N5PMouseRptParser::MouseChanged()
{
    bool changed = m_moved || !m_click_events.isEmpty();
    return changed;
}