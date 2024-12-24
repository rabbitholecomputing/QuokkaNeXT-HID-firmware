//----------------------------------------------------------------------------
//
//  QuokkaNeXT HID - a USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input
//
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokkaNeXT HID and is derived from projects below.
//
//  ADBuino & QuokkADB ADB keyboard and mouse adapter
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2007 Peter H Anderson
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of the ABDuino and the QuokkADB projects.
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
//----------------------------------------------------------------------------
#pragma once
#include "next_5pin_kbdparser.h"
#include "usbmouseparser.h"

#define N5P_MOUSE_X_IDX 0
#define N5P_MOUSE_Y_IDX 1

class N5PMouseRptParser : public MouseRptParser
{
public:
    N5PMouseRptParser(N5PKbdRptParser& kbd_parser);
    uint8_t *GetMouseData();
    bool MouseChanged();

protected:
    bool Accumulate(int8_t usb_move, int32_t &adb_move, int32_t &accumulator);
    bool m_moved;

};
