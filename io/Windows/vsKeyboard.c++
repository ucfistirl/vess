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
//    VESS Module:  vsKeyboard.c++
//
//    Description:  Class to handle the keyboard state, and/or keep track
//                  of command input strings.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsKeyboard.h++"
#include <stdio.h>
#include <ctype.h>

// ------------------------------------------------------------------------
// Create a vsKeyboard in the given mode.
// ------------------------------------------------------------------------
vsKeyboard::vsKeyboard(int kbMode)
{
    int i;

    // Initialize variables
    numButtons = VS_KB_MAX_BUTTONS;
    commandKey = -1;

    // Make sure the keyboard format is valid (default to button mode)
    if (kbMode == VS_KB_MODE_TERMINAL)
        mode = VS_KB_MODE_TERMINAL;
    else
        mode = VS_KB_MODE_BUTTON;

    // Clear the command string
    sprintf(command, "");
    commandReady = VS_FALSE;

    // Create a  vsInputButton for each key except the lower-case letter
    // keys.  These are mapped to the same vsInputButton as the upper-case
    // letter keys below.
    for (i = 0; i < VS_KB_MAX_BUTTONS; i++)
    {
        if ((i < 'a') || (i > 'z'))
            button[i] = new vsInputButton();
    }
    
    // Map the lower-case letters to the buttons for the upper-case letters
    for (i = 'a'; i <= 'z'; i++)
    {
        button[i] = button[toupper(i)];
    }
}

// ------------------------------------------------------------------------
// Delete the buttons.
// ------------------------------------------------------------------------
vsKeyboard::~vsKeyboard()
{
    int i;

    // Delete all the buttons
    for (i = 0; i < VS_KB_MAX_BUTTONS; i++)
    {
        // Don't try to delete the lower-case letter buttons, these will
        // be deleted when the same upper-case letter's button is deleted
        if (((i < 'a') || (i > 'z')) && (button[i]))
        {
            delete button[i];
        }
    }
}

