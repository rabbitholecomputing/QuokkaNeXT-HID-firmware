//----------------------------------------------------------------------------
//
//  QuokkaNeXT HID - a USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokkaNeXT HID.
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
//---------------------------------------------------------------------------

#pragma once

enum class N5PCommand 
{
  Error,
  None,
  Reset,
  MouseQuery,
  KeyboardQuery,
  BothLEDsOn,
  LeftLEDOn,
  RightLEDOn,
  BothLEDsOff,
};