#pragma once
#include <windef.h>
#include <winuser.h>
// #include <stdio.h>

void cleanup_window();
void window_hide_show();
void window_set_pos(HWND wndPos);
void window_resize(double aspectRatio, int cx);
void window_topmost();
void window_move(int x, int y);