// ------------------------------------------------------------------------
// Map the given Windows virtual keystroke to the correct vsInputButton.
// NOTE:  In many cases, this will be a US-specific keyboard mapping.
// ------------------------------------------------------------------------
int vsKeyboard::mapToButton(unsigned virtKey, unsigned flags)
{
    unsigned scanCode;
    
    // Attempt to map the given Windows virtual key to the appropriate 
    // VS_KEY_* symbol defined in the header file.  
    // 
    // If the virtual key is not recognized, a -1 is returned, indicating 
    // an error.
    //
    // See include/PlatformSDK/WinUser.h to see all the possible virtual keys
    // that Windows supports.  See vsKeyboard.h++ to see the keys that
    // VESS supports.
    
    // Check A-Z and 0-9.  These values map directly from ASCII codes
    if (((virtKey >= 'A') && (virtKey <= 'Z')) || 
        ((virtKey >= '0') && (virtKey <= '9')))
    {
        return virtKey;
    }
    
    // Space bar is also mapped to ASCII in both systems
    if (virtKey == VK_SPACE)
        return virtKey;
    
    // CTRL keys.  The extended bit needs to be checked here to identify 
    // whether the left or right key on the keyboard is pressed
    if (virtKey == VK_CONTROL)
    {
        if (flags & VS_KB_FLAG_EXT_KEY_BIT)
            return VS_KEY_RCTRL;
        else
            return VS_KEY_LCTRL;
    }
    
    // ALT keys (Windows refers to these as the "MENU" keys).  The extended
    // bit needs to be checked to identify left vs. right ALT
    if (virtKey == VK_MENU)
    {
        if (flags & VS_KB_FLAG_EXT_KEY_BIT)
            return VS_KEY_RALT;
        else
            return VS_KEY_LALT;
    }
    
    // SHIFT keys.  Unfortunately, the extended key bit is not set when right
    // shift is pressed, so we must check the hardware scan code in this case.
    if (virtKey == VK_SHIFT)
    {
        scanCode = flags & VS_KB_MASK_SCAN_CODE;
        scanCode >>= 16;
        
        if (scanCode == VS_KB_SCAN_LSHIFT)
            return VS_KEY_LSHIFT;
        else if (scanCode == VS_KB_SCAN_RSHIFT)
            return VS_KEY_RSHIFT;
        else
        {
            // We can't identify the shift key
            printf("vsKeyboard::mapToButton:  Unable to determine which SHIFT "
                "key was pressed!\n");
        }
    }
    
    // Enter Key (can either be the standard ENTER or the numeric keypad enter,
    // depending on the extended key flag)
    if (virtKey == VK_RETURN)
    {
        if (flags & VS_KB_FLAG_EXT_KEY_BIT)
            return VS_KEY_KPENTER;
        else
            return VS_KEY_ENTER;
    }
    
    // Backspace, TAB, and Escape
    if (virtKey == VK_BACK)
        return VS_KEY_BACKSPACE;
    if (virtKey == VK_TAB)
        return VS_KEY_TAB;
    if (virtKey == VK_ESCAPE)
        return VS_KEY_ESC;
    
    // Pause, CAPS LOCK, NUM LOCK, SCROLL LOCK, PRINT SCREEN
    if (virtKey == VK_PAUSE)
        return VS_KEY_PAUSE;
    if (virtKey == VK_CAPITAL)
        return VS_KEY_CAPSLOCK;
    if (virtKey == VK_NUMLOCK)
        return VS_KEY_NUMLOCK;
    if (virtKey == VK_SCROLL)
        return VS_KEY_SCRLOCK;
    if (virtKey == VK_SNAPSHOT)
        return VS_KEY_PRTSC;
    
    // HOME, END, etc..  These keys use the extended key bit to determine
    // whether the keystroke is on the numeric keypad or elsewhere
    if (((virtKey >= VK_PRIOR) && (virtKey <= VK_DOWN)) ||
        ((virtKey >= VK_INSERT) && (virtKey <= VK_DELETE)))
    {
        if (flags & VS_KB_FLAG_EXT_KEY_BIT)
        {
            // Keystroke came from elsewhere on the keyboard (the extended
            // AT keyboard area)
            switch (virtKey)
            {
                case VK_INSERT:
                    return VS_KEY_INSERT;
                case VK_DELETE:
                    return VS_KEY_DELETE;
                case VK_HOME:
                    return VS_KEY_HOME;
                case VK_END:
                    return VS_KEY_END;
                case VK_PRIOR:
                    return VS_KEY_PGUP;
                case VK_NEXT:
                    return VS_KEY_PGDN;
                case VK_UP:
                    return VS_KEY_UP;
                case VK_DOWN:
                    return VS_KEY_DOWN;
                case VK_LEFT:
                    return VS_KEY_LEFT;
                case VK_RIGHT:
                    return VS_KEY_RIGHT;
            }
        }
        else
        {
            // Keystroke came from the keypad
            switch (virtKey)
            {
                case VK_INSERT:
                    return VS_KEY_KP0;
                case VK_DELETE:
                    return VS_KEY_KPDECIMAL;
                case VK_HOME:
                    return VS_KEY_KP7;
                case VK_END:
                    return VS_KEY_KP1;
                case VK_PRIOR:
                    return VS_KEY_KP9;
                case VK_NEXT:
                    return VS_KEY_KP3;
                case VK_UP:
                    return VS_KEY_KP8;
                case VK_DOWN:
                    return VS_KEY_KP2;
                case VK_LEFT:
                    return VS_KEY_KP4;
                case VK_RIGHT:
                    return VS_KEY_KP6;
            }
        }
    }
    
    // Other numeric keypad keys
    if ((virtKey >= VK_NUMPAD0) && (virtKey <= VK_DIVIDE))
    {
        switch (virtKey)
        {
            case VK_MULTIPLY:
                return VS_KEY_KPMULTIPLY;
            case VK_ADD:
                return VS_KEY_KPADD;
            case VK_SUBTRACT:
                return VS_KEY_KPSUBTRACT;
            case VK_DECIMAL:
                return VS_KEY_KPDECIMAL;
            case VK_DIVIDE:
                return VS_KEY_KPDIVIDE;
            default:
                // The rest are the regular keypad number keys.  It so happens
                // that the virtual key number is 36 less than the desired
                // button index: VK_NUMPAD0 = 0x60 = 96
                //               VS_KEY_KP0 = 0x84 = 132
                return virtKey + 36;
        }
    }
    
    // "Clear" key (actually 5 on the numeric keypad when NUMLOCK is off)
    if (virtKey == VK_CLEAR)
        return VS_KEY_KP5;
    
    // Function keys
    if ((virtKey >= VK_F1) && (virtKey <= VK_F12))
    {
        // The virtual key numbers for the function keys are 111 greater
        // than the corresponding button index
        // (VK_F1 = 0x70, VS_KEY_F1 = 1 = 0x1)
        return virtKey - 111;
    }
    
    // Other printable character keys.  Windows gives these keys very generic
    // names, probably in order to support a variety of keyboard layouts.
    // We'll map these keys to their AT-style key labels.
    if ((virtKey >= VK_OEM_1) && (virtKey <= VK_OEM_7))
    {
        switch (virtKey)
        {
            case VK_OEM_1:
                return ';';
            case VK_OEM_PLUS:
                return '=';
            case VK_OEM_COMMA:
                return ',';
            case VK_OEM_MINUS:
                return '-';
            case VK_OEM_PERIOD:
                return '.';
            case VK_OEM_2:
                return '/';
            case VK_OEM_3:
                return '`';
            case VK_OEM_4:
                return '[';
            case VK_OEM_5:
                return '\\';
            case VK_OEM_6:
                return ']';
            case VK_OEM_7:
                return '\'';
        }
    }
    
    // Unknown/unsupported key
    return -1;
}

