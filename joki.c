#include "joki.h"
CKState gpA;
CKState gpB;
CKState gpX;
CKState gpY;
CKState gpL;
CKState gpR;
CKState gpLT;
CKState gpRT;
CKState gpDPAD_UP;
CKState gpDPAD_DOWN;
CKState gpDPAD_RIGHT;
CKState gpDPAD_LEFT;
CKState gpLS;
CKState gpRS;
CKState gpSTART;
CKState gpBACK;

DWORD xusers[XUSER_MAX_COUNT];

int gVarIdleFrame;

int frameTime = 100; /* per frame time in ms */
int keyWaitFrames = 10; /* time to wait the multiple presses */
int keyHoldFrames = 10; /* time to trigger long press */

int gConfigDeadZone = 4000;
int gConfigTriggerThreshold = 60;
int gConfigSwapABXY = 0;

int gModeCfg = 0; /* flag for switch cfg mode */

double eqm = 1.0; /* Mult factor */
int eqa = 0; /* Add factor */

/* initial vars for running state */
int iv_LS_MOUSE_MOVE = 0;
int iv_RS_MOUSE_MOVE = 0;
int iv_LS_WINDOW_MOVE = 0;
int iv_RS_WINDOW_MOVE = 0;
#define TITL_MODE_L VK_LEFT
#define TITL_MODE_R VK_RIGHT
int iv_TITL_MODE = 0;

void init_running_state() {
  iv_LS_MOUSE_MOVE = CFG_TURN_ON(KL_LS_MOUSE_MOVE);
  iv_RS_MOUSE_MOVE = CFG_TURN_ON(KL_RS_MOUSE_MOVE);
  iv_LS_WINDOW_MOVE = CFG_TURN_ON(KL_LS_WINDOW_MOVE);
  iv_RS_WINDOW_MOVE = CFG_TURN_ON(KL_RS_WINDOW_MOVE);
  iv_TITL_MODE = CFG_TURN_ON(KL_TILT_MODE);
}

/* d = thumbstick's x or y */
int stick2client(int d) {
  return d/(ABS(d)*eqm+eqa);
}

/* Calc the divider, it's scaling the thumbstick's x,y
 * to normal range */
void calcDiv(int minDiv, int maxDiv) {
  eqm = (double)(minDiv - maxDiv) / (double)(32767 - gConfigDeadZone);
  eqa = maxDiv - gConfigDeadZone * eqm;
}

int load_config_file_mode(KLabel lb) {
  if (gModeCfg) {
    load_config_file((char*)labelName[lb]);
    gModeCfg = 0;
    return 1;
  }
  return 0;
}

void simulate_keydown(KLabel lb, short type) {
  int idx = cfg_search(lb, type);
  if (CFG_IS_TOGGLE(lb)) {
    short vk = cfg_get_vk(idx), mod = cfg_get_mod(idx);
    key_toggle(vk, mod);
    printf("toggle key: %04X(%04X) %s\n", vk, mod, KEYDOWN(vk) ? "on" : "off");
  }
  else {
    key_translate(cfg_get_vk(idx), cfg_get_mod(idx), 1);
  }
}

void simulate_keyup(KLabel lb, short type) {
  int idx = cfg_search(lb, type);
  if (cfg_get_vk(cfg_search(lb, KT_TOGGLE))) {
    /* key_toggle(cfg_get_vk(idx), cfg_get_mod(idx)); */
  }
  else {
    key_translate(cfg_get_vk(idx), cfg_get_mod(idx), 0);
  }
}

void simulate_keypress(KLabel lb, short type) {
  int idx = cfg_search(lb, type);
  key_translate_all(cfg_get_vk(idx), cfg_get_mod(idx));
}

void simulate_keystrokes(KLabel lb, short n) {
  int idx = cfg_search(lb, KT_TAP_THRU);
  short vk = cfg_get_vk(idx) - 0x00FF;
  short mod = cfg_get_mod(idx);
  if (vk < 0 || !mod || vk != n) {
    return;
  }
  if (mod > 0) {
    n++;
    int end = n + mod;
    while (n < end) {
      simulate_keypress(lb, n++);
    }
  }
  else {
    n--;
    int start = n + mod;
    while (n > start) {
      simulate_keypress(lb, n--);
    }
  }
}

