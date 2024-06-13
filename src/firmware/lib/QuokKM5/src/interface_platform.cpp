//---------------------------------------------------------------------------
//
//	QuokkADB ADB keyboard and mouse adapter
//
//	   Copyright (C) 2017 bbraun
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  This file is part of the QuokkADB project.
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
//---------------------------------------------------------------------------
#include "interface_platform.h"
#include "next_5pin_kbdparser.h"
#include "hardware/gpio.h"
#include <time.h>
#include "next_5pin.h"
#include "blink.h"

extern volatile bool adb_collision;
extern volatile bool collision_detection;
extern ADBKbdRptParser KeyboardPrs;

void InterfacePlatform::init()
{
  m_io.init(NEXT_IN_GPIO, NEXT_IN_UART_CLK_DIV, NEXT_OUT_GPIO, NEXT_OUT_BIT_CLK_DIV);
}

// Check timings for a period in the reset signal to keyboard form host
// static bool within_reset_period(uint32_t begin, uint32_t end, uint32_t period)
// {
//   const size_t NUM_OF_PERIODS = 5;
//   const uint32_t period_min[NUM_OF_PERIODS] = {N5P_1BIT_WAIT - N5P_WAIT_TOLERANCE_MIN,
//                                                 N5P_4BIT_WAIT - N5P_WAIT_TOLERANCE_MIN,
//                                                 N5P_1BIT_WAIT - N5P_WAIT_TOLERANCE_MIN,
//                                                 N5P_6BIT_WAIT - N5P_WAIT_TOLERANCE_MIN,
//                                                 N5P_10BIT_WAIT - N5P_WAIT_TOLERANCE_MIN
//                                               };
//   const uint32_t period_max[NUM_OF_PERIODS] = {N5P_1BIT_WAIT + N5P_WAIT_TOLERANCE_MAX,
//                                                N5P_4BIT_WAIT + N5P_WAIT_TOLERANCE_MAX,
//                                                N5P_1BIT_WAIT + N5P_WAIT_TOLERANCE_MAX,
//                                                N5P_6BIT_WAIT + N5P_WAIT_TOLERANCE_MAX,
//                                                N5P_10BIT_WAIT + N5P_WAIT_TOLERANCE_MAX
//                                               };
//   uint32_t difference = (uint32_t)(end - begin);
//   if( difference <= period_max[period] && difference >= period_min[period] )
//   {
//     return true;
//   }
//   return false;
// }

void InterfacePlatform::wait_for_reset_signal()
{
  m_io.waitForReset();
}
// {
//   uint8_t period = 0;
//   if (NEXT_IN_GET())
//     while (NEXT_IN_GET());
//   uint32_t start = time_us_32();
//   uint32_t end = 0;
//   while (true)
//   {
// period0:
//     period = 0;
//     while (!NEXT_IN_GET());
//     end = time_us_32();
//     if (!within_reset_period(start, end, period))
//     {
//         while(NEXT_IN_GET());
//         start = time_us_32();
//         goto period0;
//     }
//     blink_led.led_on(true);
// period1:
//     start = end;
//     period = 1;
//     while (NEXT_IN_GET());
//     end = time_us_32();
//     if (!within_reset_period(start, end, period))
//     {
//       start = end;
//       goto period0;
//     } 
// period2:
//     start = end;
//     period = 2;
//     while(!NEXT_IN_GET())
//     end = time_us_32();

//     if (!within_reset_period(start, end, period))
//     {
//           while(NEXT_IN_GET());
//           start = time_us_32();
//           goto period0;
//     }
// //period3:
//     start = end;
//     period = 3;
//     while(NEXT_IN_GET());
//     end = time_us_32();
//     if(within_reset_period(start, end, 1))
//     {
//       goto period2;
//     }
//     if (!within_reset_period(start, end, period))
//     {
//       start = end;
//       goto period0;
//     }
// // period4:
//     start = end;
//     period = 4;
//     while(!NEXT_IN_GET())
//     end = time_us_32();
    
//     if(within_reset_period(start, end, 0))
//     {
//       goto period1;
//     }
//     if (within_reset_period(start, end, period))
//     {
//       // found reset sequence
//       blink_led.led_off(true); 
//     }
//     else
//     {
//       while(NEXT_IN_GET());
//       start = time_us_32();
//       goto period0;
//     }
//   }
// }


bool InterfacePlatform::adb_delay_with_detect_us(uint32_t delay) 
{
  uint64_t start = time_us_64();
  uint64_t time;
  bool collision_free = true;
  do
  {
    if (adb_collision) {
      collision_free = false;
      break;
    }
    time = time_us_64();
  } while (delay >= time - start);
  
  return collision_free;
}

bool InterfacePlatform::adb_delay_us(uint32_t delay) 
{
  uint64_t start = time_us_64();
  uint64_t time;
  do
  {
    time = time_us_64();
  } while (delay >= time - start);
  return true;
}


static void adb_in_irq_callback(uint gpio, uint32_t event_mask) {
  // maybe delete call back irq support
}

void InterfacePlatform::adb_irq_init(void) {
  gpio_acknowledge_irq(ADB_IN_GPIO, GPIO_IRQ_EDGE_FALL);
  gpio_set_irq_enabled_with_callback(ADB_IN_GPIO, GPIO_IRQ_EDGE_FALL, true, &adb_in_irq_callback);
}

void InterfacePlatform::adb_irq_disable(void) {
  gpio_set_irq_enabled_with_callback(ADB_IN_GPIO, GPIO_IRQ_EDGE_FALL, false, &adb_in_irq_callback); 
  gpio_acknowledge_irq(ADB_IN_GPIO, GPIO_IRQ_EDGE_FALL);
}

void InterfacePlatform::adb_set_leds(uint16_t reg2) 
{
  bool numlock = !(reg2 & 0x1);
  bool capslock = !(reg2 & 0x2);
  bool scrolllock = !(reg2 & 0x4);
  KeyboardPrs.SetUSBkeyboardLEDs(capslock, numlock, scrolllock);
}