// ------------------------------------------------------------------------
// Map the given Windows virtual keystroke to a printable character, if
// possible
// ------------------------------------------------------------------------
int vsKeyboard::mapToChar(unsigned virtKey)
{
    int shifted;
    SHORT capsState;
    int caps;
    
    // If any modifier keys aside from SHIFT are pressed, we don't have
    // a valid character
    if ((button[VS_KEY_LCTRL]->isPressed()) || 
        (button[VS_KEY_RCTRL]->isPressed()) ||
        (button[VS_KEY_LALT]->isPressed()) ||
        (button[VS_KEY_RALT]->isPressed()))
        return -1;
        
    // See if either shift key is pressed
    if ((button[VS_KEY_LSHIFT]->isPressed()) || 
        (button[VS_KEY_RSHIFT]->isPressed()))
        shifted = VS_TRUE;
    else
        shifted = VS_FALSE;
        
    // Check the caps lock key
    capsState = GetKeyState(VK_CAPITAL);
    if (capsState & VS_KB_FLAG_KEY_TOGGLED)
        caps = VS_TRUE;
    else
        caps = VS_FALSE;
        
    // Invert the caps state if shifted is true
    caps ^= shifted;
        
    // See if the key is a letter
    if ((virtKey >= 'A') && (virtKey <= 'Z'))
    {
        if (caps)
            return virtKey;
        else
            return tolower(virtKey);
    }
    
    // See if the key is a number
    if ((virtKey >= '0') && (virtKey <= '9'))
    {
        if (shifted)
        {
            switch (virtKey)
            {
                case '0':
                    return ')';
                case '1':
                    return '!';
                case '2':
                    return '@';
                case '3':
                    return '#';
                case '4':
                    return '$';
                case '5':
                    return '%';
                case '6':
                    return '^';
                case '7':
                    return '&';
                case '8':
                    return '*';
                case '9':
                    return '(';
            }
        }
        else
            return virtKey;
    }
    
    // Deal with numeric keypad
    if ((virtKey >= VK_NUMPAD0) && (virtKey <= VK_DIVIDE))
    {
        switch (virtKey)
        {
            case VK_NUMPAD0:
                return '0';
            case VK_NUMPAD1:
                return '1';
            case VK_NUMPAD2:
                return '2';
            case VK_NUMPAD3:
                return '3';
            case VK_NUMPAD4:
                return '4';
            case VK_NUMPAD5:
                return '5';
            case VK_NUMPAD6:
                return '6';
            case VK_NUMPAD7:
                return '7';
            case VK_NUMPAD8:
                return '8';
            case VK_NUMPAD9:
                return '9';
            case VK_MULTIPLY:
                return '*';
            case VK_ADD:
                return '+';
            case VK_SUBTRACT:
                return '-';
            case VK_DECIMAL:
                return '.';
            case VK_DIVIDE:
                return '/';
        }
    }
    
    // Deal with the "OEM" keys
    if ((virtKey >= VK_OEM_1) && (virtKey <= VK_OEM_7))
    {
        if (shifted)
        {
            switch (virtKey)
            {
                case VK_OEM_1:
                    return ':';
                case VK_OEM_PLUS:
                    return '+';
                case VK_OEM_COMMA:
                    return '<';
                case VK_OEM_MINUS:
                    return '_';
                case VK_OEM_2:
                    return '?';
                case VK_OEM_3:
                    return '~';
                case VK_OEM_4:
                    return '{';
                case VK_OEM_5:
                    return '|';
                case VK_OEM_6:
                    return '}';
                case VK_OEM_7:
                    return '"';
            }
        }
        else
        {
            switch (virtKey)
            {
                case VK_OEM_1:
                    return ';';
                case VK_OEM_PLUS:
                    return '=';
                case VK_OEM_COMMA:
                    return ',';
                case VK_OEM_MINUS:
                    return '-';
                case VK_OEM_2:
                    return '/';
                case VK_OEM_3:
                    return '`';
                case VK_OEM_4:
                    return '[';
                case VK_OEM_5:
                    return '\\';
                case VK_OEM_6:
                    return ']';
                case VK_OEM_7:
                    return '\'';
            }
        }
    }
    
    // Non-printable or unsupported key
    return -1;
}

