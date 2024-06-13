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
#include "next_5pin.h"
#include "bithacks.h"
#include "math.h"
#include <platform_logmsg.h>
#include <next_5pin_mouseparser.h>
#include <next_5pin_kbdparser.h>

uint8_t mouse_addr = MOUSE_DEFAULT_ADDR;
uint8_t kbd_addr = KBD_DEFAULT_ADDR;
uint8_t mouse_handler_id = MOUSE_DEFAULT_HANDLER_ID;
uint8_t kbd_handler_id = KBD_DEFAULT_HANDLER_ID;
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
// The original data_lo code would just set the bit as an output
// That works for a host, since the host is doing the pullup on the ADB line,
// but for a device, it won't reliably pull the line low.  We need to actually
// set it.

// Stop bit is just bit 0, but this doesn't wait after the signal goes high

inline bool N5PInterface::place_stop_bit(void)
{
  data_lo();
  adb_delay_us(70);
  data_hi();
  return true;
}

inline bool N5PInterface::place_bit0(void)
{
  data_lo();
  adb_delay_us(65);
  data_hi();
  return adb_delay_us(35);
}

inline bool N5PInterface::place_bit1(void)
{
  data_lo();
  adb_delay_us(35);
  data_hi();
  return adb_delay_us(65);
}
inline bool N5PInterface::send_byte(uint8_t data)
{
  for (int i = 0; i < 8; i++)
  {
    if (data & (0x80 >> i))
    {
      place_bit1();
    }
    else
    {
      place_bit0();
    }
  }
  return true;
}


inline bool N5PInterface::place_bit0_with_detect(void)
{
  data_lo();
  adb_delay_us(65);
  data_hi();
  return adb_delay_with_detect_us(35);
}

inline bool N5PInterface::place_bit1_with_detect(void)
{
  data_lo();
  adb_delay_us(35);
  data_hi();
  return adb_delay_with_detect_us(65);
}
inline bool N5PInterface::send_byte_with_detect(uint8_t data)
{
  for (int i = 0; i < 8; i++)
  {
    if (data & (0x80 >> i))
    {
      place_bit1_with_detect();
    }
    else
    {
      place_bit0_with_detect();
    }
  }
  return true;
}

N5PCommand N5PInterface::ReceiveCommand()
{
  next_cmd_t cmd;
  while (true)
  {
    cmd = m_io.receiveCmd();
    switch(cmd)
    {
      case next_cmd_t::reset :
        return N5PCommand::Reset;
      case next_cmd_t::query_kb :
        return N5PCommand::KeyboardQuery;
      case next_cmd_t::query_ms :
        return N5PCommand::MouseQuery;
      case next_cmd_t::set_left_led :
        Logmsg.println("NeXT Command - Left LED on");
        return N5PCommand::LeftLEDOn;
      case next_cmd_t::set_right_led :
        Logmsg.println("NeXT Command - Right LED on");
        return N5PCommand::RightLEDOn;
      case next_cmd_t::set_both_led :
        Logmsg.println("NeXT Command - Both LED on");
        return N5PCommand::BothLEDsOn;
      case next_cmd_t::reset_both_led :
        Logmsg.println("NeXT Command - Both LED off");
        return N5PCommand::BothLEDsOff;
      case next_cmd_t::error :
        Logmsg.println("Next Command errored");
        return N5PCommand::ErrorFindingCommand;
    }
  }
  return N5PCommand::ErrorFindingCommand;
}


void N5PInterface::ProcessCommand(N5PCommand cmd)
{
  if (cmd == N5PCommand::Reset)
  {
    n5p_reset = true;
    g_global_reset = true;
    if (global_debug)
    {
      Logmsg.println("ALL: Global reset signal");
    }
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
  mousesrq = false;
  kbdsrq = false;
  kbd_skip_next_listen_reg3 = false;
  mouse_skip_next_listen_reg3 = false;
  mouse_addr = MOUSE_DEFAULT_ADDR;
  kbd_addr = KBD_DEFAULT_ADDR;
  mouse_handler_id = MOUSE_DEFAULT_HANDLER_ID;
  kbd_handler_id = KBD_DEFAULT_HANDLER_ID;
  kbdreg2 = 0xFFFF;
}
