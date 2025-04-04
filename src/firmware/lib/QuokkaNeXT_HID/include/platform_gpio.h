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
#include <stdlib.h>
#include <hardware/structs/sio.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include "next_io.h"

// Status LED GPIOs
#define LED_GPIO     18
#define LED_ON()    sio_hw->gpio_set = 1 << LED_GPIO
#define LED_OFF()   sio_hw->gpio_clr = 1 << LED_GPIO
#define LED_SET(x)  (x ? sio_hw->gpio_set = 1 << LED_GPIO : sio_hw->gpio_clr = 1 << LED_GPIO)

// // NeXT non ADB data GPIOs
#define NEXT_OUT_GPIO  0
// pio bit clock need to run twice as fast as the output bit clock
#define NEXT_OUT_BIT_CLK_DIV 7200 / 2

#define NEXT_IN_GPIO   1
#define NEXT_IN_UART_CLK_DIV 900

#define NEXT_POWER_GPIO     22
#define POWER_BUTTON_DOWN() gpio_put(NEXT_POWER_GPIO, false)
#define POWER_BUTTON_UP()   gpio_put(NEXT_POWER_GPIO, true)

// UART out messaging
#define UART_TX_GPIO    16
#define UART_TX_BAUD    115200
#define UART_PORT       uart0

void next_pwr_gpio_init(void);
void uart_gpio_init(void);
void led_gpio_init(void);