// ------------------------------------------------------------------------
// Present a prompt to enter string commands and echo the current command
// being typed.
// ------------------------------------------------------------------------
void vsKeyboard::redrawPrompt()
{
    printf("\rCOMMAND:  %s", command);
    fflush(stdout);
}

// ------------------------------------------------------------------------
// Set the given key's state to pressed.
// ------------------------------------------------------------------------
void vsKeyboard::pressKey(unsigned virtKey, unsigned flags)
{
    int index;
    int result;
    char character;
    int cmdLength;
    
    // Make sure this keystroke is not an auto-repeat key
    if (flags & 0x40000000)
        return;

    // See if the keystroke is a printable character
    result = mapToChar(virtKey);
    if ((result >= 0) && (result <= 127))
    {
        character = (char)result;
    }

    // Map the keystroke to an index in the vsInputButton array
    index = mapToButton(virtKey, flags);

    // Process the key
    if (index >= 0)
    {
        // Press the corresponding input button
        (button[index])->setPressed();

        // Set the state to "just pressed"
        keyState[index] = VS_KB_JUST_PRESSED;

        // Check the keyboard mode (terminal or button).  
        // In button mode, the keyboard simply keeps track of the state of
        // each keyboard "button".  In terminal mode, the keyboard also 
        // accumulates a command string that terminated and stored for the 
        // application when the ENTER key is pressed.
        if (mode == VS_KB_MODE_TERMINAL)
        {
            // Get the length of the current command
            cmdLength = (int)strlen(command);

            // Check the key to see if this is a keystroke we should add
            // to the command string
            if (((cmdLength + 1) < VS_KB_COMMAND_LENGTH) &&
                (((index >= ' ') && (index <= '~')) ||
                 ((index >= VS_KEY_KP0) && (index <= VS_KEY_KPADD))))
            {
                // Add the latest key to the command string
                sprintf(command, "%s%c", command, character);

                // Redraw the prompt with the current command appended
                redrawPrompt();
            }
            else if (index == VS_KEY_BACKSPACE)
            {
                // Remove the last character from the command string
                if (cmdLength > 0)
                    command[cmdLength - 1] = '\0';

                // Redraw the prompt with the current command appended
                redrawPrompt();
            }
            else if ((index == VS_KEY_ENTER) || (index == VS_KEY_KPENTER))
            {
                // Signal a command is ready to execute
                printf("\n");
                if (strlen(command) > 0)
                {
                    // Copy the current command to lastCommand, from which it
                    // can be retrieved by the application
                    strcpy(lastCommand, command);

                    // Clear the current command
                    sprintf(command, "");

                    // Set the command ready flag
                    commandReady = VS_TRUE;
                }
            }
        }
        else
        {
            // Check if this keystroke is the designated command key
            if (index == commandKey)
            {
                // Switch to terminal mode to obtain the command
                modeToggled = VS_TRUE;
                mode = VS_KB_MODE_TERMINAL;

                // Clear the command string
                sprintf(command, "");

                // Draw the command prompt
                redrawPrompt();
            }
        }
    }
}

