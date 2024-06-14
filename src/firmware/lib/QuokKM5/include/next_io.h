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
#include <next_5pin_commands.h>

class NeXTIO
{
    public:
    
        virtual void init(uint8_t rx_pin, uint16_t rx_idiv, uint8_t tx_pin, uint16_t tx_idiv);

        // blocks until a command is read or an error occurred and returns the value
        virtual N5PCommand receiveCmd();

        // Blocks until detecting a reset command
        virtual void waitForReset();

        // Check if see if the transmitting interface is idle
        virtual bool readyToTransmit();

        // Transmit two 8 bit packets to the NeXT machine
        virtual void transmit(uint8_t data[2]);

    protected:
        // Receive 9bit packet from NeXT machine
        virtual uint16_t receiveData();

        PIO m_rx_pio, m_tx_pio;
        uint8_t m_rx_state_machine, m_tx_state_machine;
};