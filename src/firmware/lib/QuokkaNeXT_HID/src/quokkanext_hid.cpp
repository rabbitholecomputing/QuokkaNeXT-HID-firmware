//---------------------------------------------------------------------------
//
//  QuokkaNeXT HID - a USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokkaNeXT HID and is derived from projects below.
//
//	QuokkADB ADB keyboard and mouse adapter
//
//     Copyright 2011 Jun WAKO <wakojun@gmail.com>
//     Copyright 2013 Shay Green <gblargg@gmail.com>
//	   Copyright (C) 2017 bbraun
//     Copyright (C) 2019 Ha Thach (tinyusb.org)
//	   Copyright (C) 2020 difegue
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of QuokkADB.
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
//  Portions of this code were originally released under a Modified BSD 
//  License. See LICENSE in the root of this repository for more info.
//  
//  Portions of this code were originally released under the MIT License (MIT). 
//  See LICENSE in the root of this repository for more info.
//----------------------------------------------------------------------------

#include <Arduino.h>

#include "pico/stdlib.h"
#include <pico/multicore.h>
#include "tusb.h"
#include "host/usbh.h"
#include "platform_logmsg.h"
#include "platform_interface.h"
#include "platform_gpio.h"
#include "next_5pin_kbdparser.h"
#include "next_5pin_mouseparser.h"
#include "flashsettings.h"
#include "platform_config.h"
#include "blink.h"
#include "log_cache.h"

// Globals
extern uint8_t mousepending;
extern uint8_t kbdpending;
extern uint8_t kbdskip;
extern uint16_t kbdprev0;
extern uint16_t mousereg0;
extern uint16_t kbdreg0;
extern uint16_t kbdreg2;
extern uint8_t kbdsrq;
extern uint8_t mousesrq;
extern uint32_t kbskiptimer;
extern uint16_t modifierkeys;
extern bool n5p_reset;
bool usb_reset = false;
bool usb_mouse_reset = false;
bool usb_kbd_reset = false;
bool g_first_reset = false;
bool global_debug = false;

PlatformInterface n5p;

N5PKbdRptParser KeyboardPrs;
N5PMouseRptParser MousePrs(KeyboardPrs);

/*------------ Pre Core0 and Core1 setup ------------*/
void initVariant() 
{ 
  led_gpio_init();
  next_pwr_gpio_init();
  setting_storage.init();
}

/*------------ Core0 setup ------------*/
void setup()
{
  blink_led.blink(1);

  uart_gpio_init();
  log_init();
  Serial1.begin();
  Logmsg.println(PLATFORM_FW_VER_STRING);
  n5p.init();
  g_first_reset = false;
}

/*------------ Core0 main loop ------------*/
void loop()
{
  static bool first_loop = true;

  if (first_loop)
  {
    first_loop = false;
    n5p.blockUntilResetCmd();
    g_first_reset = true;
  }

  N5PCommand cmd = N5PCommand::None;

  cmd =  n5p.ReceiveCommand();
  if (KeyboardPrs.PendingKeyboardEvent() || MousePrs.MouseChanged())
    blink_led.led_on();
  n5p.ProcessCommand(cmd);
  blink_led.led_off();
}


// /*------------ Core1 setup ------------*/

void setup1()
{
  tuh_init(0);  
  sleep_us(500);
}

/*------------ Core1 main loop ------------*/
void loop1()
{
  tuh_task(); // tinyusb host task
  blink_led.poll();
  log_poll();
  
  KeyboardPrs.ChangeUSBKeyboardLEDs();

  if (g_first_reset)
  {
    g_first_reset = false;
    KeyboardPrs.Reset();
    MousePrs.Reset();
  }
}
