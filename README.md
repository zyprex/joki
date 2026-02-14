# About Joki

![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/zyprex/joki/total)

Joki is a console program used to map gamepad keys to keyboard or mouse inputs.

# Requirements

- A controller with X-input mode support.
- Windows 8 (XInput1.4)

# Table of Contents
- [Quick Start](#quick-start)
- [Docs](#docs)
  - [Multiple Tap and Long Press](#multiple-tap-and-long-press)
  - [Command Arguments](#command-arguments)
  - [Configure File Specification](#configure-file-specification)
- [Usage](#usage)
  - [Bind Key](#bind-key)
  - [Switch Profiles](#switch-profiles)
  - [Morse Code Input](#morse-code-input)
  - [Tilt Mode](#tilt-mode)
  - [Trigger Keystrokes](#trigger-keystrokes)
- [Know Issues](#know-issues)
- [Reason](#reason)

# Quick Start

1. Create directory `configs`, and in its, a file `START.ini`.
```
--- ..
 |_ configs/
 |     |__ START.ini
 |_ joki.exe
```
2. Write below lines in the `START.ini`, save it.
```
LT <down_up> mouse_left
RT <down_up> mouse_right
LS_MOUSE_MOVE <option> on
```
3. Run the `joki.exe`, and plug-in your gamepad controller.
   After console print `user(0) connected`, minimize the 
   console window, try move the left thumbsticks,
   tap the left or right trigger.

# Docs

## Multiple Tap and Long Press

<details>
<summary>Multiple Tap and Long Press</summary>

If tune time setting is default value, then:
- Multiple tap occured in 1s.
- Hold 1s long, trigger long press.

Adjust this behavior from cmdarg `--tune-time`.

Assume "||" is 100ms.

```
|  6   |   10    |
||||||||||||||||||
|      |         |
down---down-----(tap 2 times)
 up      up

|    10   |
|||||||||||
|         |
|         |<- (long press)
down-----------up

```

</details>

## Command Arguments

Type `joki.exe -h` see Command args help.

<details>
<summary>Example:</summary>

Load `configs/mycfg.ini` as initial file.
And swap 'ABXY' buttons.
Set frame delay to 25ms, trigger multiple tap in 1s(25x40),
trigger long press after 2s(25x80).

```sh
joki --tune-time 25:40:80 --config mycfg --swap-abxy
```

</details>


## Configure File Specification

Explain one line config:

```
{CONFIG_NAME} {CONFIG_TYPE} {VIRTUAL_CODE} [MOD_CODE]
```

1. `CONFIG_NAME`
  - A gamepad key name
    - `A,B,X,Y,L,R,LT,RT,LS,RS,START,BACK,DPAD_UP,DPAD_DOWN,DPAD_RIGHT,DPAD_LEFT`
    - `LS_UP,LS_DOWN,LS_RIGHT,LS_LEFT`
    - `RS_UP,RS_DOWN,RS_RIGHT,RS_LEFT`
  - A config item name
    - `LS_MOUSE_MOVE`: on / off
    - `RS_MOUSE_MOVE`: on / off
    - `LS_WINDOW_MOVE`: on / off
    - `RS_WINDOW_MOVE`: on / off
    - `TITL_MODE`: left / right / off

2. `CONFIG_TYPE`
  - `<option>` (only for config item name)
  - `<down_up>` or `0`: simulate key down and up.
  - `<single_tap>` or `1`: trigger only at single tap.
  - `<double_tap>` or `2`: trigger only at double tap.
  - `<long_press>` or `-1`: trigger only at long hold the key.
  - `<repeat>`: repeat after long press a key, repeat long press.
  - `<toggle>`: switch key's down up state.
  - `<tap_thru>`: multiple tap fall through.
  - number N > 2 : trigger only when you multiple tap N times.

**WARN: some type are definitly confilct with other!**


3. `VIRTUAL_CODE`
  - one lowercase word. (ref in `cfg_ex_vk.def` file)
  - '#Num': #2, #1, etc.

4. `MOD_CODE`
  - 'A': alt key
  - 'W': win key
  - 'S': shift key
  - 'C': ctrl key
  - lowercase word: treat as `VIRTUAL_CODE`.
  - number: the parameter for `VIRTUAL_CODE`.

# Usage

## Bind Key

A pseudo-example with some comments to demostrate almost all type usage:
```ini
# Press 'A' key to emulate mouse click.
A <down_up> mouse_left
# Above line's equal form.
A 0 mouse_left
# ctrl+shift+alt+win+c
A 0 c CSAW
# Press two key down or up (right to left)
A 0 c ctrl
# Long press 'A'
A <long_press> down
# Long press enable repeat (implicate disabled)
A <repeat> on
# Toggle A key's down or up  (implicate disabled)
A <toggle> on
# Press A 5 times to trigger up arrow key.
A 5 up
# Press A to move mouse down side 10 px.
A 0 mouse_move_y 10
```

## Switch Profiles

Switch different config file by `cfg_mode` function.

Assume in `START.ini`, `A.ini`, `B.ini`, has line `START 0 cfg_mode`.

Then press 'START' to enter the cfg mode. On this mode,
Press 'A' to load `A.ini`, Press 'B' to load `B.ini`.
When you want load `B.ini`, press 'START' then 'A' switch to `A.ini`.
Load `START.ini` press 'START' then the 'START' again.

## Morse Code Input
There have some internal functions let you input alphabeta use morse code.

Example:
```ini
A 0 morse_send 0
B 0 morse_send 1
X 0 morse_clear
Y 0 morse_char
R 0 morse_char_repeat
R -1 morse_char_repeat
R <repeat> on
```

A website for morse code: https://www.omniglot.com/writing/morsecode.htm

## Run Shortcuts

Use `runlnk` command to run `.lnk` file in directory `lnks`.

Example:
```ini
# open the "lnks/1.lnk"
A 0 runlnk 1
# open the "lnks/1.lnk" as adminstrator
A 0 runlnk_admin 1
```

## Tilt Mode

If want a single hand mode, use option `TITL_MODE`.

```ini
# Use the controller by single left hand.
# The DPAD and two sticks rotate 90 degree.
TITL_MODE <option> left
# Use the controller by single right hand.
# The DPAD and two sticks rotate -90 degree.
TITL_MODE <option> right
# And any other value for TITL_MODE is useless.
```

## Trigger Keystrokes

If want trigger a sequence of keypresses. use `<tap_thru>`.

Example:

```ini
BACK 1 a
BACK 2 b
BACK 3 c
# press BACK 3 tap, output: cba
BACK <tap_thru> #3 -2
# press BACK 1 tap, output: abc
BACK <tap_thru> #1 2
```

# Know Issues

1. The program didn't hijack original gamepad key event.
2. Some windows settings, such as ClickLock, will cause
   unexpected results.
3. Use mouse select text in console let the program halted
   (if missing Adminstrator privilege).
4. Some app (e.g. Task Manager) require Adminstrator privilege
   to emulate key strokes, so run it as Adminstrator. (The run 
   entrance is in right click context menu)
5. Always disgusted honeycomb-like setup GUI, so no GUI setup
   in the future.

# Reason

I get primal ideas from [Gopher360](https://github.com/Tylemagne/gopher360).
Unfortunately his configure file is way too hard, not easy for day-to-day
use, so I made mine.

