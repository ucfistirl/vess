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

// ------------------------------------------------------------------------
// Create a vsKeyboard in the given mode.
// ------------------------------------------------------------------------
vsKeyboard::vsKeyboard(int kbMode)
{
    int i;

    // Initialize variables
    numButtons = VS_KB_MAX_BUTTONS;
    commandKey = -1;

    if (kbMode == VS_KB_MODE_TERMINAL)
        mode = VS_KB_MODE_TERMINAL;
    else
        mode = VS_KB_MODE_BUTTON;

    sprintf(command, "");

    // Create a  vsInputButton for each key
    for (i = 0; i < VS_KB_MAX_BUTTONS; i++)
        button[i] = new vsInputButton();
}

// ------------------------------------------------------------------------
// Delete the buttons.
// ------------------------------------------------------------------------
vsKeyboard::~vsKeyboard()
{
    int i;

    for (i = 0; i < VS_KB_MAX_BUTTONS; i++)
    {
        if (button[i])
            delete button[i];
    }
}

// ------------------------------------------------------------------------
// Map the given X KeySym to the correct vsInputButton.
// ------------------------------------------------------------------------
int vsKeyboard::mapToButton(KeySym keySym)
{
    if (((keySym >= XK_Shift_L) && (keySym <= XK_Caps_Lock)) ||
        ((keySym >= XK_Alt_L) && (keySym <= XK_Alt_R)))
    {
        switch (keySym)
        {
            case XK_Shift_L:
                return VS_KEY_LSHIFT;
            case XK_Shift_R:
                return VS_KEY_RSHIFT;
            case XK_Control_L:
                return VS_KEY_LCTRL;
            case XK_Control_R:
                return VS_KEY_RCTRL;
            case XK_Caps_Lock:
                return VS_KEY_CAPSLOCK;
            case XK_Alt_L:
                return VS_KEY_LALT;
            case XK_Alt_R:
                return VS_KEY_RALT;
        }
    }
    else if ((keySym >= XK_F1) && (keySym <= XK_F12))
    {
        return (keySym - XK_F1 + 1);
    }
    else if ((keySym >= XK_Home) && (keySym <= XK_End))
    {
        switch (keySym)
        {
            case XK_Home:
                return VS_KEY_HOME;
            case XK_End:
                return VS_KEY_END;
            case XK_Up:
                return VS_KEY_UP;
            case XK_Down:
                return VS_KEY_DOWN;
            case XK_Left:
                return VS_KEY_LEFT;
            case XK_Right:
                return VS_KEY_RIGHT;
            case XK_Page_Up:
                return VS_KEY_PGUP;
            case XK_Page_Down:
                return VS_KEY_PGDN;
        }
    }
    else if ((keySym >= XK_BackSpace) && (keySym <= XK_Escape))
    {
        switch (keySym)
        {
            case XK_BackSpace:
                return VS_KEY_BACKSPACE;
            case XK_Tab:
                return VS_KEY_TAB;
            case XK_Return:
                return VS_KEY_RETURN;
            case XK_Pause:
            case XK_Break:
                return VS_KEY_PAUSE;
            case XK_Scroll_Lock:
                return VS_KEY_SCRLOCK;
            case XK_Escape:
                return VS_KEY_ESC;
        }
    }
    else if (keySym == XK_Insert)
    {
        return VS_KEY_INSERT;
    }
    else if (keySym == XK_Delete)
    {
        return VS_KEY_DELETE;
    }
    else if (keySym == XK_Num_Lock)
    {
        return VS_KEY_NUMLOCK;
    }
    else if ((keySym == XK_Print) || (keySym == XK_Sys_Req))
    {
        return VS_KEY_PRTSC;
    }
    else if ((keySym >= XK_0) && (keySym <= XK_9))
    {
        return keySym;
    }
    else if ((keySym >= XK_A) && (keySym <= XK_Z))
    {
        return keySym;
    }
    else if ((keySym >= XK_a) && (keySym <= XK_z))
    {
        // Shift the lower-case KeySym to upper-case
        return (keySym - 0x20);
    }
    else if (((keySym >= XK_space) && (keySym <= XK_slash)) ||
        ((keySym >= XK_colon) && (keySym <= XK_at)) ||
        ((keySym >= XK_bracketleft) && (keySym <= XK_quoteleft)) ||
        ((keySym >= XK_braceleft) && (keySym <= XK_asciitilde)))
    {
        switch (keySym)
        {
            case XK_asciitilde:
                return XK_grave;
            case XK_exclam:
                return XK_1;
            case XK_at:
                return XK_2;
            case XK_numbersign:
                return XK_3;
            case XK_dollar:
                return XK_4;
            case XK_percent:
                return XK_5;
            case XK_asciicircum:
                return XK_6;
            case XK_ampersand:
                return XK_7;
            case XK_asterisk:
                return XK_8;
            case XK_parenleft:
                return XK_9;
            case XK_parenright:
                return XK_0;
            case XK_underscore:
                return XK_minus;
            case XK_plus:
                return XK_equal;
            case XK_braceleft:
                return XK_bracketleft;
            case XK_braceright:
                return XK_bracketright;
            case XK_bar:
                return XK_backslash;
            case XK_colon:
                return XK_semicolon;
            case XK_quotedbl:
                return XK_quoteleft;
            case XK_less:
                return XK_comma;
            case XK_greater:
                return XK_period;
            case XK_question:
                return XK_slash;
            default:
                return keySym;
        }
    }
    else if ((keySym >= XK_KP_Enter) && (keySym <= XK_KP_Divide))
    {
        switch (keySym)
        {
            case XK_KP_Insert:
                return VS_KEY_KP0;
            case XK_KP_Delete:
            case XK_KP_Decimal:
                return VS_KEY_KPDECIMAL;
            case XK_KP_End:
                return VS_KEY_KP1;
            case XK_KP_Down:
                return VS_KEY_KP2;
            case XK_KP_Page_Down:
                return VS_KEY_KP3;
            case XK_KP_Left:
                return VS_KEY_KP4;
            case XK_KP_Begin:
                return VS_KEY_KP5;
            case XK_KP_Right:
                return VS_KEY_KP6;
            case XK_KP_Home:
                return VS_KEY_KP7;
            case XK_KP_Up:
                return VS_KEY_KP8;
            case XK_KP_Page_Up:
                return VS_KEY_KP9;
            case XK_KP_Divide:
                return VS_KEY_KPDIVIDE;
            case XK_KP_Multiply:
                return VS_KEY_KPMULTIPLY;
            case XK_KP_Subtract:
                return VS_KEY_KPSUBTRACT;
            case XK_KP_Add:
                return VS_KEY_KPADD;
            case XK_KP_Enter:
                return VS_KEY_KPENTER;
        }
    }
    else if ((keySym >= XK_KP_0) && (keySym <= XK_KP_9))
    {
        return (VS_KEY_KP0 + (keySym - XK_KP_0));
    }

    // Invalid key
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
void vsKeyboard::pressKey(KeySym keySym, char *string)
{
    int  index;
    char buffer[50];
    int  cmdLength;

    // Copy the string representation for use in terminal commands
    strcpy(buffer, string);

    // Map the Keysym to an index in the vsInputButton array
    index = mapToButton(keySym);

    // Process the key
    if (index >= 0)
    {
        (button[index])->setPressed();
        keyState[index] = VS_KB_JUST_PRESSED;

        if (mode == VS_KB_MODE_TERMINAL)
        {
            cmdLength = strlen(command);

            if (((cmdLength + strlen(string)) < VS_KB_COMMAND_LENGTH) &&
                (((index >= ' ') && (index <= '~')) ||
                 ((index >= VS_KEY_KP0) && (index <= VS_KEY_KPADD))))
            {
                // Add the latest key to the command string
                strcat(command, string);

                redrawPrompt();
            }
            else if (index == VS_KEY_BACKSPACE)
            {
                // Remove the last character from the command string
                if (cmdLength > 0)
                    command[cmdLength - 1] = '\0';

                redrawPrompt();
            }
            else if ((index == VS_KEY_ENTER) || (index == VS_KEY_KPENTER))
            {
                // Signal a command is ready to execute
                printf("\n");
                if (strlen(command) > 0)
                {
                    strcpy(lastCommand, command);
                    sprintf(command, "");
                    commandReady = VS_TRUE;
                }
            }
        }
        else
        {
            if (index == commandKey)
            {
                // Switch to terminal mode to obtain a command
                modeToggled = VS_TRUE;
                mode = VS_KB_MODE_TERMINAL;

                sprintf(command, "");

                redrawPrompt();
            }
        }
    }
}

// ------------------------------------------------------------------------
// Set the given key's state to released.
// ------------------------------------------------------------------------
void vsKeyboard::releaseKey(KeySym keySym)
{
    int index;

    // Map the Keysym to an index in the vsInputButton array
    index = mapToButton(keySym);

    if (index)
    {
        if ((button[index])->isPressed())
        {
            keyState[index] = VS_KB_JUST_RELEASED;
        }
    }
}

// ------------------------------------------------------------------------
// Return the number of input axes (zero).
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
    if (index < numButtons)
    {
        return button[index];
    }
    else
    {
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
    for (i = 0; i < numButtons; i++)
    {
        if (button[i]->isPressed())
        {
            if (keyState[i] == VS_KB_STILL_RELEASED)
            {
                keyState[i] = VS_KB_STABLE;
                (button[i])->setReleased();
            }
            else if (keyState[i] == VS_KB_JUST_RELEASED)
            {
                keyState[i] = VS_KB_STILL_RELEASED;
            }
            else if (keyState[i] == VS_KB_JUST_PRESSED)
            {
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
// Change the keyboard operational mode to newMode.
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
    if (keyIndex < numButtons)
    {
        if ((keyIndex >= 'a') && (keyIndex <= 'z'))
            keyIndex -= 0x20;

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
