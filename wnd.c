#include "wnd.h"

HWND hideShowHwnd = NULL;

typedef struct FgWnd_t {
  HWND hwnd;
  int x;
  int y;
  int w;
  int h;
} FgWnd;


FgWnd* fgwnd_query() {
  static FgWnd fgwnd;
  fgwnd.hwnd = GetForegroundWindow();
  RECT rect;
  GetWindowRect(fgwnd.hwnd, &rect);
  fgwnd.x = rect.left;
  fgwnd.y = rect.top;
  fgwnd.w = rect.right - rect.left;
  fgwnd.h = rect.bottom - rect.top;
  return &fgwnd;
}


void cleanup_window() {
  if (hideShowHwnd != NULL) {
    ShowWindow(hideShowHwnd, SW_SHOW);
  }
}

void window_hide_show() {
  if (hideShowHwnd) {
    ShowWindow(hideShowHwnd, SW_SHOW);
    hideShowHwnd = NULL;
  }
  else {
    hideShowHwnd = GetForegroundWindow();
    ShowWindow(hideShowHwnd, SW_HIDE);
  }
}
/*
HWND_BOTTOM    -> Places the window at the bottom of the Z order.
HWND_NOTOPMOST -> Places the window behind all topmost windows.
HWND_TOP       -> Places the window at the top of the Z order.
HWND_TOPMOST   -> Places the window above all non-topmost windows.
*/
void window_set_pos(HWND wndPos) {
  FgWnd* fgwnd = fgwnd_query();
  SetWindowPos(fgwnd->hwnd, wndPos,
      fgwnd->x, fgwnd->y, fgwnd->w, fgwnd->h, SWP_SHOWWINDOW);
}

void window_resize_client_area(int w, int h) {
  FgWnd* fgwnd = fgwnd_query();
  RECT cr;
  GetClientRect(fgwnd->hwnd, &cr);
  int brimW = fgwnd->w - (cr.right - cr.left);
  int brimH = fgwnd->h - (cr.bottom - cr.top);
  w += brimW;
  h += brimH;
  MoveWindow(fgwnd->hwnd, fgwnd->x, fgwnd->y, w, h, 0);
}

void window_topmost() {
  LONG exStyle = GetWindowLong(GetForegroundWindow(), GWL_EXSTYLE);
  if (exStyle & WS_EX_TOPMOST) {
    window_set_pos(HWND_NOTOPMOST);
  }
  else {
    window_set_pos(HWND_TOPMOST);
  }
}

void window_move(int x, int y) {
  FgWnd* fgwnd = fgwnd_query();
  MoveWindow(fgwnd->hwnd,
      (fgwnd->x)+x, (fgwnd->y)+y, fgwnd->w, fgwnd->h, 1);
}

void window_move_center() {
  FgWnd* fgwnd = fgwnd_query();
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int x = (screenWidth - fgwnd->w) / 2;
  int y = (screenHeight - fgwnd->h) / 2;
  window_move(x, y);
}

void screen_off() {
  PostMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}


/* operation = "open" or "runas"*/
void runlnk(char* operation, short mod) {
  char filepath[60];
  sprintf(filepath, "lnks\\%d.lnk", mod);
  HINSTANCE hInst = NULL;
  SHELLEXECUTEINFO shxInfo;
  shxInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  shxInfo.fMask = SEE_MASK_DEFAULT;
  shxInfo.hwnd = NULL;
  shxInfo.lpVerb = operation;
  shxInfo.lpFile = filepath;
  shxInfo.lpParameters = NULL;
  shxInfo.lpDirectory = NULL;
  shxInfo.nShow = SW_SHOWNORMAL;
  shxInfo.hInstApp = hInst;
  if (!ShellExecuteEx(&shxInfo)) {
    printf("Run:'%s' failed.\n", filepath);
  }
}

void runlnk_op_runas(void* m) {
  runlnk("runas", *(short*)m);
}

void runlnk_op_open(void* m) {
  runlnk("open", *(short*)m);
}

void runlnk_async(char* operation, short mod) {
  if (!strcmp(operation, "open"))
    _beginthread(runlnk_op_open, 0, &mod);
  if (!strcmp(operation, "runas"))
    _beginthread(runlnk_op_runas, 0, &mod);
}
