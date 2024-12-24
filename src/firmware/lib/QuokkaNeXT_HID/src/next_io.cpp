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
//
//  A majority of this code were originally released under a 
//  General Public License version 2 (GPL2) License.
//  See LICENSE in the root of this repository for more info.
//
//---------------------------------------------------------------------------
#include "next_io.h"
#include "uart_rx_9bit.pio.h"
#include "tx_packet_to_next.pio.h"

#define NEXT_RX_RESET_FIRST   0x1EF
#define NEXT_RX_RESET_SECOND  0x000
#define NEXT_RX_KB_QUERY      0x011
#define NEXT_RX_MS_QUERY      0x111
#define NEXT_RX_LED_CMD       0x001
#define NEXT_RX_L_LED_ON      0x100
#define NEXT_RX_R_LED_ON      0x080
#define NEXT_RX_B_LED_ON      0x180
#define NEXT_RX_B_LED_OFF     0x000

static PIO g_next_io_rx_pio, g_next_io_tx_pio;
static bool g_next_io_rx_error = false;
static bool g_next_io_tx_done = true;

extern bool g_first_reset;

extern "C" void isr_pio_rx_error()
{
    pio_interrupt_clear(g_next_io_rx_pio, 0);
    irq_clear(PIO1_IRQ_0);
    g_next_io_rx_error = true;
}

extern "C" void isr_pio_tx_done()
{
    pio_interrupt_clear(g_next_io_tx_pio, 0);
    irq_clear(PIO0_IRQ_0);
    g_next_io_tx_done = true;
}

void NeXTIO::init(uint8_t rx_pin, uint16_t rx_idiv, uint8_t tx_pin, uint16_t tx_idiv)
{
    uint32_t offset;

    // From NeXT host pio initialization
    m_rx_pio = pio1;
    m_rx_state_machine = pio_claim_unused_sm(m_rx_pio, false);
    // setup PIO IRQ0 to fire off isr_pio_transfer_done
    g_next_io_rx_pio = m_rx_pio;
    irq_set_exclusive_handler(PIO1_IRQ_0, isr_pio_rx_error);
    irq_set_enabled(PIO1_IRQ_0, true);
    offset = pio_add_program(m_rx_pio, &uart_rx_9bit_program);
    uart_rx_9bit_program_init(m_rx_pio, m_rx_state_machine, offset, rx_pin, rx_idiv);


    // To NeXT host pio initialization
    m_tx_pio = pio0;
    m_tx_state_machine = pio_claim_unused_sm(m_tx_pio, false);
    // setup PIO IRQ0 to fire off isr_pio_transfer_done
    g_next_io_tx_pio = m_tx_pio;
    irq_set_exclusive_handler(PIO0_IRQ_0, isr_pio_tx_done);
    irq_set_enabled(PIO0_IRQ_0, true);
    offset = pio_add_program(m_tx_pio, &tx_packet_to_next_program);
    tx_packet_to_next_program_init(m_tx_pio, m_tx_state_machine, offset, tx_pin, tx_idiv);
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

N5PCommand NeXTIO::receiveCmd()
{
    uint16_t rx_9bit_data_first, rx_9bit_data_second;
    N5PCommand cmd = N5PCommand::Error;
    rx_9bit_data_first = receiveData();
    while(cmd == N5PCommand::Error)
    {
        if (g_next_io_rx_error) goto rx_error;
        if(rx_9bit_data_first == NEXT_RX_RESET_FIRST)
        {
            rx_9bit_data_second = receiveData();
            if (g_next_io_rx_error) goto rx_error;

            if (rx_9bit_data_second == NEXT_RX_RESET_SECOND)
            {
                    cmd = N5PCommand::Reset;
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
                cmd = N5PCommand::LeftLEDOn;
                break;
            }
            else if (rx_9bit_data_second == NEXT_RX_R_LED_ON)
            {
                cmd = N5PCommand::RightLEDOn;
                break;
            }
            else if(rx_9bit_data_second == NEXT_RX_B_LED_ON)
            {
                cmd = N5PCommand::BothLEDsOn;
                break;
            }
            else if(rx_9bit_data_second == NEXT_RX_B_LED_OFF)
            {
                cmd = N5PCommand::BothLEDsOff;
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
            cmd = N5PCommand::KeyboardQuery;
            break;
        }
        else if (rx_9bit_data_first == NEXT_RX_MS_QUERY)
        {
            cmd = N5PCommand::MouseQuery;
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
    return N5PCommand::Error;
}
void NeXTIO::waitForReset()
{
    while (receiveCmd() != N5PCommand::Reset)
    {
        g_first_reset = true;
    }
}

bool NeXTIO::readyToTransmit()
{
    return g_next_io_tx_done;
}

void NeXTIO::transmit(uint8_t *data)
{
    uint32_t raw_data = 0;
    // idle state
    if (data == nullptr)
    {
        // stop bits high
        raw_data = (1 << 8) | (1 << 17); 
    }
    else
    {
        raw_data = data[0] | (data[1] << 9);
    }
    tx_packet_to_next_send(m_tx_pio, m_tx_state_machine, raw_data);
}