void simulate_downup_or_load_cfg(KLabel lb) {
  if (!load_config_file_mode(lb)) {
    simulate_keydown(lb, KT_DOWN_UP);
  }
  else {
    /* Reset the idle frame to max ensure the
     * short tap not to executed by accident.  */
    gVarIdleFrame = 256;
  }
}

void key_complex_register(KLabel lb, int isDown, int idleFrame, CKState* stat) {
  if (stat->inRepeat) {
    LOG("%s in repeat\n", labelName[lb]);
    simulate_keypress(lb, KT_LONG_PRESS);
  }
  if (idleFrame == keyWaitFrames) {
    if (stat->repeat > 0) {
      LOG("%s--press%2d\n", labelName[lb], stat->repeat);
      simulate_keypress(lb, stat->repeat);
      simulate_keystrokes(lb, stat->repeat);
      stat->repeat = 0;
      stat->hold = 0;
    }
  }
  if (stat->hold == keyHoldFrames) {
    LOG("%s--press long\n", labelName[lb]);
    simulate_keypress(lb, KT_LONG_PRESS);
    stat->repeat = 0;
    if (CFG_CAN_REPEAT(lb)) {
      /* stat->hold = 0; */
      stat->inRepeat = 1;
    }
  }
  if (isDown) {
    stat->hold++;
  }
  if (isDown && !stat->down) {
    LOG("(%s)down\n", labelName[lb]);
    simulate_downup_or_load_cfg(lb);
    stat->down = 1;
    if (idleFrame < keyWaitFrames) {
      stat->repeat++;
    }
  }
  if (!isDown && stat->down) {
    LOG("(%s)up\n", labelName[lb]);
    simulate_keyup(lb, KT_DOWN_UP);
    stat->down = 0;
    stat->hold = 0;
    stat->inRepeat = 0;
  }
}

void thumbstick_move_register(const char lr, int idx, int isMove) {
  if (!isMove) {
    return;
  }
  KLabel ldk[] = {KL_LS_UP, KL_LS_DOWN, KL_LS_RIGHT, KL_LS_LEFT};
  KLabel rdk[] = {KL_RS_UP, KL_RS_DOWN, KL_RS_RIGHT, KL_RS_LEFT};
  KLabel lb = (lr == 'L' ? ldk[idx]: rdk[idx]);
  simulate_downup_or_load_cfg(lb);
}

void calc_tilted_xy(int* x, int* y) {
  int tempX = *x;
  int tempY = *y;
  if (iv_TITL_MODE == TITL_MODE_L) {
    *x = -tempY;
    *y = -tempX;
  }
  else if (iv_TITL_MODE == TITL_MODE_R) {
    *x = tempY;
    *y = tempX;
  }
  else {
    *y = -*y;
  }
}

