#include "cfg.h"
const char* labelName[] = {
  "",
  "A", "B", "X", "Y",
  "L", "R", "LT", "RT",
  "DPAD_UP", "DPAD_DOWN", "DPAD_RIGHT", "DPAD_LEFT",
  "LS", "RS",
  "LS_UP", "LS_DOWN", "LS_RIGHT", "LS_LEFT",
  "RS_UP", "RS_DOWN", "RS_RIGHT", "RS_LEFT",
  "START", "BACK",
  "LS_MOUSE_MOVE",
  "RS_MOUSE_MOVE",
  "LS_WINDOW_MOVE",
  "RS_WINDOW_MOVE",
};
/* Keys that doesn't exist on most keyboards are
 * not in this list, if has any specific purpose, 
 * see more on:
 * https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
 */
/* reserved code:
 * 07
 * 0A 0B
 * 5E
 * 88 8F
 * B8 B9
 * C1 C2
 * E0
 * FC
 * */
VCode vcodes[] = {
#include "cfg_ex_vk.def"
};

const int vcodesLen = sizeof(vcodes) / sizeof(VCode);
/* Macro Expand Example:
 * LB_DEF(A) => { "A", KL_A },*/
#define LB_DEF(s) { #s, KL_##s },
VCode kLabels[] = {
  LB_DEF(A)  LB_DEF(B)  LB_DEF(X)  LB_DEF(Y)
  LB_DEF(L)  LB_DEF(R)  LB_DEF(LT) LB_DEF(RT)
  LB_DEF(DPAD_UP)    LB_DEF(DPAD_DOWN)
  LB_DEF(DPAD_RIGHT) LB_DEF(DPAD_LEFT)
  LB_DEF(LS)    LB_DEF(RS)
  LB_DEF(LS_UP)      LB_DEF(LS_DOWN)
  LB_DEF(LS_RIGHT)   LB_DEF(LS_LEFT)
  LB_DEF(RS_UP)      LB_DEF(RS_DOWN)
  LB_DEF(RS_RIGHT)   LB_DEF(RS_LEFT)
  LB_DEF(START) LB_DEF(BACK)
  LB_DEF(LS_MOUSE_MOVE)
  LB_DEF(RS_MOUSE_MOVE)
  LB_DEF(LS_WINDOW_MOVE)
  LB_DEF(RS_WINDOW_MOVE)
};
const int klabelsLen = sizeof(kLabels) / sizeof(VCode);

Cfg* configs;
int configsLen;

short cfg_get_mod(int idx) {
  return idx == -1 ? 0 : configs[idx].mod;
}

short cfg_get_vk(int idx) {
  return idx == -1 ? 0 : configs[idx].vk;
}

int cfg_search(short label, short type) {
  for (int i = 0; i < configsLen; ++i) {
    if (configs[i].label == label &&
        configs[i].type == type) {
       return i;
    }
  }
  return -1;
}

short s2label(char* s) {
 for (int i = 0; i < klabelsLen; ++i) {
  if (!strcmp(s, kLabels[i].name)) {
    return kLabels[i].code;
  }
 }
 return 0;
}

short s2type(char* s) {
  if (!strcmp("<double_tap>", s)) return KT_DOUBLE_TAP;
  if (!strcmp("<single_tap>", s)) return KT_SINGLE_TAP;
  if (!strcmp("<down_up>", s))    return KT_DOWN_UP;
  if (!strcmp("<long_press>", s)) return KT_LONG_PRESS;
  if (!strcmp("<repeat>", s))     return KT_REPEAT;
  if (!strcmp("<toggle>", s))     return KT_TOGGLE;
  if (!strcmp("<option>", s))     return KT_OPTION;
  return atoi(s);
}

short s2vk(char* s) {
 for (int i = 0; i < vcodesLen; ++i) {
  if (!strcmp(s, vcodes[i].name)) {
    return vcodes[i].code;
  }
 }
 return 0;
}

short s2mod(char* s, short vk) {
 short ret = 0;
 if (vk >= 0x00FF) {
   return atoi(s);
 }
 else {
   if (strchr(s, 'A')) ret |= MOD_A;
   if (strchr(s, 'C')) ret |= MOD_C;
   if (strchr(s, 'S')) ret |= MOD_S;
   if (strchr(s, 'W')) ret |= MOD_W;
   if (ret != 0) {
     return ret;
   }
   ret = s2vk(s);
   if (ret != 0) {
     ret += 0x8000;
   }
 }
 return ret;
}

void cfg_parse(char* line, int lnum) {
  char slabel[64],stype[64],svk[64],smod[64];
  int n = sscanf(line, "%s %s %s %s", slabel, stype, svk, smod);
  if (n > 0) configs[lnum].label = s2label(slabel);
  if (n > 1) configs[lnum].type  = s2type(stype);
  if (n > 2) configs[lnum].vk    = s2vk(svk);
  if (n > 3) configs[lnum].mod   = s2mod(smod, configs[lnum].vk);
  printf("cfg_parse:%15s %2d %04X %04X(%d)\n",
      labelName[configs[lnum].label],
      configs[lnum].type,
      configs[lnum].vk,
      configs[lnum].mod, configs[lnum].mod);
}

void cfg_unload() {
  if(configs)
    free(configs);
}

int cfg_load(char* fname) {
  char line[256];
  int lcnt = 0, lnum = 0;
  FILE* fp = fopen(fname, "r");
  if (!fp) {
    printf("Error: Can't open file '%s'!\n", fname);
    return 1;
  }
  while (fgets(line, sizeof(line), fp)) {
    if (line[0] != '#' && !isspace(line[0])) {
      lcnt++;
    }
  }
  fseek(fp,0,SEEK_SET);
  if (configs) {
   configs = realloc(configs, lcnt*sizeof(Cfg));
   memset(configs, 0, lcnt*sizeof(Cfg));
  }
  else {
   configs = calloc(lcnt, sizeof(Cfg));
  }
  configsLen = lcnt;
  while (fgets(line, sizeof(line), fp)) {
    if (line[0] != '#' && !isspace(line[0])) {
      cfg_parse(line, lnum++);
    }
  }
  printf("config read '%s' successful!\n", fname);
  fclose(fp);
  return 0;
}
