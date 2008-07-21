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
//    VESS Module:  vsSequencer.h++
//
//    Description:  This object is meant to handle scheduling of
//                  vsUpdatables.  It will allow more control
//                  for a various amount of updatables, including
//                  insertion of latencies and order of updating.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SEQUENCER_HPP
#define VS_SEQUENCER_HPP

#include "vsUpdatable.h++"
#include "vsTimer.h++"
#include "vsGlobals.h++"

// Maximum length of a name of a vsUpdatable in the sequencer.
// This includes space for the \0 ending character (all names are
// required to end in \0).
#define VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH   80

struct VESS_SYM UpdatableEntry
{
    vsUpdatable     *updatable;
    double          time;
    char            name[VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH];
    UpdatableEntry  *prev;
    UpdatableEntry  *next;
};

class VESS_SYM vsSequencer : public vsUpdatable
{
private:

    unsigned long   updatableCount;
    UpdatableEntry  *updatableListHead;
    UpdatableEntry  *updatableListTail;
    vsTimer         *sequencerTimer;

    void            removeEntryFromList(UpdatableEntry *entry);

public:

                    vsSequencer(void);
    virtual         ~vsSequencer(void);

    const char      *getClassName();

    void            addUpdatable(vsUpdatable *updatable, char *name);
    void            addUpdatable(vsUpdatable *updatable, double time, 
                                 char *name);

    void            removeUpdatable(vsUpdatable *updatable);

    double          getUpdatableTime(vsUpdatable *updatable);
    void            setUpdatableTime(vsUpdatable *updatable, double time);

    void            setUpdatableName(vsUpdatable *updatable, char *name);
    char            *getUpdatableName(vsUpdatable *updatable);

    void            setUpdatablePosition(vsUpdatable *updatable,
                                         unsigned long newPosition);
    unsigned long   getUpdatablePosition(vsUpdatable *updatable);

    unsigned long   getUpdatableCount(void);
    vsUpdatable     *getUpdatable(unsigned long i);
    vsUpdatable     *getUpdatableByName(char *name);

    virtual void    update(void);
};

#endif
