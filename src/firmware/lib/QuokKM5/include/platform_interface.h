
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
#include "next_io.h"
#include "next_5pin_interface.h"

class PlatformInterface : public N5PInterface 
{
  public:
    virtual void init() override; 
    virtual void blockUntilResetCmd() override;
    virtual N5PCommand ReceiveCommand() override;
    virtual void sendPacket(uint8_t data[2]) override; 
  protected:
    NeXTIO m_io;

};
