//---------------------------------------------------------------------------
//
//  QuokkaNeXT HID - a USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input
//
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokkaNeXT HID and is derived from projects below.
//
//	QuokkADB ADB keyboard and mouse adapter
//
//     Copyright (c) 2008 David A. Mellis.  All right reserved.
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
//
//  Portions of this code were originally released under a Modified BSD 
//  License. See LICENSE in the root of this repository for more info.
//
//----------------------------------------------------------------------------

// Logging system based on Arduino Serial library
#pragma once

enum print_type {
    fmtBIN,
    fmtOCT,
    fmtDEC,
    fmtHEX, 
};

namespace platform {
    class PLogmsg 
    {
        public:
            size_t print(int number);
            size_t print(int number, print_type type);
            size_t print(const char* string);
            size_t println(int number);
            size_t println(int number, print_type type);
            size_t println(const char* string);
    };
    
}

extern platform::PLogmsg Logmsg;