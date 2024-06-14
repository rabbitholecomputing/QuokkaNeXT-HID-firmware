//---------------------------------------------------------------------------
//
//	ADBuino & QuokkADB ADB keyboard and mouse adapter
//
//     Copyright 2011 Jun WAKO <wakojun@gmail.com>
//     Copyright 2013 Shay Green <gblargg@gmail.com>
//	   Copyright (C) 2017 bbraun
//	   Copyright (C) 2020 difegue
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
//  with the file. If not, see <https://www.gnu.org/licenses/>.
//
//  Portions of this code were originally released under a Modified BSD 
//  License. See LICENSE in the root of this repository for more info.
//
//----------------------------------------------------------------------------
#include <Arduino.h>
#include "next_5pin_interface.h"
#include "bithacks.h"
#include "math.h"
#include <platform_logmsg.h>
#include <next_5pin_mouseparser.h>
#include <next_5pin_kbdparser.h>


uint8_t mousepending = 0;
uint8_t kbdpending = 0;
uint8_t kbdskip = 0;
uint16_t kbdprev0 = 0;
uint16_t kbdreg0 = 0;
uint16_t kbdreg2 = 0xFFFF;
uint8_t kbdsrq = 0;
uint8_t mousesrq = 0;
uint16_t modifierkeys = 0xFFFF;
uint64_t kbskiptimer = 0;

bool n5p_reset = false;

bool mouse_flush = false;
bool kbd_flush = false;
volatile bool adb_collision = false; 
volatile bool collision_detection = false;
bool mouse_skip_next_listen_reg3 = false;
bool kbd_skip_next_listen_reg3 = false;
bool g_global_reset = false; 
extern bool global_debug;
extern ADBMouseRptParser MousePrs;
extern ADBKbdRptParser KeyboardPrs;




void N5PInterface::ProcessCommand(N5PCommand cmd)
{
  if (cmd == N5PCommand::Reset)
  {
    // \todo Do something here, maybe
    return;
  }

  // see if it is addressed to us
  if (cmd == N5PCommand::MouseQuery)
  {
    if (mousepending)
    {
      mousepending = 0;
      // collect mouse input and output
    }
     
    return;
  }
  
  if (cmd == N5PCommand::KeyboardQuery)
  {
      static bool make = true;
      uint8_t packet[2] = {0x47, 0x80};
      if (!make)
      {
        packet[0] |= 0x80;
      }
      make = !make;

      sendPacket(packet);
      if (kbdpending)
      {
        // send latest keyboard key
        // or no output
      }
      return;
  }

  static bool leftLED = false;
  static bool rightLED = false;
  if (cmd == N5PCommand::BothLEDsOn)
  { 
      KeyboardPrs.SetUSBkeyboardLEDs(true, true, false);
      leftLED = true;
      rightLED = true;
      return;
  }
  if (cmd == N5PCommand::BothLEDsOff)
  { 
      KeyboardPrs.SetUSBkeyboardLEDs(false, false, false);
      leftLED = false;
      rightLED = false;
      return;
  }
  if (cmd == N5PCommand::LeftLEDOn)
  { 
      KeyboardPrs.SetUSBkeyboardLEDs(true, rightLED, false);
      leftLED = true;
      return;
  }
  if (cmd == N5PCommand::RightLEDOn)
  { 
      KeyboardPrs.SetUSBkeyboardLEDs(leftLED, true, false);
      rightLED = true;
      return;
  }
}

void N5PInterface::Reset(void)
{
}
