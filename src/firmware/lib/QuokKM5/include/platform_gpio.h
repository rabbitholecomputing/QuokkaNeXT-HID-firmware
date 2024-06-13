//---------------------------------------------------------------------------
//
//	QuokKM5 - USB Keyboard and Mouse to 5 pin NeXT Keyboard and Mouse input 
//
//     Copyright (C) 2024 Rabbit Hole Computing LLC
//
//  This file is part of QuokKM5 and is derived from projects below.
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

extern NeXTIO g_next_io;

// Status LED GPIOs
#define LED_GPIO     18
#define LED_ON()    sio_hw->gpio_set = 1 << LED_GPIO
#define LED_OFF()   sio_hw->gpio_clr = 1 << LED_GPIO
#define LED_SET(x)  (x ? sio_hw->gpio_set = 1 << LED_GPIO : sio_hw->gpio_clr = 1 << LED_GPIO)

// // NeXT non ADB data GPIOs
#define NEXT_OUT_GPIO  0
#define NEXT_OUT_HIGH() (gpio_put(NEXT_OUT_GPIO, true)) // sio_hw->gpio_set = 1 << NEXT_OUT_GPIO
#define NEXT_OUT_LOW()  (gpio_put(NEXT_OUT_GPIO, false)) // sio_hw->gpio_clr = 1 << NEXT_OUT_GPIO

// pio bit clock need to run twice as fast as the output bit clock
#define NEXT_OUT_BIT_CLK_DIV 7200 / 2

#define NEXT_IN_GPIO   1
#define NEXT_IN_GET() (gpio_get(NEXT_IN_GPIO))
#define NEXT_IN_UART_CLK_DIV 900


// \todo delete block
#define ADB_IN_GPIO   1
#define ADB_OUT_GPIO  0
#define ADB_OUT_LOW() NEXT_OUT_LOW()
#define ADB_OUT_HIGH() NEXT_OUT_HIGH()
#define ADB_IN_GET() NEXT_IN_GET()

// UART out messaging
#define UART_TX_GPIO    16
#define UART_TX_BAUD    115200
#define UART_PORT       uart0

void next_gpio_init(void);
void next_io_init(void);
void uart_gpio_init(void);
void led_gpio_init(void);
