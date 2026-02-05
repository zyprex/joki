#pragma once

#include <stdio.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 /* MOUSEEVENTF_HWHEEL */
#include <windef.h>
#include <winuser.h> /* SendInput */
#include "joki.h"
#include "log.h"


#define KEYDOWN(vk) (GetKeyState(vk) & 0x8000)

void input_event(INPUT* inps);
void mice_event(short dx, short dy, DWORD mouseData, DWORD dwFlags);
void kbd_event(WORD wVk, DWORD dwFlags);
void mice_move_abs(unsigned short x, unsigned short y);
void mice_move(short dx, short dy);
void mice_wheel(short span);
void mice_hwheel(short span);
void key_use_mod(WORD mod, DWORD dwFlags);
void key_down(WORD vk, WORD mod);
void key_up(WORD vk, WORD mod);
void key_press(WORD vk, WORD mod);
void key_toggle(WORD vk, WORD mod);
void key_translate(WORD vk, WORD mod, int down);
void key_translate_all(WORD vk, WORD mod);

#include "morse.h"
WORD key_char(char ch);
void morse_char();
void morse_char_repeat();

#include "wnd.h"

void key_translate_ex(WORD vk, WORD mod);

