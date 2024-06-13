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
#include "next_io.h"
#include "uart_rx_9bit.pio.h"

#define NEXT_RX_RESET_FIRST   0x1EF
#define NEXT_RX_RESET_SECOND  0x000
#define NEXT_RX_KB_QUERY      0x011
#define NEXT_RX_MS_QUERY      0x111
#define NEXT_RX_LED_CMD       0x001
#define NEXT_RX_L_LED_ON      0x100
#define NEXT_RX_R_LED_ON      0x080
#define NEXT_RX_B_LED_ON      0x180
#define NEXT_RX_B_LED_OFF     0x000

static PIO g_next_io_rx_pio;
static bool g_next_io_rx_error = false;

extern "C" void isr_pio_rx_error()
{
    pio_interrupt_clear(g_next_io_rx_pio, 0);
    irq_clear(PIO1_IRQ_0);
    g_next_io_rx_error = true;
}

void NeXTIO::init(uint8_t rx_pin, uint16_t rx_idiv, uint8_t tx_pin, uint16_t tx_idiv)
{

    // From NeXT host pio initialization

    // grab free state machine
    m_rx_pio = pio1;
    m_rx_state_machine = pio_claim_unused_sm(m_rx_pio, false);
    if (m_rx_state_machine == -1)
    {
        m_rx_pio = pio0;
        m_rx_state_machine = pio_claim_unused_sm(m_rx_pio, true);
    }
    // setup PIO IRQ0 to fire off isr_pio_transfer_done
    g_next_io_rx_pio = m_rx_pio;
    irq_set_exclusive_handler(PIO1_IRQ_0, isr_pio_rx_error);
    irq_set_enabled(PIO1_IRQ_0, true);
    uint32_t offset = pio_add_program(m_rx_pio, &uart_rx_9bit_program);
    uart_rx_9bit_program_init(m_rx_pio, m_rx_state_machine, offset, rx_pin, rx_idiv);
}
uint16_t NeXTIO::receiveData()
{
    return uart_rx_9bit_program_get(m_rx_pio, m_rx_state_machine);

}

static bool is_first_data(uint16_t data)
{
    return data == NEXT_RX_RESET_FIRST 
            || data == NEXT_RX_LED_CMD
            || data == NEXT_RX_KB_QUERY
            || data == NEXT_RX_MS_QUERY;
}

next_cmd_t NeXTIO::receiveCmd()
{
    uint16_t rx_9bit_data_first, rx_9bit_data_second;
    next_cmd_t cmd = next_cmd_t::error;
    rx_9bit_data_first = receiveData();
    while(cmd == next_cmd_t::error)
    {
        if (g_next_io_rx_error) goto rx_error;
        if(rx_9bit_data_first == NEXT_RX_RESET_FIRST)
        {
            rx_9bit_data_second = receiveData();
            if (g_next_io_rx_error) goto rx_error;

            if (rx_9bit_data_second == NEXT_RX_RESET_SECOND)
            {
                    cmd = next_cmd_t::reset;
                    break;
            }
            else if (is_first_data(rx_9bit_data_second))
            {
                rx_9bit_data_first = rx_9bit_data_second;
                continue;
            }
            else
            {
                goto rx_error;
            }
        }
        else if (rx_9bit_data_first == NEXT_RX_LED_CMD)
        {
            rx_9bit_data_second = receiveData();
            if (g_next_io_rx_error) goto rx_error;

            if (rx_9bit_data_second == NEXT_RX_L_LED_ON)
            {
                cmd = next_cmd_t::set_left_led;
                break;
            }
            else if (rx_9bit_data_second == NEXT_RX_R_LED_ON)
            {
                cmd = next_cmd_t::set_right_led;
                break;
            }
            else if(rx_9bit_data_second == NEXT_RX_B_LED_ON)
            {
                cmd = next_cmd_t::set_both_led;
                break;
            }
            else if(rx_9bit_data_second == NEXT_RX_B_LED_OFF)
            {
                cmd = next_cmd_t::reset_both_led;
                break;
            }
            else if (is_first_data(rx_9bit_data_second))
            {
                rx_9bit_data_first = rx_9bit_data_second;
                continue;
            }
            else
            {
                goto rx_error;
            }
        }
        else if (rx_9bit_data_first == NEXT_RX_KB_QUERY)
        {
            cmd = next_cmd_t::query_kb;
            break;
        }
        else if (rx_9bit_data_first == NEXT_RX_MS_QUERY)
        {
            cmd = next_cmd_t::query_ms;
            break;
        }
        else
        {
            goto rx_error;
        }
    }
    return cmd;
rx_error:
    g_next_io_rx_error = false;
    return next_cmd_t::error;
}
void NeXTIO::waitForReset()
{
    while (receiveCmd() != next_cmd_t::reset);
}