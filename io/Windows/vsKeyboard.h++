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
#include <windows.h>

#define VS_KB_MAX_BUTTONS    150

#define VS_KB_COMMAND_LENGTH 80

// Windows-specific keyboard message flags and masks
#define VS_KB_FLAG_AUTOREPEAT_BIT 0x40000000
#define VS_KB_FLAG_EXT_KEY_BIT    0x01000000
#define VS_KB_MASK_SCAN_CODE      0x00FF0000
#define VS_KB_MASK_REPEAT_COUNT   0x0000FFFF

// Used with the GetKeyState() function to see if a toggle key (CAPS LOCK,
// for example) is toggled on or off
#define VS_KB_FLAG_KEY_TOGGLED 0x0001

// Scan codes (used only when necessary)
#define VS_KB_SCAN_LSHIFT 0x2a
#define VS_KB_SCAN_RSHIFT 0x36

// Operational modes
enum VS_IO_DLL
{
    VS_KB_MODE_BUTTON,
    VS_KB_MODE_TERMINAL
};

// Key "states"
enum VS_IO_DLL
{
    VS_KB_STABLE,
    VS_KB_JUST_PRESSED,
    VS_KB_JUST_RELEASED,
    VS_KB_STILL_RELEASED
};

// Enum to index the non-printable keys into the vsInputButton array
// the printable keys will map directly to the corresponding ASCII codes 
enum VS_IO_DLL
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

    // Printable characters will map to ASCII codes directly
    // We'll leave 97-122 free so users can use either lower-case 
    // letters ('a'=97 - 'z'=122) or upper-case letters 
    // ('A'=65 - 'Z'=90) in getButton() calls to access the state 
    // of the letter keys.

    // Cursor keys
    VS_KEY_UP         = 128,
    VS_KEY_DOWN       = 129,
    VS_KEY_LEFT       = 130,
    VS_KEY_RIGHT      = 131,

    // Keypad keys
    VS_KEY_KP0        = 132,
    VS_KEY_KP1        = 133,
    VS_KEY_KP2        = 134,
    VS_KEY_KP3        = 135,
    VS_KEY_KP4        = 136,
    VS_KEY_KP5        = 137,
    VS_KEY_KP6        = 138,
    VS_KEY_KP7        = 139,
    VS_KEY_KP8        = 140,
    VS_KEY_KP9        = 141,
    VS_KEY_KPDECIMAL  = 142,
    VS_KEY_KPDIVIDE   = 143,
    VS_KEY_KPMULTIPLY = 144,
    VS_KEY_KPSUBTRACT = 145,
    VS_KEY_KPADD      = 146,
    VS_KEY_KPENTER    = 147,
    VS_KEY_NUMLOCK    = 148
};

class VS_IO_DLL vsKeyboard : public vsInputDevice
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

    // Keyboard-specific mapping functions
    int              mapToButton(unsigned virtKey, unsigned flags);
    int              mapToChar(unsigned virtKey);

    // Display prompt and current command
    void             redrawPrompt();

VS_INTERNAL:

    // Change the state of a key
    void             pressKey(unsigned virtKey, unsigned flags);
    void             releaseKey(unsigned virtKey, unsigned flags);

    void             update();

public:

                          vsKeyboard(int mode);
    virtual               ~vsKeyboard();
                     
    // Inherited class name method
    virtual const char    *getClassName();

    // General input device methods
    virtual int           getNumAxes();
    virtual int           getNumButtons();

    vsInputAxis           *getAxis(int index);
    vsInputButton         *getButton(int index);

    // Retrieve the current command string
    int                   isCommandReady();
    char                  *getCommand();

    // Set/get the operational mode
    void                  setMode(int newMode);
    int                   getMode();

    // Set/get the command key (the key that temporarily switches the
    // keyboard to terminal mode so that a command can be typed)
    void                  setCommandKey(int keyIndex);
    int                   getCommandKey();
};

#endif
