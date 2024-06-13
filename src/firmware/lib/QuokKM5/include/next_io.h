//----------------------------------------------------------------------------
//
//  QuokKM5 non ADB NeXT keyboard and mouse adapter
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokKM5.
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
//
//  A majority of this code were originally released under a 
//  General Public License version 2 (GPL2) License.
//  See LICENSE in the root of this repository for more info.
//
//---------------------------------------------------------------------------
#pragma once
#include <hardware/pio.h>

enum class next_cmd_t 
{
    error,
    reset,
    query_kb,
    query_ms,
    set_left_led,
    set_right_led,
    set_both_led,
    reset_both_led

};

class NeXTIO
{
    public:
        virtual void init(uint8_t rx_pin, uint16_t rx_idiv, uint8_t tx_pin, uint16_t tx_idiv);
        virtual uint16_t receiveData();
        virtual next_cmd_t receiveCmd();
        virtual void waitForReset();
    protected:
        PIO m_rx_pio;
        uint8_t m_rx_state_machine;
};