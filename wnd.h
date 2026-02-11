#pragma once
#include <windef.h>
#include <winuser.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <stdio.h>
#include <process.h>

#define ASPECT_RATIO_W2H(rw,rh, w) (((w)*1.0)/((rw)/(rh)))
#define ASPECT_RATIO_H2W(rw,rh, h) (((w)*1.0)*((rw)/(rh)))

void cleanup_window();
void window_hide_show();
void window_set_pos(HWND wndPos);
void window_resize_client_area(int w, int h);
void window_topmost();
void window_move(int x, int y);
void window_move_center();
void screen_off();
void runlnk(char* operation, short mod);
void runlnk_op_runas(void* m);
void runlnk_op_open(void* m);
void runlnk_async(char* operation, short mod);