void thumbstick_register(const char lr, short x, short y) {
  if (IN_DEAD_ZONE(x, y, gConfigDeadZone)) {
    return;
  }
  /* -32768~32767 */
  LOG("%cS(X,Y) = (%d,%d)\n", lr, x, y);
  if (COND_CAN_MOVE_MOUSE) {
    int mX = stick2client(x);
    int mY = stick2client(y);
    LOG("mouse_xy(%d,%d) \n", mX, -mY);
    calc_tilted_xy(&mX, &mY);
    mice_move(mX, mY);
    if (!gModeCfg) { /* walk through */
      return;
    }
  }
  if (COND_CAN_MOVE_WINDOW) {
    int mX = x/256;
    int mY = y/256;
    LOG("window_xy(%d,%d) \n", mX, -mY);
    calc_tilted_xy(&mX, &mY);
    window_move(mX, mY);
    if (!gModeCfg) { /* walk through */
      return;
    }
  }
  int absX = ABS(x);
  int absY = ABS(y);
  if (iv_TITL_MODE == TITL_MODE_L) {
    thumbstick_move_register(lr,3,(y > 0 && absY > absX));
    thumbstick_move_register(lr,2,(y < 0 && absY > absX));
    thumbstick_move_register(lr,0,(x > 0 && absX > absY));
    thumbstick_move_register(lr,1,(x < 0 && absX > absY));
  }
  else if (iv_TITL_MODE == TITL_MODE_R) {
    thumbstick_move_register(lr,2,(y > 0 && absY > absX));
    thumbstick_move_register(lr,3,(y < 0 && absY > absX));
    thumbstick_move_register(lr,1,(x > 0 && absX > absY));
    thumbstick_move_register(lr,0,(x < 0 && absX > absY));
  }
  else {
    thumbstick_move_register(lr,0,(y > 0 && absY > absX));
    thumbstick_move_register(lr,1,(y < 0 && absY > absX));
    thumbstick_move_register(lr,2,(x > 0 && absX > absY));
    thumbstick_move_register(lr,3,(x < 0 && absX > absY));
  }
}

int xusers_seek() {
  int cnt = 0;
  XINPUT_STATE state;
  DWORD dwResult;
  for (DWORD i=0; i< XUSER_MAX_COUNT; i++ ) {
    memset(&state, 0, sizeof(XINPUT_STATE));
    dwResult = XInputGetState(i, &state);
    if(dwResult == ERROR_SUCCESS) {
      printf("user(%ld) connected!\n", i);
      xusers[i] = 1;
      cnt++;
    }
  }
  return cnt;
}

