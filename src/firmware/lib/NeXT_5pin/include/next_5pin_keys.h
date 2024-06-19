//----------------------------------------------------------------------------
//
//  ADBuino ADB keyboard and mouse adapter
//	   Copyright (C) 2021-2022 bbraun
//	   Copyright (C) 2021-2022 akuker
//
//  This file is part of ADBuino.
//
//  ADBuino is free software: you can redistribute it and/or modify it under 
//  the terms of the GNU General Public License as published by the Free 
//  Software Foundation, either version 3 of the License, or (at your option) 
// any later version.
//
//  ADBuino is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
//  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
//  details.
//
//  You should have received a copy of the GNU General Public License along 
//  with ADBuino. If not, see <https://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
#pragma once

#define N5P_KEYCODE_IDX 0
#define N5P_MOD_KEY_IDX 1

// These are the modifier bits for the non-adb NeXT keyboard to host protocol
// M
//  Bit   Meaning
//  7   = Not modifier only (negated when only modifier keys are pressed)
//  6   = Right Alt
//  5   = Left Alt
//  4   = Right Command
//  3   = Left Command
//  2   = Right Shift
//  1   = Left Shift
//  0   = Control

#define N5P_MOD_NOT_ONLY     0x80
#define N5P_MOD_KEY_RALT     0x40
#define N5P_MOD_KEY_LALT     0x20
#define N5P_MOD_KEY_RCOMMAND 0x10
#define N5P_MOD_KEY_LCOMMAND 0x08
#define N5P_MOD_KEY_RSHIFT   0x04
#define N5P_MOD_KEY_LSHIFT   0x02
#define N5P_MOD_KEY_CONTROL  0x01


// These are the faux keycodes for matching 
#define N5P_KEYCODE_NONE     0x00
#define N5P_KEYCODE_RALT     0x51 
#define N5P_KEYCODE_LALT     0x52
#define N5P_KEYCODE_RCOMMAND 0x53
#define N5P_KEYCODE_LCOMMAND 0x54
#define N5P_KEYCODE_RSHIFT   0x55
#define N5P_KEYCODE_LSHIFT   0x56
#define N5P_KEYCODE_CONTROL  0x57
#define N5P_KEYCODE_CAPSLOCK 0x60
#define N5P_KEYCODE_NUMLOCK  0x61
#define N5P_KEYCODE_MUTE     0x62
#define N5P_KEYCODE_POWER    0x7F

// These are all the key codes for the keyboard
#define N5P_KEYCODE_BRIGHTDOWN  0x01
#define N5P_KEYCODE_VOLDOWN     0x02
#define N5P_KEYCODE_BACKSLASH   0x03
#define N5P_KEYCODE_RBRACKET    0x04
#define N5P_KEYCODE_LBRACKET    0x05
#define N5P_KEYCODE_I           0x06
#define N5P_KEYCODE_O           0x07
#define N5P_KEYCODE_P           0x08
#define N5P_KEYCODE_LARROW      0x09
// No keycode 0x0A
#define N5P_KEYCODE_KP0         0x0B
#define N5P_KEYCODE_KPDOT       0x0C
#define N5P_KEYCODE_KPENTER     0x0D
// No keycode 0x0E
#define N5P_KEYCODE_DOWNARROW   0x0F
#define N5P_KEYCODE_RARROW      0x10
#define N5P_KEYCODE_KP1         0x11
#define N5P_KEYCODE_KP4         0x12
#define N5P_KEYCODE_KP6         0x13
#define N5P_KEYCODE_KP3         0x14
#define N5P_KEYCODE_KPPLUS      0x15
#define N5P_KEYCODE_UPARROW     0x16
#define N5P_KEYCODE_KP2         0x17
#define N5P_KEYCODE_KP5         0x18
#define N5P_KEYCODE_BRIGHTUP    0x19
#define N5P_KEYCODE_VOLUP       0x1A
#define N5P_KEYCODE_DELETE      0x1B
#define N5P_KEYCODE_EQUAL       0x1C
#define N5P_KEYCODE_MINUS       0x1D
#define N5P_KEYCODE_8           0x1E
#define N5P_KEYCODE_9           0x1F
#define N5P_KEYCODE_0           0x20
#define N5P_KEYCODE_KP7         0x21
#define N5P_KEYCODE_KP8         0x22
#define N5P_KEYCODE_KP9         0x23
#define N5P_KEYCODE_KPMINUS     0x24
#define N5P_KEYCODE_KPASTERISK  0x25
#define N5P_KEYCODE_KPGRAVE     0x26
#define N5P_KEYCODE_KPEQUAL     0x27
#define N5P_KEYCODE_KPSLASH     0x28
// No keycode 0x29
#define N5P_KEYCODE_ENTER       0x2A
#define N5P_KEYCODE_QUOTE       0x2B
#define N5P_KEYCODE_SEMICOLON   0x2C
#define N5P_KEYCODE_L           0x2D
#define N5P_KEYCODE_COMMA       0x2E
#define N5P_KEYCODE_PERIOD      0x2F
#define N5P_KEYCODE_SLASH       0x30
#define N5P_KEYCODE_Z           0x31
#define N5P_KEYCODE_X           0x32
#define N5P_KEYCODE_C           0x33
#define N5P_KEYCODE_V           0x34
#define N5P_KEYCODE_B           0x35
#define N5P_KEYCODE_M           0x36
#define N5P_KEYCODE_N           0x37
#define N5P_KEYCODE_SPACE       0x38
#define N5P_KEYCODE_A           0x39
#define N5P_KEYCODE_S           0x3A
#define N5P_KEYCODE_D           0x3B
#define N5P_KEYCODE_F           0x3C
#define N5P_KEYCODE_G           0x3D
#define N5P_KEYCODE_K           0x3E
#define N5P_KEYCODE_J           0x3F
#define N5P_KEYCODE_H           0x40
#define N5P_KEYCODE_TAB         0x41
#define N5P_KEYCODE_Q           0x42
#define N5P_KEYCODE_W           0x43
#define N5P_KEYCODE_E           0x44
#define N5P_KEYCODE_R           0x45
#define N5P_KEYCODE_U           0x46
#define N5P_KEYCODE_Y           0x47
#define N5P_KEYCODE_T           0x48
#define N5P_KEYCODE_ESC         0x49
#define N5P_KEYCODE_1           0x4A
#define N5P_KEYCODE_2           0x4B
#define N5P_KEYCODE_3           0x4C
#define N5P_KEYCODE_4           0x4D
#define N5P_KEYCODE_7           0x4E
#define N5P_KEYCODE_6           0x4F
#define N5P_KEYCODE_5           0x50
