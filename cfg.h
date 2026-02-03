#pragma once
#include <string.h>
#include <stdio.h>
#include <ctype.h> /* isspace() */
#include <stdlib.h>

#include <windef.h>
#include <winuser.h>


#define MOD_A 0x0001 /*Alt*/
#define MOD_C 0x0002 /*ctrl*/
#define MOD_S 0x0004 /*shift*/
#define MOD_W 0x0008 /*win*/

#define VK_GAMEPAD_A                       0xC3 /*Gamepad A button*/
#define VK_GAMEPAD_B                       0xC4 /*Gamepad B button*/
#define VK_GAMEPAD_X                       0xC5 /*Gamepad X button*/
#define VK_GAMEPAD_Y                       0xC6 /*Gamepad Y button*/
#define VK_GAMEPAD_RIGHT_SHOULDER          0xC7 /*Gamepad Right Shoulder button*/
#define VK_GAMEPAD_LEFT_SHOULDER           0xC8 /*Gamepad Left Shoulder button*/
#define VK_GAMEPAD_LEFT_TRIGGER            0xC9 /*Gamepad Left Trigger button*/
#define VK_GAMEPAD_RIGHT_TRIGGER           0xCA /*Gamepad Right Trigger button*/
#define VK_GAMEPAD_DPAD_UP                 0xCB /*Gamepad D-pad Up button*/
#define VK_GAMEPAD_DPAD_DOWN               0xCC /*Gamepad D-pad Down button*/
#define VK_GAMEPAD_DPAD_LEFT               0xCD /*Gamepad D-pad Left button*/
#define VK_GAMEPAD_DPAD_RIGHT              0xCE /*Gamepad D-pad Right button*/
#define VK_GAMEPAD_MENU                    0xCF /*Gamepad Menu/Start button*/
#define VK_GAMEPAD_VIEW                    0xD0 /*Gamepad View/Back button*/
#define VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON  0xD1 /*Gamepad Left Thumbstick button*/
#define VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON 0xD2 /*Gamepad Right Thumbstick button*/
#define VK_GAMEPAD_LEFT_THUMBSTICK_UP      0xD3 /*Gamepad Left Thumbstick up*/
#define VK_GAMEPAD_LEFT_THUMBSTICK_DOWN    0xD4 /*Gamepad Left Thumbstick down*/
#define VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT   0xD5 /*Gamepad Left Thumbstick right*/
#define VK_GAMEPAD_LEFT_THUMBSTICK_LEFT    0xD6 /*Gamepad Left Thumbstick left*/
#define VK_GAMEPAD_RIGHT_THUMBSTICK_UP     0xD7 /*Gamepad Right Thumbstick up*/
#define VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN   0xD8 /*Gamepad Right Thumbstick down*/
#define VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT  0xD9 /*Gamepad Right Thumbstick right*/
#define VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT   0xDA /*Gamepad Right Thumbstick left*/

/* Item structure for one line config */
typedef struct Cfg_t {
  short label; /* key name label */
  short type; /* type code */
  short vk; /* virtual keycode */
  short mod; /* modifier mask */
} Cfg;

/* key labels */
extern const char* labelName[];
typedef enum KLabel_e {
  KL_A = 1, KL_B, KL_X, KL_Y,
  KL_L, KL_R, KL_LT, KL_RT,
  KL_DPAD_UP, KL_DPAD_DOWN, KL_DPAD_RIGHT, KL_DPAD_LEFT,
  KL_LS, KL_RS,
  KL_LS_UP, KL_LS_DOWN, KL_LS_RIGHT, KL_LS_LEFT,
  KL_RS_UP, KL_RS_DOWN, KL_RS_RIGHT, KL_RS_LEFT,
  KL_START, KL_BACK,
  KL_LS_MOUSE_MOVE,
  KL_RS_MOUSE_MOVE,
  KL_LS_WINDOW_MOVE,
  KL_RS_WINDOW_MOVE,
} KLabel;
/* key types */
#define KT_DOUBLE_TAP 2
#define KT_SINGLE_TAP 1
#define KT_DOWN_UP    0
#define KT_LONG_PRESS -1
#define KT_REPEAT -2
#define KT_TOGGLE -3
#define KT_OPTION -4

/* Item structure for map name to virtual code */
typedef struct VCode_t {
  char* name; /* key name string */
  short code; /* virtual code */
} VCode;

#define CFG_IS_TOGGLE(x) (cfg_get_vk(cfg_search(x,KT_TOGGLE)))
#define CFG_CAN_REPEAT(x) (cfg_get_vk(cfg_search(x,KT_REPEAT)))
#define CFG_TURN_ON(x) (cfg_get_vk(cfg_search(x,KT_OPTION)))

short cfg_get_mod(int idx);
short cfg_get_vk(int idx);
int cfg_search(short label, short type);
void cfg_unload();
int cfg_load(char* fname);
