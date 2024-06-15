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
#include "platform_interface.h"
#include "next_5pin_kbdparser.h"
#include "hardware/gpio.h"
#include "platform_gpio.h"
#include <time.h>


extern volatile bool adb_collision;
extern volatile bool collision_detection;
extern N5PKbdRptParser KeyboardPrs;

void PlatformInterface::init()
{
  m_io.init(NEXT_IN_GPIO, NEXT_IN_UART_CLK_DIV, NEXT_OUT_GPIO, NEXT_OUT_BIT_CLK_DIV);
}


void PlatformInterface::blockUntilResetCmd()
{
  m_io.waitForReset();
}

N5PCommand PlatformInterface::ReceiveCommand()
{
    return m_io.receiveCmd();
}

void PlatformInterface::sendPacket(uint8_t data[2])
{
  while (!m_io.readyToTransmit());
  m_io.transmit(data);
}; 