void xusers_loop() {
  XINPUT_STATE state;
  DWORD dwResult;
  DWORD dwPacketNumber;
  for (DWORD i=0; i< XUSER_MAX_COUNT; i++) {
    if (!xusers[i]) {
      continue;
    }
    memset(&state, 0, sizeof(XINPUT_STATE));
    dwResult = XInputGetState(i, &state);
    if (dwResult != ERROR_SUCCESS) {
      /* user disconnected */
      /* xusers[i] = 0; */
      continue;
    }

    WORD buttons = state.Gamepad.wButtons;
    if (buttons) {
      gVarIdleFrame = 0;
    }
    if (gVarIdleFrame < 256) {
      gVarIdleFrame++;
    }

    KEY_X_REG(START)
    KEY_X_REG(BACK)
    KEY_X_REG(L);
    KEY_X_REG(R);
    KEY_X_REG(LS);
    KEY_X_REG(RS);
    KEY_X_REG(LT);
    KEY_X_REG(RT);

    if (iv_TITL_MODE == TITL_MODE_L) {
      KEY_XY_REG(DPAD_UP,   DPAD_RIGHT);
      KEY_XY_REG(DPAD_DOWN, DPAD_LEFT);
      KEY_XY_REG(DPAD_RIGHT,DPAD_DOWN);
      KEY_XY_REG(DPAD_LEFT, DPAD_UP);
    }
    else if (iv_TITL_MODE == TITL_MODE_R) {
      KEY_XY_REG(DPAD_UP,   DPAD_LEFT);
      KEY_XY_REG(DPAD_DOWN, DPAD_RIGHT);
      KEY_XY_REG(DPAD_RIGHT,DPAD_UP);
      KEY_XY_REG(DPAD_LEFT, DPAD_DOWN);
    }
    else {
      KEY_X_REG(DPAD_UP);
      KEY_X_REG(DPAD_DOWN);
      KEY_X_REG(DPAD_RIGHT);
      KEY_X_REG(DPAD_LEFT);
    }

    if (gConfigSwapABXY) {
      KEY_XY_REG(A,B);
      KEY_XY_REG(B,A);
      KEY_XY_REG(X,Y);
      KEY_XY_REG(Y,X);
    }
    else {
      KEY_X_REG(A);
      KEY_X_REG(B);
      KEY_X_REG(X);
      KEY_X_REG(Y);
    }

    if (state.dwPacketNumber == dwPacketNumber) {
      /* Controller is not change */
      continue;
    }
    dwPacketNumber = state.dwPacketNumber;

    thumbstick_register('L', state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
    thumbstick_register('R', state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
  }
}

void load_config_file(char* fname) {
  char fullpath[256];
  sprintf(fullpath, "configs/%s.ini", fname);
  if (!cfg_load(fullpath)) {
    init_running_state();
  }
}

void cleanup() {
  cfg_unload();
  cleanup_window();
  printf("program end.\n");
}

void parse_cmdargs(int argc, char* argv[]) {
  int hasArgC = 0;
  int minDiv = 600;
  int maxDiv = 1800;
#define HAS_ARG(s1, s2) (!strcmp(s1, argv[i]) || !strcmp(s2, argv[i]))
#define NORM_RANGE(a,x,y,d) (((a)>=(x)&&(a)<=(y))?(a):(d))
  for (int i = 1; i < argc; ++i) {
    if (HAS_ARG("-h", "--help")) {
      printf(
          "Usage:\n"
          "-h | --help : show this info page.\n"
          "-e | --tune-time [INT]:[INT]:[INT] | frame-delay:multiple-tap-frames:long-press-frames.\n"
          "-c | --config [STR]                | load specific config file from 'configs' dir.\n"
          "-d | --dead-zone [INT]             | thumbsticks deadzone (0~32767).\n"
          "-t | --trigger-threshold [INT]     | LR trigger threshold (0~255).\n"
          "-a | --swap-abxy [BOOL]            | swap A<->B, X<->Y.\n"
          "-m | --mouse-damp [INT]:[INT]      | min:max damp factors to control mouse speed.\n"
          );
      exit(0);
    }
    if (HAS_ARG("-c", "--config") && i < argc) {
      hasArgC = 1;
      load_config_file(argv[i+1]);
    }
    if (HAS_ARG("-e", "--tune-time") && i < argc) {
      int a, b, c;
      sscanf(argv[i+1], "%d:%d:%d", &a, &b, &c);
      frameTime     = NORM_RANGE(a,25,255,100);
      keyWaitFrames = NORM_RANGE(b, 1,255, 10);
      keyHoldFrames = NORM_RANGE(c, 1,255, 10);
    }
    if (HAS_ARG("-d", "--dead-zone") && i < argc) {
      gConfigDeadZone = atoi(argv[i+1]);
    }
    if (HAS_ARG("-t", "--trigger-threshold") && i < argc) {
      gConfigTriggerThreshold = atoi(argv[i+1]);
    }
    if (HAS_ARG("-a", "--swap-abxy")) {
      gConfigSwapABXY = 1;
    }
    if (HAS_ARG("-m", "--mouse-damp") && i < argc) {
      int a, b;
      sscanf(argv[i+1], "%d:%d", &a, &b);
      minDiv = NORM_RANGE(a, 1, 32767, 600);
      maxDiv = NORM_RANGE(b, 1, 32767, 1800);
    }
  }
  if (!hasArgC) {
    load_config_file("START");
  }
  calcDiv(minDiv, maxDiv);
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
  cleanup();
  return 0;
  /*
  switch (fdwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      return 0;
  }
  */
}

int main(int argc, char *argv[]) {
  /* The `atexit` can't execute ShowWindow() */
  /* if (atexit(cleanup)) { */
    /* printf("program aborted.\n"); */
    /* exit(1); */
  /* } */

  if (SetConsoleCtrlHandler(CtrlHandler, 1) == 0) {
    printf("Error: Could not set control handler\n");
    return 1;
  }
  SetConsoleTitle(APP_NAME " " APP_VERSION);
  parse_cmdargs(argc, argv);
  printf("[%d:%d:%d]\nprogram start, press CTRL+C to end.\n",
      frameTime, keyWaitFrames, keyHoldFrames);
  while(!xusers_seek()) {
    Sleep(3000);
  }
  while(1){
    xusers_loop();
    Sleep(frameTime);
  }
  return 0;
}

