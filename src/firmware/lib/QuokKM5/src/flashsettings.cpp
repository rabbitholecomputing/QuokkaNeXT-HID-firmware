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
#include "flashsettings.h"
#include "RP2040Support.h"
#include "string.h"
#include "platform_config.h"

#define STORAGE_CMD_TOTAL_BYTES 64

extern RP2040 rp2040;

void FlashSettings::init()
{
    // Get Flash info
    uint8_t txbuf[STORAGE_CMD_TOTAL_BYTES] = {0x9f};
    uint8_t rxbuf[STORAGE_CMD_TOTAL_BYTES] = {0};
    uint32_t saved_isr_state = save_and_disable_interrupts();
    flash_do_cmd(txbuf, rxbuf, STORAGE_CMD_TOTAL_BYTES);
    restore_interrupts(saved_isr_state);
    _capacity =  1 << rxbuf[3];
    _last_sector = _capacity - FLASH_SECTOR_SIZE;

    // Read initial settings
    uint8_t* setting_buffer = read_settings_page();
    
    if (((uint16_t*)setting_buffer)[0] == QUOKKADB_SETTINGS_MAGIC_NUMBER) 
    {
        memcpy((void*)&_settings, setting_buffer, FLASH_PAGE_SIZE);
    }
    else
    {
        // set default values
        _settings.magic_number = QUOKKADB_SETTINGS_MAGIC_NUMBER;
        reset();
    }
}

void FlashSettings::clear()
{
    uint8_t buf[FLASH_PAGE_SIZE];
    memset(buf, 0, FLASH_PAGE_SIZE);
    write_settings_page(buf);
    reset();
}

void FlashSettings::reset()
{
    _settings.led_on = 1;
    _settings.swap_modifiers = 0;
    _settings.sensitivity_divisor = DEFAULT_MOUSE_SENSITIVITY_DIVISOR;
}

void FlashSettings::write_settings_page(uint8_t *buf)
{
    rp2040.idleOtherCore();
    uint32_t saved_isr_state = save_and_disable_interrupts();

    flash_range_erase(_last_sector, FLASH_SECTOR_SIZE);
    flash_range_program(_last_sector, buf, FLASH_PAGE_SIZE);

    restore_interrupts(saved_isr_state);
    rp2040.resumeOtherCore();
}

uint8_t* FlashSettings::read_settings_page(void)
{
    return (uint8_t*)(XIP_BASE + _last_sector);
}

void FlashSettings::save(void)
{
    write_settings_page((uint8_t*)&_settings);
}