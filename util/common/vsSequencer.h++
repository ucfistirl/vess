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
//    Description:  This object is ment to handle scheduleing of
//                  vsUpdatables.  It will allow more control
//                  for a various ammount of updatables, including
//                  insertion of latencies and order of updating.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SEQUENCER_HPP
#define VS_SEQUENCER_HPP

#include "vsUpdatable.h++"

struct UpdatableEntry
{
    vsUpdatable     *updatable;
    double          time;
    UpdatableEntry  *prev;
    UpdatableEntry  *next;
};

class vsSequencer : public vsUpdatable
{
private:

    unsigned long   updatableCount;
    UpdatableEntry  *updatableListHead;
    UpdatableEntry  *updatableListTail;

    void            removeEntryFromList(UpdatableEntry *entry);

public:

                    vsSequencer(void);
    virtual         ~vsSequencer(void);

    const char      *getClassName();

    void            addUpdatable(vsUpdatable *updatable);
    void            addUpdatable(vsUpdatable *updatable, double time);

    void            removeUpdatable(vsUpdatable *updatable);

    double          getUpdatableTime(vsUpdatable *updatable);
    void            setUpdatableTime(vsUpdatable *updatable, double time);

    void            setUpdatablePosition(vsUpdatable *updatable,
                                         unsigned long newPosition);
    unsigned long   getUpdatablePosition(vsUpdatable *updatable);

    unsigned long   getUpdatableCount(void);
    vsUpdatable     *getUpdatable(unsigned long i);

    virtual void    update(void);
};

#endif
