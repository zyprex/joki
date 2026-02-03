# Create Time   : 2026.01.21
# Modified Time : 2026.02.02
# Compiler      : mingw64-x86_64-win32-seh-rev0
# GCC Version   : 8.1.0
# Make          : mingw32-make

SRCS:=joki.c cfg.c inputs.c morse.c wnd.c

debug:
	gcc -O0 -Wall -Werror -o joki $(SRCS) -lxinput -DDEBUG

joki:
	gcc -O2 -o joki $(SRCS) -lxinput


