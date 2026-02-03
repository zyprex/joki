#include "morse.h"

MCode morseCode;

/* International Morse Code Table */
const char morseTb[] = {
  '\0',8, 0b00000000,
  'a', 2, 0b01,
  'b', 4, 0b1000,
  'c', 4, 0b1010,
  'd', 3, 0b100,
  'e', 1, 0b0,
  'f', 4, 0b0010,
  'g', 3, 0b110,
  'h', 4, 0b0000,
  'i', 2, 0b00,
  'j', 4, 0b0111,
  'k', 3, 0b101,
  'l', 4, 0b0100,
  'm', 2, 0b11,
  'n', 2, 0b10,
  'o', 3, 0b111,
  'p', 4, 0b0110,
  'q', 4, 0b1101,
  'r', 3, 0b010,
  's', 3, 0b000,
  't', 1, 0b1,
  'u', 3, 0b001,
  'v', 4, 0b0001,
  'w', 3, 0b011,
  'x', 4, 0b1001,
  'y', 4, 0b1011,
  'z', 4, 0b1100,
  '0', 5, 0b11111,
  '1', 5, 0b01111,
  '2', 5, 0b00111,
  '3', 5, 0b00011,
  '4', 5, 0b00001,
  '5', 5, 0b00000,
  '6', 5, 0b10000,
  '7', 5, 0b11000,
  '8', 5, 0b11100,
  '9', 5, 0b11110,
};
const int morseTbLen = sizeof(morseTb) / sizeof(char);

void morse_clear() {
  morseCode.code = 0;
  morseCode.n = 0;
}

void morse_send(char ditOrDah) {
  morseCode.code <<= 1;
  morseCode.code |= (ditOrDah ? 1 : 0);
  morseCode.n++;
}

char morse_compose() {
  for (int i = 0; i < morseTbLen; i+=3) {
    if (morseCode.code == morseTb[i+2] &&
        morseCode.n == morseTb[i+1]) {
      return morseTb[i];
    }
  }
  return 0;
}

