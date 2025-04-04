#include <SerialUART.h>
#include "log_cache.h"
#include <stdarg.h>
#include <pico/stdio.h>
#include <pico/mutex.h>
#include "platform_config.h"

static mutex log_mutex;
// This memory buffer can be read by debugger and is also saved to zululog.txt
#define LOGBUFMASK (LOGBUFSIZE - 1)

// The log buffer is in special uninitialized RAM section so that it is not reset
// when soft rebooting or jumping from bootloader.
uint32_t g_log_magic;
char g_logbuffer[LOGBUFSIZE + 1];
uint32_t g_logpos;

void log_raw(const char *str)
{
    mutex_enter_blocking(&log_mutex);
    // Keep log from reboot / bootloader if magic matches expected value
    const char *p = str;
    while (*p)
    {
        g_logbuffer[g_logpos & LOGBUFMASK] = *p++;
        g_logpos++;
    }

    // Keep buffer null-terminated
    g_logbuffer[g_logpos & LOGBUFMASK] = '\0';
    mutex_exit(&log_mutex);
}

// Log byte as hex
void log_raw(uint8_t value)
{
    const char *nibble = "0123456789ABCDEF";
    char hexbuf[5] = {
        '0', 'x', 
        nibble[(value >>  4) & 0xF], nibble[(value >>  0) & 0xF],
        0
    };
    log_raw(hexbuf);
}

// Log uint16_t as hex
void log_raw(uint16_t value)
{
    const char *nibble = "0123456789ABCDEF";
    char hexbuf[11] = {
        '0', 'x', 
        nibble[(value >> 12) & 0xF], nibble[(value >>  8) & 0xF],
        nibble[(value >>  4) & 0xF], nibble[(value >>  0) & 0xF],
        0
    };
    log_raw(hexbuf);
}

// Log integer as hex
void log_raw(uint32_t value)
{
    const char *nibble = "0123456789ABCDEF";
    char hexbuf[11] = {
        '0', 'x', 
        nibble[(value >> 28) & 0xF], nibble[(value >> 24) & 0xF],
        nibble[(value >> 20) & 0xF], nibble[(value >> 16) & 0xF],
        nibble[(value >> 12) & 0xF], nibble[(value >>  8) & 0xF],
        nibble[(value >>  4) & 0xF], nibble[(value >>  0) & 0xF],
        0
    };
    log_raw(hexbuf);
}

// Log integer as hex
void log_raw(uint64_t value)
{
    const char *nibble = "0123456789ABCDEF";
    char hexbuf[19] = {
        '0', 'x',
        nibble[(value >> 60) & 0xF], nibble[(value >> 56) & 0xF],
        nibble[(value >> 52) & 0xF], nibble[(value >> 48) & 0xF],
        nibble[(value >> 44) & 0xF], nibble[(value >> 40) & 0xF],
        nibble[(value >> 36) & 0xF], nibble[(value >> 32) & 0xF],
        nibble[(value >> 28) & 0xF], nibble[(value >> 24) & 0xF],
        nibble[(value >> 20) & 0xF], nibble[(value >> 16) & 0xF],
        nibble[(value >> 12) & 0xF], nibble[(value >>  8) & 0xF],
        nibble[(value >>  4) & 0xF], nibble[(value >>  0) & 0xF],
        0
    };
    log_raw(hexbuf);
}

void log_raw(int32_t value)
{
    char decbuf[16] = {0};
    char *p = &decbuf[14];
    int remainder = (value < 0) ? -value : value;
    do
    {
        *--p = '0' + (remainder % 10);
        remainder /= 10;
    } while (remainder > 0);
    
    if (value < 0)
    {
        *--p = '-';
    }

    log_raw(p);
}

// Log integer as decimal
void log_raw(int value)
{
    log_raw((int32_t) value);
}

void log_raw(bytearray array)
{
    for (size_t i = 0; i < array.len; i++)
    {
        log_raw(array.data[i]);
        log_raw(" ");
        if (i > 32)
        {
            log_raw("... (total ", (int)array.len, ")");
            break;
        }
    }
}

void log_init()
{
    mutex_init(&log_mutex);
}

uint32_t log_get_buffer_len()
{
    return g_logpos;
}

const char *log_get_buffer(uint32_t *startpos, uint32_t *available)
{
    uint32_t default_pos = 0;
    if (startpos == NULL)
    {
        startpos = &default_pos;
    }
    mutex_enter_blocking(&log_mutex);
    // Check oldest data available in buffer
    uint32_t lag = (g_logpos - *startpos);
    if (lag >= LOGBUFSIZE)
    {
        // If we lose data, skip 512 bytes forward to give us time to transmit
        // pending data before new log messages arrive. Also skip to next line
        // break to keep formatting consistent.
        uint32_t oldest = g_logpos - LOGBUFSIZE + 512;
        while (oldest < g_logpos)
        {
            char c = g_logbuffer[oldest & LOGBUFMASK];
            if (c == '\r' || c == '\n') break;
            oldest++;
        }

        if (oldest > g_logpos)
        {
            oldest = g_logpos;
        }
        *startpos = oldest;
    }

    const char *result = &g_logbuffer[*startpos & LOGBUFMASK];

    // Calculate number of bytes available
    uint32_t len;
    if ((g_logpos & LOGBUFMASK) >= (*startpos & LOGBUFMASK))
    {
        // Can read directly to g_logpos
        len = g_logpos - *startpos;
    }
    else
    {
        // Buffer wraps, read to end of buffer now and start from beginning on next call.
        len = LOGBUFSIZE - (*startpos & LOGBUFMASK);
    }
    mutex_exit(&log_mutex);
    if (available) { *available = len; }
    *startpos += len;

    return result;
}


void log_poll()
{
    static uint32_t log_pos = 0;
    // Retrieve pointer to log start and determine number of bytes available.
    uint32_t available = 0;
    const char *data = log_get_buffer(&log_pos, &available);
    uint32_t len = available;
    if (len == 0) return;

    // Update log position by the actual number of bytes sent
    uint32_t actual = 0;
    actual = Serial1.write(data, len);
    log_pos -= available - actual;
}