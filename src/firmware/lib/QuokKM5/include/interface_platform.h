
//---------------------------------------------------------------------------
//
//	ADBuino & QuokkADB ADB keyboard and mouse adapter
//
//	   Copyright (C) 2017 bbraun
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
//---------------------------------------------------------------------------
#pragma once

#include <stdint.h>
#include "platform_gpio.h"
#include <hardware/timer.h>
#include <pico/time.h>

extern bool volatile adb_collision;

inline uint32_t millis()
{
  return to_ms_since_boot(get_absolute_time());
}

class InterfacePlatform 
{
  public:
      void wait_for_reset_signal();
  protected:

    void data_lo();
    void data_hi();
    void adb_pin_out();
    void adb_pin_in();

    uint8_t data_in();

    uint16_t wait_data_lo(uint32_t us);
    uint16_t wait_data_hi(uint32_t us);
    bool adb_delay_us(uint32_t delay);
    // delay for set amount unless a collision is detected then return false
    // otherwise return true
    bool adb_delay_with_detect_us(uint32_t delay);

    void adb_irq_init(void);
    void adb_irq_disable(void);
    void adb_set_leds(uint16_t reg2);

};



inline void InterfacePlatform::data_lo()
{
    ADB_OUT_LOW();
}

inline void InterfacePlatform::data_hi()
{
    ADB_OUT_HIGH();
}
inline uint8_t InterfacePlatform::data_in()
{
    return ADB_IN_GET();
}

inline void InterfacePlatform::adb_pin_out()
{
}
inline void InterfacePlatform::adb_pin_in()
{
}



inline uint16_t InterfacePlatform::wait_data_lo(uint32_t us)
{
  // use 64bit time to prevent possible wrapping
  uint64_t start = time_us_64();
  uint64_t time;
  do
  {
    if (!data_in()) {
      time = time_us_64();
      break;
    }
    time = time_us_64();
  } while (us >= time - start);
  uint16_t diff = static_cast<uint16_t>(time - start);
  return us >= diff ? diff : 0;
}

inline uint16_t InterfacePlatform::wait_data_hi(uint32_t us)
{
  uint64_t start = time_us_64();
  uint64_t time;
  do
  {
    if (data_in()) {
      time = time_us_64();
      break;
    }
    time = time_us_64();
  } while (us >= time - start);
  uint16_t diff = static_cast<uint16_t>(time - start);
  return diff;
}


