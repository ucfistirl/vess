//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsKeyboard.h++
//
//    Description:  Class to handle the keyboard state, and/or keep track
//                  of command input strings.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_KEYBOARD_HPP
#define VS_KEYBOARD_HPP

// This implementation supports the keys found on standard 101-key PC
// keyboards (or equivalent keyboards on other systems) running on X
// Windows systems.

#include "vsInputDevice.h++"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>

#define VS_KB_MAX_BUTTONS    128

#define VS_KB_COMMAND_LENGTH 80

// Operational modes
enum 
{
    VS_KB_MODE_BUTTON,
    VS_KB_MODE_TERMINAL
};

// Key "states"
enum
{
    VS_KB_STABLE,
    VS_KB_JUST_PRESSED,
    VS_KB_JUST_RELEASED,
    VS_KB_STILL_RELEASED
};

// Enum to index the non-printable keys into the vsInputButton array
// the printable keys will map directly to the corresponding X keysyms 
enum
{
    VS_KEY_ESC        = 0,
    VS_KEY_F1         = 1,
    VS_KEY_F2         = 2,
    VS_KEY_F3         = 3,
    VS_KEY_F4         = 4,
    VS_KEY_F5         = 5,
    VS_KEY_F6         = 6,
    VS_KEY_F7         = 7,
    VS_KEY_F8         = 8,
    VS_KEY_F9         = 9,
    VS_KEY_F10        = 10,
    VS_KEY_F11        = 11,
    VS_KEY_F12        = 12,
    VS_KEY_BACKSPACE  = 13,
    VS_KEY_TAB        = 14,
    VS_KEY_CAPSLOCK   = 15,
    VS_KEY_RETURN     = 16,
    VS_KEY_ENTER      = 16,
    VS_KEY_LSHIFT     = 17,
    VS_KEY_RSHIFT     = 18,
    VS_KEY_LCTRL      = 19,
    VS_KEY_RCTRL      = 20,
    VS_KEY_LALT       = 21,
    VS_KEY_RALT       = 22,
    VS_KEY_PRTSC      = 23,
    VS_KEY_SCRLOCK    = 24,
    VS_KEY_PAUSE      = 25,
    VS_KEY_INSERT     = 26,
    VS_KEY_DELETE     = 27,
    VS_KEY_HOME       = 28,
    VS_KEY_END        = 29,
    VS_KEY_PGUP       = 30,
    VS_KEY_PGDN       = 31,

    // Printable characters will map to KeySyms (ASCII code) directly

    // Cursor keys
    VS_KEY_UP         = 97,
    VS_KEY_DOWN       = 98,
    VS_KEY_LEFT       = 99,
    VS_KEY_RIGHT      = 100,

    // Keypad keys
    VS_KEY_KP0        = 101,
    VS_KEY_KP1        = 102,
    VS_KEY_KP2        = 103,
    VS_KEY_KP3        = 104,
    VS_KEY_KP4        = 105,
    VS_KEY_KP5        = 106,
    VS_KEY_KP6        = 107,
    VS_KEY_KP7        = 108,
    VS_KEY_KP8        = 109,
    VS_KEY_KP9        = 110,
    VS_KEY_KPDECIMAL  = 111,
    VS_KEY_KPDIVIDE   = 112,
    VS_KEY_KPMULTIPLY = 113,
    VS_KEY_KPSUBTRACT = 114,
    VS_KEY_KPADD      = 115,
    VS_KEY_KPENTER    = 116,
    VS_KEY_NUMLOCK    = 117
};

class vsKeyboard : public vsInputDevice
{
protected:

    // Keyboard "buttons"
    int              numButtons;
    vsInputButton    *button[VS_KB_MAX_BUTTONS];
    int              keyState[VS_KB_MAX_BUTTONS];

    // The current and last command strings
    char             command[VS_KB_COMMAND_LENGTH];
    char             lastCommand[VS_KB_COMMAND_LENGTH];

    // Flag to indicate when a command is ready to be retrieved
    int              commandReady;

    // The input mode
    int              mode;

    // The index of the "command" key (see below)
    int              commandKey;

    // Flag to indicate whether the mode has been toggled by the 
    // "command" key
    int              modeToggled;

    // Keyboard-specific mapping function
    int              mapToButton(KeySym keySym);

    // Display prompt and current command
    void             redrawPrompt();

VS_INTERNAL:

    // Change the state of a key
    void             pressKey(KeySym keySym, char *string);
    void             releaseKey(KeySym keySym);

    void             update();

public:

                     vsKeyboard(int mode);
                     ~vsKeyboard();

    // General input device methods
    virtual int      getNumAxes();
    virtual int      getNumButtons();

    vsInputAxis      *getAxis(int index);
    vsInputButton    *getButton(int index);

    // Retrieve the current command string
    int              isCommandReady();
    char             *getCommand();

    // Set/get the operational mode
    void             setMode(int newMode);
    int              getMode();

    // Set/get the command key (the key that temporarily switches the
    // keyboard to terminal mode so that a command can be typed)
    void             setCommandKey(int keyIndex);
    int              getCommandKey();
};

#endif
