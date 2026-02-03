#pragma once

#include <stdio.h>

typedef struct MCode_t {
  char code; /* binary display for morse code */
  char n; /* morse code's length */
} MCode;

void morse_clear();
void morse_send(char ditOrDah);
char morse_compose();
