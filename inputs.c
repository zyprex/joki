#include "inputs.h"

char morseCodeChar;

void input_event(INPUT* inps) {
  UINT uSend = SendInput(1, inps, sizeof(INPUT));
  if (!uSend) {
    printf("Simulate input failed!\n");
  }
}

void mice_event(short dx, short dy, DWORD mouseData, DWORD dwFlags) {
  INPUT inps = {};
  inps.type = INPUT_MOUSE;
  inps.mi.dx = dx;
  inps.mi.dy = dy;
  inps.mi.mouseData = mouseData;
  inps.mi.dwFlags = dwFlags;
  input_event(&inps);
}

void kbd_event(WORD wVk, DWORD dwFlags) {
  INPUT inps = {};
  inps.type = INPUT_KEYBOARD;
  inps.ki.wVk = wVk;
  inps.ki.dwFlags = dwFlags;
  input_event(&inps);
}

/* 0~65535 */
void mice_move_abs(unsigned short x, unsigned short y) {
  mice_event(x, y, 0, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE);
}

void mice_move(short dx, short dy) {
  mice_event(dx, dy, 0, MOUSEEVENTF_MOVE);
}

void mice_wheel(short span) {
  mice_event(0, 0, span, MOUSEEVENTF_WHEEL);
}
void mice_hwheel(short span) {
  mice_event(0, 0, span, MOUSEEVENTF_HWHEEL);
}

void key_use_mod(WORD mod, DWORD dwFlags) {
  if (mod > 0x8000) {
    short rvk = mod;
    rvk &= (0x00FF) ; /* the right side vk */
    LOG("use mod(%04X) as vk.\n", rvk);
    kbd_event(rvk, dwFlags);
    return;
  }
  if (mod & MOD_A) kbd_event(VK_MENU, dwFlags);
  if (mod & MOD_C) kbd_event(VK_CONTROL, dwFlags);
  if (mod & MOD_S) kbd_event(VK_SHIFT, dwFlags);
  if (mod & MOD_W) kbd_event(VK_LWIN, dwFlags);
}

void key_down(WORD vk, WORD mod) {
  key_use_mod(mod, 0);
  switch (vk) {
    case 0: break;
    case VK_LBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_LEFTDOWN); break;
    case VK_RBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_RIGHTDOWN); break;
    case VK_MBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_MIDDLEDOWN); break;
    case VK_XBUTTON1: mice_event(0, 0, XBUTTON1, MOUSEEVENTF_XDOWN); break;
    case VK_XBUTTON2: mice_event(0, 0, XBUTTON2, MOUSEEVENTF_XDOWN); break;
    default: kbd_event(vk, 0); break;
  }
  LOG("key(%04X) down\n", vk);
}

void key_up(WORD vk, WORD mod) {
  switch (vk) {
    case 0: break;
    case VK_LBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_LEFTUP); break;
    case VK_RBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_RIGHTUP); break;
    case VK_MBUTTON:  mice_event(0, 0, 0, MOUSEEVENTF_MIDDLEUP); break;
    case VK_XBUTTON1: mice_event(0, 0, XBUTTON1, MOUSEEVENTF_XUP); break;
    case VK_XBUTTON2: mice_event(0, 0, XBUTTON2, MOUSEEVENTF_XUP); break;
    default: kbd_event(vk, KEYEVENTF_KEYUP); break;
  }
  LOG("key(%04X) up\n", vk);
  key_use_mod(mod, KEYEVENTF_KEYUP);
}


void key_press(WORD vk, WORD mod) {
  key_down(vk, mod);
  key_up(vk, mod);
}

void key_toggle(WORD vk, WORD mod) {
  if (KEYDOWN(vk)) {
    key_up(vk, mod);
  }
  else {
    key_down(vk, mod);
  }
}

void key_translate(WORD vk, WORD mod, int down) {
  if (!vk && !mod) {
    return;
  }
  if (down) {
    if (vk >= 0x00FF) {
      /* Use reserved code to execute all other functions,
       * in this case, the modifier are still has effect. */
      key_translate_ex(vk, mod);
    }
    else {
      key_down(vk, mod);
    }
  }
  else {
    key_up(vk, mod);
  }
}

void key_translate_all(WORD vk, WORD mod) {
  key_translate(vk, mod, 1);
  key_translate(vk, mod, 0);
}

/* Send printable characters vkcode */
WORD key_char(char ch) {
  WORD vk = 0;
  if (ch >= 'a' && ch <= 'z') {
    vk = (ch-97)+0x41;
  }
  if (ch >= '0' && ch <= '9') {
    vk = (ch-48)+0x30;
  }
  return vk;
}

void morse_char() {
  morseCodeChar = morse_compose();
  key_press(key_char(morseCodeChar), 0);
  morse_clear();
}

void morse_char_repeat() {
  key_press(key_char(morseCodeChar), 0);
}

void key_translate_ex(WORD vk, WORD mod) {
  switch (vk) {
    /* NOTE: See 'cfg_ex_vk.def', use same vkcode there. */
    case 0x00FF: { gModeCfg = 1; }; break; /* cfg_mode */
    case 0x0100: mice_wheel(mod); break; /* wheel_scroll */
    case 0x0101: mice_hwheel(mod); break; /* hwheel_scroll */
    case 0x0102: mice_move(mod, 0); break;  /*mouse_move_x */
    case 0x0103: mice_move(0, mod); break;  /*mouse_move_y */
    case 0x0104: mice_move_abs(32767,32767); break;  /*mouse_to_center */

    case 0x0200: morse_send(mod); break;  /* morse_send */
    case 0x0201: morse_clear(); break;  /* morse_clear */
    case 0x0202: morse_char(); break;  /* morse_char */
    case 0x0203: morse_char_repeat(); break;  /* morse_char_repeat */

    case 0x0300: window_hide_show(); break; /* window_hide_show */
    case 0x0301: window_topmost(); break; /* window_topmost */
    case 0x0302: window_resize(16.0/9.0, mod); break; /* window16:9_width */
    case 0x0303: window_resize(4.0/3.0, mod); break; /* window4:3_width */
    case 0x0304: window_move(mod, 0); break; /* window_move_x */
    case 0x0305: window_move(0, mod); break; /* window_move_y */
    case 0x0306: screen_off(); break; /* screen_off */
    case 0x0307: runlnk_async("open", mod); break; /* runlnk */
    case 0x0308: runlnk_async("runas", mod); break; /* runlnk_admin */
  }
}
