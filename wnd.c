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

void window_resize(double aspectRatio, int cx) {
  // Get screen width and height
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int cy = (cx * 1.0) / aspectRatio;
  int x = (screenWidth - cx) / 2;
  int y = (screenHeight - cy) / 2;
  MoveWindow(GetForegroundWindow(), x, y, cx, cy, 0);

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