// ------------------------------------------------------------------------
// Set the given key's state to released.
// ------------------------------------------------------------------------
void vsKeyboard::releaseKey(unsigned virtKey, unsigned flags)
{
    int index;

    // Map the Keysym to an index in the vsInputButton array
    index = mapToButton(virtKey, flags);

    // Make sure the key is valid
    if (index >= 0)
    {
        // Set the key to "just released" if it is currently pressed
        if ((button[index])->isPressed())
        {
            keyState[index] = VS_KB_JUST_RELEASED;
        }
    }
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsKeyboard::getClassName()
{
    return "vsKeyboard";
}

// ------------------------------------------------------------------------
// Return the number of input axes (zero since the keyboard has no axes).
// ------------------------------------------------------------------------
int vsKeyboard::getNumAxes()
{
    return 0;
}

// ------------------------------------------------------------------------
// Return the number of input buttons.
// ------------------------------------------------------------------------
int vsKeyboard::getNumButtons()
{
    // Return the number of buttons (the number of keys on the keyboard)
    return numButtons;
}

// ------------------------------------------------------------------------
// Return NULL, since the keyboard has no axes.
// ------------------------------------------------------------------------
vsInputAxis *vsKeyboard::getAxis(int index)
{
    return NULL;
}

// ------------------------------------------------------------------------
// Return the requested input button.
// ------------------------------------------------------------------------
vsInputButton *vsKeyboard::getButton(int index)
{
    // Check to see if the specified button index is valid
    if ((index >= 0) && (index < numButtons))
    {
        // Return the corresponding button
        return button[index];
    }
    else
    {
        // Invalid index specified
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Update function (called by the window system)
// ------------------------------------------------------------------------
void vsKeyboard::update()
{
    int i;

    // Make sure a key press is acknowledged for at least one frame
    // This helps account for slow frame rates

    // For each key...
    for (i = 0; i < numButtons; i++)
    {
        // If this key is currently pressed...
        if (button[i]->isPressed())
        {
            // Process the key based on its current state
            if (keyState[i] == VS_KB_STILL_RELEASED)
            {
                // The key has been released for one complete frame,
                // so we can safely release the button now.
                keyState[i] = VS_KB_STABLE;
                (button[i])->setReleased();
            }
            else if (keyState[i] == VS_KB_JUST_RELEASED)
            {
                // The key was just released, so set its state to
                // "still released."  We'll actually release it next
                // frame.
                keyState[i] = VS_KB_STILL_RELEASED;
            }
            else if (keyState[i] == VS_KB_JUST_PRESSED)
            {
                // We're not so worried about presses, just set
                // it to stable immediately.
                keyState[i] = VS_KB_STABLE;
            }
        }
    }
}

// ------------------------------------------------------------------------
// Return the state of the command string.
// ------------------------------------------------------------------------
int vsKeyboard::isCommandReady()
{
    return commandReady;
}

// ------------------------------------------------------------------------
// Return the accumulated command string (if any).
// ------------------------------------------------------------------------
char *vsKeyboard::getCommand()
{
    commandReady = VS_FALSE;

    // If we entered terminal mode by the command key, switch 
    // back to button mode now
    if (modeToggled)
    {
        modeToggled = VS_FALSE;
        mode = VS_KB_MODE_BUTTON;
    }

    return lastCommand;
}

// ------------------------------------------------------------------------
// Change the keyboard operational mode to newMode.  See the pressKey()
// method above for a brief description of keyboard modes.  See the VESS
// User's Guide for a more thorough description.
// ------------------------------------------------------------------------
void vsKeyboard::setMode(int newMode)
{
    mode = newMode;
}

// ------------------------------------------------------------------------
// Return the current keyboard operational mode.
// ------------------------------------------------------------------------
int vsKeyboard::getMode()
{
    return mode;
}

// ------------------------------------------------------------------------
// Change the key that temporarily switches the keyboard to TERMINAL mode
// so that a command can be typed.
// ------------------------------------------------------------------------
void vsKeyboard::setCommandKey(int keyIndex)
{
    // Check the key index to see if it is valid
    if ((keyIndex >= 0) && (keyIndex < numButtons))
    {
        // If the key is a lower-case letter, change it to upper-case
        if ((keyIndex >= 'a') && (keyIndex <= 'z'))
            keyIndex -= 0x20;

        // Set the command key to the specified key
        commandKey = keyIndex;
    }
}

// ------------------------------------------------------------------------
// Return the index of the command key.
// ------------------------------------------------------------------------
int vsKeyboard::getCommandKey()
{
    return commandKey;
}
