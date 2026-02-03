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
int gentlePushStick; /* init with Dead Zone! */

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

void thumbstick_register(const char lr, short x, short y) {
  if (IN_DEAD_ZONE(x, y, gConfigDeadZone)) {
    return;
  }
  int absX = ABS(x);
  int absY = ABS(y);
  /* -32768~32767 */
  LOG("%cS(X,Y) = (%d,%d)\n", lr, x, y);
  if (COND_CAN_MOVE_MOUSE) {
    int mX = absX > gentlePushStick ? absX/1000 : absX/2000;
    int mY = absY > gentlePushStick ? absY/1000 : absY/2000;
    LOG("mouse_xy(%d,%d) \n", mX*SIGN(x), mY*SIGN(y)*(-1));
    mice_move(mX*SIGN(x), mY*SIGN(y)*(-1));
    if (!gModeCfg) { /* walk through */
      return;
    }
  }
  if (COND_CAN_MOVE_WINDOW) {
    int mX = absX > gentlePushStick ? absX/1000 : absX/2000;
    int mY = absY > gentlePushStick ? absY/1000 : absY/2000;
    LOG("window_xy(%d,%d) \n", mX*SIGN(x), mY*SIGN(y)*(-1));
    window_move(mX*SIGN(x), mY*SIGN(y)*(-1));
    if (!gModeCfg) { /* walk through */
      return;
    }
  }
  thumbstick_move_register(lr,0,(y > 0 && absY > absX));
  thumbstick_move_register(lr,1,(y < 0 && absY > absX));
  thumbstick_move_register(lr,2,(x > 0 && absX > absY));
  thumbstick_move_register(lr,3,(x < 0 && absX > absY));
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
    KEY_X_REG(DPAD_UP);
    KEY_X_REG(DPAD_DOWN);
    KEY_X_REG(DPAD_RIGHT);
    KEY_X_REG(DPAD_LEFT);
    KEY_X_REG(LS);
    KEY_X_REG(RS);
    KEY_X_REG(LT);
    KEY_X_REG(RT);

    if (gConfigSwapABXY) {
      key_complex_register(KL_B, COND_A, gVarIdleFrame, &gpA);
      key_complex_register(KL_A, COND_B, gVarIdleFrame, &gpB);
      key_complex_register(KL_Y, COND_X, gVarIdleFrame, &gpX);
      key_complex_register(KL_X, COND_Y, gVarIdleFrame, &gpY);
    }
    else {
      key_complex_register(KL_A, COND_A, gVarIdleFrame, &gpA);
      key_complex_register(KL_B, COND_B, gVarIdleFrame, &gpB);
      key_complex_register(KL_X, COND_X, gVarIdleFrame, &gpX);
      key_complex_register(KL_Y, COND_Y, gVarIdleFrame, &gpY);
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
  if (cfg_load(fullpath)) {
    return;
  }
}

void cleanup() {
  cfg_unload();
  cleanup_window();
  printf("program end.\n");
}

void parse_cmdargs(int argc, char* argv[]) {
  int hasArgC = 0;
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
      frameTime = NORM_RANGE(a,25,255,100);
      keyWaitFrames = NORM_RANGE(b,1,255,10);
      keyHoldFrames = NORM_RANGE(c,1,255,10);
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
  }
  if (!hasArgC) {
    load_config_file("START");
  }
  gentlePushStick = (32767 - gConfigDeadZone)/2;
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

