//---------------------------------------------------------------------------
//
//	ADBuino & QuokkADB are ADB keyboard and mouse adapters
//	   Copyright (C) 2021-2022 bbraun
//	   Copyright (C) 2021-2022 akuker
//     Copyright (C) 2022 Rabbit Hole Computing LLC
//
//  Based upon the virtual keycodes provided at:
//    http://meandmark.com/keycodes.html
//
//  Also refer to the Guide to the Macintosh family hardware, figure 8-10:
//    https://archive.org/details/apple-guide-macintosh-family-hardware/page/n345/mode/2up
//
//  This file is part of both the ADBuino and  the QuokkADB projects.
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
//---------------------------------------------------------------------------

#include "usb_hid_keys.h"
#include "next_5pin_keys.h"
#include <stdint.h>
#include <Arduino.h>
// #ifdef ADBUINO
// #include <Arduino.h>
#ifdef QUOKKADB

#endif
#include <platform_logmsg.h>

extern bool global_debug;

// Virtual Keycodes for the NeXT keyboard layout
// Keycodes are in hexadecimal.
uint8_t usb_keycode_to_n5p_code(uint8_t usb_code)
{
    switch (usb_code)
    {
    case USB_KEY_A:
        return N5P_KEYCODE_A;
    case USB_KEY_S:
        return N5P_KEYCODE_S;
    case USB_KEY_D:
        return N5P_KEYCODE_D;
    case USB_KEY_F:
        return N5P_KEYCODE_F;
    case USB_KEY_H:
        return N5P_KEYCODE_H;
    case USB_KEY_G:
        return N5P_KEYCODE_G;
    case USB_KEY_Z:
        return N5P_KEYCODE_Z;
    case USB_KEY_X:
        return N5P_KEYCODE_X;
    case USB_KEY_C:
        return N5P_KEYCODE_C;
    case USB_KEY_V:
        return N5P_KEYCODE_V;
    case USB_KEY_B:
        return N5P_KEYCODE_B;
    case USB_KEY_Q:
        return N5P_KEYCODE_Q;
    case USB_KEY_W:
        return N5P_KEYCODE_W;
    case USB_KEY_E:
        return N5P_KEYCODE_E;
    case USB_KEY_R:
        return N5P_KEYCODE_R;
    case USB_KEY_Y:
        return N5P_KEYCODE_Y;
    case USB_KEY_T:
        return N5P_KEYCODE_T;
    case USB_KEY_1:
        return N5P_KEYCODE_1;
    case USB_KEY_2:
        return N5P_KEYCODE_2;
    case USB_KEY_3:
        return N5P_KEYCODE_3;
    case USB_KEY_4:
        return N5P_KEYCODE_4;
    case USB_KEY_6:
        return N5P_KEYCODE_6;
    case USB_KEY_5:
        return N5P_KEYCODE_5;
    case USB_KEY_EQUAL:
        return N5P_KEYCODE_EQUAL;
    case USB_KEY_9:
        return N5P_KEYCODE_9;
    case USB_KEY_7:
        return N5P_KEYCODE_7;
    case USB_KEY_MINUS:
        return N5P_KEYCODE_MINUS;
    case USB_KEY_8:
        return N5P_KEYCODE_8;
    case USB_KEY_0:
        return N5P_KEYCODE_0;
    case USB_KEY_RIGHTBRACE:
        return N5P_KEYCODE_RBRACKET;
    case USB_KEY_O:
        return N5P_KEYCODE_O;
    case USB_KEY_U:
        return N5P_KEYCODE_U;
    case USB_KEY_LEFTBRACE:
        return N5P_KEYCODE_LBRACKET;
    case USB_KEY_I:
        return N5P_KEYCODE_I;
    case USB_KEY_P:
        return N5P_KEYCODE_P;
    case USB_KEY_ENTER:
        return N5P_KEYCODE_ENTER;
    case USB_KEY_L:
        return N5P_KEYCODE_L;
    case USB_KEY_J:
        return N5P_KEYCODE_J;
    case USB_KEY_APOSTROPHE:
        return N5P_KEYCODE_QUOTE;
    case USB_KEY_K:
        return N5P_KEYCODE_K;
    case USB_KEY_SEMICOLON:
        return N5P_KEYCODE_SEMICOLON;
    case USB_KEY_BACKSLASH:
        return N5P_KEYCODE_BACKSLASH;
    case USB_KEY_COMMA:
        return N5P_KEYCODE_COMMA;
    case USB_KEY_SLASH:
        return N5P_KEYCODE_SLASH;
    case USB_KEY_N:
        return N5P_KEYCODE_N;
    case USB_KEY_M:
        return N5P_KEYCODE_M;
    case USB_KEY_DOT:
        return N5P_KEYCODE_PERIOD;
    case USB_KEY_TAB:
        return N5P_KEYCODE_TAB;
    case USB_KEY_SPACE:
        return N5P_KEYCODE_SPACE;
    case USB_KEY_GRAVE:
        return N5P_KEYCODE_KPGRAVE;
    case USB_KEY_BACKSPACE:
        return N5P_KEYCODE_DELETE;
    case USB_KEY_ESC:
        return N5P_KEYCODE_ESC;
    case USB_KEY_CAPSLOCK:
        return N5P_KEYCODE_CAPSLOCK;
    case USB_KEY_LEFT:
        return N5P_KEYCODE_LARROW;
    case USB_KEY_RIGHT:
        return N5P_KEYCODE_RARROW;
    case USB_KEY_DOWN:
        return N5P_KEYCODE_DOWNARROW;
    case USB_KEY_UP:
        return N5P_KEYCODE_UPARROW;
    case USB_KEY_KPDOT:
        return N5P_KEYCODE_KPDOT;
    case USB_KEY_KPASTERISK:
        return N5P_KEYCODE_KPASTERISK;
    case USB_KEY_KPPLUS:
        return N5P_KEYCODE_KPPLUS;
    case USB_KEY_NUMLOCK: 
        return N5P_KEYCODE_NUMLOCK;
    case USB_KEY_KPSLASH:
        return N5P_KEYCODE_KPSLASH;
    case USB_KEY_KPENTER:
        return N5P_KEYCODE_KPENTER;
    case USB_KEY_KPMINUS:
        return N5P_KEYCODE_KPMINUS;
    case USB_KEY_KPEQUAL:
        return N5P_KEYCODE_KPEQUAL;
    case USB_KEY_KP0:
        return N5P_KEYCODE_KP0;
    case USB_KEY_KP1:
        return N5P_KEYCODE_KP1;
    case USB_KEY_KP2:
        return N5P_KEYCODE_KP2;
    case USB_KEY_KP3:
        return N5P_KEYCODE_KP3;
    case USB_KEY_KP4:
        return N5P_KEYCODE_KP4;
    case USB_KEY_KP5:
        return N5P_KEYCODE_KP5;
    case USB_KEY_KP6:
        return N5P_KEYCODE_KP6;
    case USB_KEY_KP7:
        return N5P_KEYCODE_KP7;
    case USB_KEY_KP8:
        return N5P_KEYCODE_KP8;
    case USB_KEY_KP9:
        return N5P_KEYCODE_KP9;
    case USB_KEY_F2:
        return N5P_KEYCODE_VOLDOWN;
    case USB_KEY_F3:
        return N5P_KEYCODE_VOLUP;
    case USB_KEY_F11:
        return N5P_KEYCODE_BRIGHTDOWN;
    case USB_KEY_F12:
        return N5P_KEYCODE_BRIGHTUP;
    case USB_KEY_F1:
        return N5P_KEYCODE_MUTE;


/* Power key trapped at USB scancode level
    case USB_KEY_F15:
    case USB_KEY_INSERT: // fall through to Help
    case USB_KEY_HELP:
        return N5P_KEYCODE_POWER;
*/

/* Currently not assigned to anything
     case USB_KEY_F5:
        return 0x60;
    case USB_KEY_F8:
        return 0x64;
    case USB_KEY_F9:
        return 0x65;
    case USB_KEY_F6:
        return 0x61;
    case USB_KEY_F7:
        return 0x62;
    case USB_KEY_SCROLLLOCK: // fall through to F14
    case USB_KEY_F14:
        return 0x6B;
    case USB_KEY_F10:
        return 0x6D;
    case USB_KEY_PAUSE:
        return 0x71;
        case USB_KEY_HOME:
        return 0x73;
    case USB_KEY_PAGEUP:
        return 0x74;
    case USB_KEY_DELETE:
        return 0x75;
    case USB_KEY_F4:
        return 0x76;
    case USB_KEY_END:
        return 0x77;

    case USB_KEY_PAGEDOWN:
        return 0x79; */

    // Use PrntScr/Sysrq key or F13 key as Apple Power key
    case USB_KEY_SYSRQ: // fall through to F13
    case USB_KEY_F13:
        return N5P_KEYCODE_POWER;
    case USB_KEY_RIGHTCTRL:
    case USB_KEY_LEFTCTRL:
        return N5P_KEYCODE_CONTROL;
    case USB_KEY_RIGHTSHIFT:
        return N5P_KEYCODE_RSHIFT;
    case USB_KEY_LEFTSHIFT:
        return N5P_KEYCODE_LSHIFT;
    case USB_KEY_RIGHTALT:
        return N5P_KEYCODE_RCOMMAND;        
    case USB_KEY_LEFTALT:
        return N5P_KEYCODE_LCOMMAND;
    case USB_KEY_RIGHTMETA: // fall through to LEFTMETA
        return N5P_KEYCODE_RALT;
    case USB_KEY_LEFTMETA:
        return N5P_KEYCODE_LALT;


    default:
        return N5P_KEYCODE_NONE;
    }
}