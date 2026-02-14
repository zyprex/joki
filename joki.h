#pragma once
#include <xinput.h>
#include <stdio.h>
#include <synchapi.h> /* Sleep() */
#include <WinError.h> /* ERROR_SUCCESS */
#include <wincon.h> /* SetConsoleTitle */
#include "cfg.h"
#include "inputs.h"

#ifdef DEBUG
#define DEFAULT_FRAME_TIME     100
#define DEFAULT_KEYWAIT_FRAMES 10
#define DEFAULT_KEYHOLD_FRAMES 10
#else
#define DEFAULT_FRAME_TIME     25
#define DEFAULT_KEYWAIT_FRAMES 12
#define DEFAULT_KEYHOLD_FRAMES 20
#endif

#define DEFAULT_DEAD_ZONE         4000
#define DEFAULT_TRIGGER_THRESHOLD 60
#define DEFAULT_MINDIV 600
#define DEFAULT_MAXDIV 1800

#define ABS(a) ((a)>=0?(a):(-(a)))
#define IN_DEAD_ZONE(x, y, z) ((x)*(x)+(y)*(y) < z*z)

#define APP_NAME "joki"
#define APP_VERSION "v0.6-beta"

/* Controller key State */
typedef struct CKState_t {
  int down; /* key down = 1, up = 0 */
  int repeat; /* repeat counter */
  int hold; /* hold time */
  int inRepeat; /* enter repeat */
} CKState;


#define COND_A (buttons & XINPUT_GAMEPAD_A)
#define COND_B (buttons & XINPUT_GAMEPAD_B)
#define COND_X (buttons & XINPUT_GAMEPAD_X)
#define COND_Y (buttons & XINPUT_GAMEPAD_Y)
#define COND_L (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER)
#define COND_R (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER)


#define COND_DPAD_UP    (buttons & XINPUT_GAMEPAD_DPAD_UP)
#define COND_DPAD_DOWN  (buttons & XINPUT_GAMEPAD_DPAD_DOWN)
#define COND_DPAD_RIGHT (buttons & XINPUT_GAMEPAD_DPAD_RIGHT)
#define COND_DPAD_LEFT  (buttons & XINPUT_GAMEPAD_DPAD_LEFT)
#define COND_LS (buttons & XINPUT_GAMEPAD_LEFT_THUMB)
#define COND_RS (buttons & XINPUT_GAMEPAD_RIGHT_THUMB)


#define COND_LT (state.Gamepad.bLeftTrigger > gConfigTriggerThreshold)
#define COND_RT (state.Gamepad.bRightTrigger > gConfigTriggerThreshold)


#define COND_START (buttons & XINPUT_GAMEPAD_START)
#define COND_BACK (buttons & XINPUT_GAMEPAD_BACK)

#define COND_CAN_MOVE_MOUSE \
  (lr == 'L' && iv_LS_MOUSE_MOVE) || \
  (lr == 'R' && iv_RS_MOUSE_MOVE)

#define COND_CAN_MOVE_WINDOW \
  (lr == 'L' && iv_LS_WINDOW_MOVE) || \
  (lr == 'R' && iv_RS_WINDOW_MOVE)

/* Macro Expand Example:
 * KEY_X_REG(A) =>
 * key_complex_register(KL_A, COND_A, gVarIdleFrame, &gpA); */
#define KEY_X_REG(lb) \
  key_complex_register(KL_##lb, COND_##lb, gVarIdleFrame, &(gp##lb));
/* Macro Expand Example:
 * KEY_XY_REG(A,B) =>
 * key_complex_register(KL_A, COND_B, gVarIdleFrame, &gpB); */
#define KEY_XY_REG(lb,lbr) \
  key_complex_register(KL_##lb, COND_##lbr, gVarIdleFrame, &(gp##lbr));


void init_running_state();

int load_config_file_mode(KLabel lb);
void simulate_keydown(KLabel lb, short type);
void simulate_keyup(KLabel lb, short type);
void simulate_keypress(KLabel lb, short type);
void simulate_keystrokes(KLabel lb, short n);
void simulate_downup_or_load_cfg(KLabel lb);
void key_complex_register(KLabel lb, int isDown, int idleFrame, CKState* stat);
void thumbstick_move_register(const char lr, int idx, int isMove);
void calc_tilted_xy(int* x, int* y);
void thumbstick_register(const char lr, short x, short y);

int xusers_seek();
void xusers_loop();

void load_config_file(char* fname);
void cleanup();

#define HAS_ARG(s1, s2) (!strcmp(s1, argv[i]) || !strcmp(s2, argv[i]))
#define NORM_RANGE(a,x,y,d) (((a)>=(x)&&(a)<=(y))?(a):(d))

void parse_cmdargs(int argc, char* argv[]);
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

extern int gModeCfg;

