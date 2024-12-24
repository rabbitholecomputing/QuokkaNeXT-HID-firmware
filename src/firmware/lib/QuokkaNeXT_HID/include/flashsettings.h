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
#pragma once
#include <stdint.h>
#include <hardware/flash.h>

#define QUOKKANEXT_HID_SETTINGS_MAGIC_NUMBER 0x1337

struct __attribute((packed)) QuokkaNeXT_HID_Settings 
{
    uint16_t magic_number;
    uint8_t led_enabled: 1;
    uint8_t swap_modifiers: 1;
    uint8_t swap_mouse_wheel_axis: 1;
    uint8_t caps_as_control: 1;
    uint8_t reserved_bits: 4;
    uint8_t sensitivity_divisor;
    int8_t mouse_wheel_count;
    uint8_t reserved_bytes[251];
};

class FlashSettings
{
public:
    void init();

    // write buffer to last page in flash
    void write_settings_page(uint8_t *buf);
    // set setting from last page in flash
    uint8_t* read_settings_page(void);
    // save settings to flash - should be called from core 0
    void save(void);
    // erase settings in flash - should be called from core 0
    void clear(void);
    // reset in memory settings to defaults
    void reset();

    inline QuokkaNeXT_HID_Settings* settings() {return &_settings;}
private:
    uint32_t _capacity = 0;
    uint32_t _last_sector = 0;
    bool m_save_requested = false;
    bool m_clear_requested = false;
    QuokkaNeXT_HID_Settings _settings;
};

extern FlashSettings setting_storage;