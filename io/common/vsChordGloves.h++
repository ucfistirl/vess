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
//    VESS Module:  vsChordGloves.h++
//
//    Description:  Device to keep track of the state of a pair of VR
//                  chord gloves.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_CHORD_GLOVES_HPP
#define VS_CHORD_GLOVES_HPP

// Chord gloves have electrical contact pads (or other means) to detect when
// certain digits are touching (such as the thumb and the index finger).
// They are generally paired, allowing both hands to manipulate objects in a
// virtual environment.  Generally, chord gloves work by detecting when a
// finger comes into contact with the thumb or the palm.  Many systems (such
// as the Fakespace PINCH system) also allow contact between digits of each
// hand (i.e.: the left forefinger touching the right thumb).
//
// This class is written generically to allow support for most glove systems
// (for example, palm contact is supported even though the PINCH system
// has no palm contacts).  The data are maintained as an array of 
// vsInputButtons, each button representing a contact pair.  If there are more
// than two digits in a single contact, this is represented transitively
// as a series of pairs (left forefinger touching left thumb, left middle 
// finger touching left thumb, left forefinger touching left middle finger).

#include "vsGlobals.h++"
#include "vsIODevice.h++"

// Index constants for each digit (single glove)
enum 
{
    VS_CG_THUMB  =  0,  // Thumb
    VS_CG_FORE   =  1,  // Forefinger
    VS_CG_MIDDLE =  2,  // Middle finger
    VS_CG_RING   =  3,  // Ring finger
    VS_CG_PINKY  =  4,  // Pinky
    VS_CG_HAND   =  5   // Hand (palm)
};

// Index constants for each digit (paired gloves)
enum 
{
    VS_CG_LTHUMB  =  0,  // Left thumb
    VS_CG_LFORE   =  1,  // Left forefinger
    VS_CG_LMIDDLE =  2,  // Left middle finger
    VS_CG_LRING   =  3,  // Left ring finger
    VS_CG_LPINKY  =  4,  // Left pinky
    VS_CG_LHAND   =  5,  // Left hand (palm)
    VS_CG_RTHUMB  =  6,  // Right thumb
    VS_CG_RFORE   =  7,  // Right forefinger
    VS_CG_RMIDDLE =  8,  // Right middle finger
    VS_CG_RRING   =  9,  // Right ring finger
    VS_CG_RPINKY  = 10,  // Right pinky
    VS_CG_RHAND   = 11   // Right hand (palm)
};

#define VS_CG_MAX_DIGITS 12

class VESS_SYM vsChordGloves : public vsIODevice
{
protected:

    // Stores the current state of the gloves as half a 2-D matrix of
    // vsInputButtons
    vsInputButton    *contactMatrix[VS_CG_MAX_DIGITS][VS_CG_MAX_DIGITS];

VS_INTERNAL:

    // State-changing functions
    void   connect(int first, int second);
    void   disconnect(int first, int second);
    void   clearContacts();

public:

    // Constructor/Destructor
                             vsChordGloves();
    virtual                  ~vsChordGloves();

    // Inherited functions
    virtual const char       *getClassName();

    virtual int              getNumAxes();
    virtual int              getNumButtons();

    virtual vsInputAxis      *getAxis(int index);
    virtual vsInputButton    *getButton(int index);

    // Additional getButton() call for meaningful digit pair contacts
    vsInputButton            *getButton(int i, int j);

    // Enumerates the pairs of digits in contact in the given array.
    // Returns the number of contact pairs.
    int                      getContactPairs(int *pairs, int maxSize);
};

#endif
