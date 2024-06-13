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


#include "platform_gpio.h"
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/clocks.h>

NeXTIO g_next_io;


void next_gpio_init(void) {
    gpio_init(NEXT_OUT_GPIO);
    gpio_set_function(NEXT_OUT_GPIO, GPIO_FUNC_SIO);
    gpio_set_dir(NEXT_OUT_GPIO, GPIO_OUT);
    gpio_put(NEXT_OUT_GPIO, true);

}

void led_gpio_init(void) {
    gpio_init(LED_GPIO);
    gpio_set_function(LED_GPIO, GPIO_FUNC_SIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);
    LED_OFF();
}

void uart_gpio_init(void) {
    uart_init(UART_PORT, UART_TX_BAUD);
    gpio_set_function(UART_TX_GPIO, GPIO_FUNC_UART);
}

void next_io_init(void)
{
    g_next_io.init(NEXT_IN_GPIO, NEXT_IN_UART_CLK_DIV, NEXT_OUT_GPIO, NEXT_OUT_BIT_CLK_DIV);
}

