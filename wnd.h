#pragma once
#include <windef.h>
#include <winuser.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <stdio.h>
#include <process.h>

void cleanup_window();
void window_hide_show();
void window_set_pos(HWND wndPos);
void window_resize(double aspectRatio, int cx);
void window_topmost();
void window_move(int x, int y);
void screen_off();
void runlnk(char* operation, short mod);
void runlnk_op_runas(void* m);
void runlnk_op_open(void* m);
void runlnk_async(char* operation, short mod);
