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
//    VESS Module:  vsSequencer.c++
//
//    Description:  This object is ment to handle scheduleing of
//                  vsUpdatables.  It will allow more control
//                  for a various ammount of updatables, including
//                  insertion of latencies and order of updating.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsSequencer.h++"

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

// ------------------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------------------
vsSequencer::vsSequencer(void) : vsUpdatable()
{
    // Initialize the list to empty.
    updatableCount = 0;
    updatableListHead = NULL;
    updatableListTail = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsSequencer::~vsSequencer(void)
{
    UpdatableEntry  *tempEntry;

    // Delete elements of the list.
    while (updatableListHead)
    {
        tempEntry = updatableListHead;
        updatableListHead = updatableListHead->next;

        // Unreference the vsUpdatable
        vsObject::unrefDelete( tempEntry->updatable );

        free(tempEntry);
    }
}

// ------------------------------------------------------------------------
// Return the name of this class (inherited from vsObject)
// ------------------------------------------------------------------------
const char * vsSequencer::getClassName()
{
    return "vsSequencer";
}

// ------------------------------------------------------------------------
// Add the specified updatable to the end of this sequencer.
// Set the time to 0, which means run as fast as you can.
// ------------------------------------------------------------------------
void vsSequencer::addUpdatable(vsUpdatable *updatable, char *name)
{
    addUpdatable(updatable, 0.0, name);
}

// ------------------------------------------------------------------------
// Add the specified updatable to the end of this sequencer.
// Set how much time it should take as well.
// ------------------------------------------------------------------------
void vsSequencer::addUpdatable(vsUpdatable *updatable, double time, char *name)
{
    UpdatableEntry  *tempEntry;

    // Allocate the structure for the updatable.
    tempEntry = (UpdatableEntry *) malloc(sizeof(UpdatableEntry));

    // If it could not allocate, print error and return.
    if (!tempEntry)
    {
        printf("vsSequencer::addUpdatable: Unable to allocate space"
            "for updatable!\n");
        return;
    }

    // Reference the vsUpdatable
    updatable->ref();

    // Set the structures values.
    tempEntry->updatable = updatable;
    tempEntry->time = time;
    strncpy(tempEntry->name, name, VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH - 1);
    tempEntry->name[VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH - 1] = '\0';
    tempEntry->next = NULL;

    // Insert it to the end of the list.
    if (!updatableListHead)
    {
        updatableListHead = tempEntry;
    }
    else
    {
        updatableListTail->next = tempEntry;
    }

    tempEntry->prev = updatableListTail;
    updatableListTail = tempEntry;

    // Increment the count of updatables.
    updatableCount++;
}

// ------------------------------------------------------------------------
// Remove the specified updatable from this sequencer.
// ------------------------------------------------------------------------
void vsSequencer::removeUpdatable(vsUpdatable *updatable)
{
    UpdatableEntry  *tempEntry;
    bool            found;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    while ((tempEntry) && (!found))
    {
        // If this is the structure we are to remove.
        if (tempEntry->updatable == updatable)
        {
            // Remove the entry from the list.
            removeEntryFromList(tempEntry);

            // Unreference the vsUpdatable
            vsObject::unrefDelete( tempEntry->updatable );

            // Delete the structure, decrement count,
            // and set the found variable to true.
            free(tempEntry);
            updatableCount--;
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::removeUpdatable: Updatable not found!\n");
    }
}

// ------------------------------------------------------------------------
// Return the time the specified updatable is set to take.  Zero means
// it has no limits, it updates as fast as it can.
// ------------------------------------------------------------------------
double vsSequencer::getUpdatableTime(vsUpdatable *updatable)
{
    UpdatableEntry  *tempEntry;
    bool            found;
    double          time;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    time = 0.0;
    while ((tempEntry) && (!found))
    {
        // If this is the entry we want, stop traversing.
        // Also get its time so we can return it.
        if (tempEntry->updatable == updatable)
        {
            time = tempEntry->time;
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::getUpdatableTime: Updatable not found!\n");
    }

    return (time);
}

// ------------------------------------------------------------------------
// Set the time the specified updatable should take.
// ------------------------------------------------------------------------
void vsSequencer::setUpdatableTime(vsUpdatable *updatable, double time)
{
    UpdatableEntry  *tempEntry;
    bool            found;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    while ((tempEntry) && (!found))
    {
        // If this is the entry we want, stop traversing.
        // Also set its time.
        if (tempEntry->updatable == updatable)
        {
            tempEntry->time = time;
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::setUpdatableTime: Updatable not found!\n");
    }
}

// ------------------------------------------------------------------------
// Return the name the specified updatable is known by.
// ------------------------------------------------------------------------
char *vsSequencer::getUpdatableName(vsUpdatable *updatable)
{
    UpdatableEntry  *tempEntry;
    bool            found;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    while ((tempEntry) && (!found))
    {
        // If this is the entry we want, stop traversing.
        if (tempEntry->updatable == updatable)
        {
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::getUpdatableName: Updatable not found!\n");
        return (NULL);
    }
    else
       return (tempEntry->name);
}

// ------------------------------------------------------------------------
// Set the name the specified updatable should take.
// ------------------------------------------------------------------------
void vsSequencer::setUpdatableName(vsUpdatable *updatable, char *name)
{
    UpdatableEntry  *tempEntry;
    bool            found;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    while ((tempEntry) && (!found))
    {
        // If this is the entry we want, stop traversing.
        // Also set its name.
        if (tempEntry->updatable == updatable)
        {
            strncpy(tempEntry->name, name, 
                    VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH - 1);
            tempEntry->name[VS_SEQUENCER_MAX_UPDATABLE_NAME_LENGTH - 1] = '\0';
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::setUpdatableName: Updatable not found!\n");
    }
}

// ------------------------------------------------------------------------
// Set the specified updatable to be in the specified position, if possible.
// ------------------------------------------------------------------------
void vsSequencer::setUpdatablePosition(vsUpdatable *updatable,
                                       unsigned long newPosition)
{
    UpdatableEntry  *tempEntry;
    UpdatableEntry  *sourceEntry;
    UpdatableEntry  *destinationEntry;
    bool            found;
    unsigned long   position;

    // Go through the list, starting at the head.
    sourceEntry = NULL;
    destinationEntry = NULL;
    tempEntry = updatableListHead;
    found = false;
    while ((tempEntry) && (!found))
    {
        // If we are in the position where we want to move to, remember
        // what entry is currently there.
        if (position == newPosition)
        {
            destinationEntry = tempEntry;
            found = ((destinationEntry) && (sourceEntry));
        }

        // If this is the entry with the given updatable, remember
        // what entry it is.
        if (tempEntry->updatable == updatable)
        {
            sourceEntry = tempEntry;
            found = ((destinationEntry) && (sourceEntry));
        }

        // If we have not found both entries, continue down the list.
        if ((!destinationEntry) || (!sourceEntry))
        {
            tempEntry = tempEntry->next;

            // Increment the position as we move through the list.
            if (!destinationEntry)
            {
                position++;
            }
        }
    }

    // If both the entries were found, modify the list accordingly.
    if ((destinationEntry) && (sourceEntry))
    {
        // If the source and destination are not equal.
        if (sourceEntry != destinationEntry)
        {
            // Remove the sourceEntry from the list.
            removeEntryFromList(sourceEntry);

            // Place the now removed source where the destinationEntry is,
            // the other entries will be shifted down accordingly.

            // If the list is empty.
            if (!updatableListHead)
            {
                updatableListHead = sourceEntry;
                updatableListTail = sourceEntry;
            }
            // Else if the destination is the head of the list.
            else if (destinationEntry == updatableListHead)
            {
                sourceEntry->next = updatableListHead;
                updatableListHead->prev = sourceEntry;
                updatableListHead = sourceEntry;
            }
            // Else the destination is somewhere in the list with a valid
            // destinationEntry->prev pointer.
            else
            {
                sourceEntry->next = destinationEntry;
                sourceEntry->prev = destinationEntry->prev;
                destinationEntry->prev->next = sourceEntry;
                destinationEntry->prev = sourceEntry;
            }
        }
        // Else it is already where it belongs.
    }
    // Else if no destination was found, then the source belongs in the end.
    else if ((!destinationEntry) && (sourceEntry))
    {
        // If the source is not already int the tail.
        if (sourceEntry != updatableListTail)
        {
            // Remove the sourceEntry from the list.
            removeEntryFromList(sourceEntry);

            // Place at the end of the list.
            sourceEntry->prev = updatableListTail;
            updatableListTail->next = sourceEntry;
            updatableListTail = sourceEntry;
        }
        // Else it is already where it belongs.
    }
    // Else it cannot be moved.
}

// ------------------------------------------------------------------------
// Return the position the specified updatable is in.  If it cannot be
// found than it will return the last position.
// ------------------------------------------------------------------------
unsigned long vsSequencer::getUpdatablePosition(vsUpdatable *updatable)
{
    UpdatableEntry  *tempEntry;
    bool            found;
    unsigned long   position;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;
    position = 0;
    while ((tempEntry) && (!found))
    {
        // If this is the entry we want, stop traversing.
        if (tempEntry->updatable == updatable)
        {
            found = true;
        }
        // Else continue down the list.
        else
        {
            tempEntry = tempEntry->next;

            // Increment the position as we move through the list.
            position++;
        }
    }

    // If it was not found, print an error.
    if (!found)
    {
        printf("vsSequencer::getUpdatablePosition: Updatable not found!\n");
    }

    return (position);
}

// ------------------------------------------------------------------------
// Return the ith updatable this sequencer has.  NULL if not found.
// ------------------------------------------------------------------------
vsUpdatable *vsSequencer::getUpdatable(unsigned long i)
{
    UpdatableEntry  *tempEntry;
    unsigned long   currentPosition;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    currentPosition = 0;

    // Make sure the requested updatable is in the list
    if (i >= updatableCount)
    {
        // We don't have enough entries so print that and return NULL
        printf("vsSequencer::getUpdatable: Updatable not found!\n");
        return NULL;
    }
    else
    {
        // Go through the list until the ith position
        while ((tempEntry) && (currentPosition < i))
        {
            // Advance to the next updatable entry and increment the count
            tempEntry = tempEntry->next;
            currentPosition++;
        }

        // Return the updatable
        return tempEntry->updatable;
    }
}

// ------------------------------------------------------------------------
// Return how many updatables this sequencer has.
// ------------------------------------------------------------------------
unsigned long vsSequencer::getUpdatableCount(void)
{
    return updatableCount;
}

// ------------------------------------------------------------------------
// Return the updatable with this name.  NULL if not found.
// ------------------------------------------------------------------------
vsUpdatable *vsSequencer::getUpdatableByName(char *name)
{
    UpdatableEntry  *tempEntry;
    bool            found;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    found = false;

    // Go through the list until the item is found
    while ((tempEntry) && (!found))
    {
        // Check if the names match (meaning we found it)
        if (strcmp(tempEntry->name, name) == 0)
            found = true;
        else
        {
           // Advance to the next updatable entry if not found yet
           tempEntry = tempEntry->next;
        }
    }

    // Return appropriate value
    if (!found)
        return NULL;
    else
        return tempEntry->updatable;
}

// ------------------------------------------------------------------------
// Update all the updatables this sequencer manages.
// ------------------------------------------------------------------------
void vsSequencer::update(void)
{
    UpdatableEntry  *tempEntry;
    timeval         timeValue;
    double          beginTime;
    double          tempTime;
    double          endTime;
    unsigned long   sleepTime;

    // Go through the list, starting at the head.
    tempEntry = updatableListHead;
    while (tempEntry)
    {
        if (tempEntry->time != 0.0)
        {
            // Get the time we begin, convert to seconds in a double.
            gettimeofday(&timeValue, NULL);
            beginTime = ((double) timeValue.tv_sec) +
                ((double) timeValue.tv_usec / 1000000.0);

            // Update the updatable.
            tempEntry->updatable->update();

            // Calculate when the updating should be over.
            endTime = beginTime + tempEntry->time;

            // Get the current time, convert to seconds in a double.
            gettimeofday(&timeValue, NULL);
            tempTime = ((double) timeValue.tv_sec) +
                ((double) timeValue.tv_usec / 1000000.0); 

/* Busy Wait */
/*
            // While we have not reached the end time, keep querying the time.
            while (tempTime > endTime)
            {
                // Get the current time, convert to seconds in a double.
                gettimeofday(&timeValue, NULL);
                tempTime = ((double) timeValue.tv_sec) +
                    ((double) timeValue.tv_usec / 1000000.0); 
            }
*/
/* End Busy Wait */
/* Sleep Wait */
            if (tempTime > endTime)
            {
                tempTime -= endTime;
                sleepTime = ((unsigned long) (tempTime * 1000000.0));
                usleep(sleepTime);
            }
/* End Sleep Wait */
        }
        else
        {
            tempEntry->updatable->update();
        }

        tempEntry = tempEntry->next;
    }
}

// ------------------------------------------------------------------------
// Private function that removed the specified entry from the list.
// The argument must be an entry that is currently in the list.
// ------------------------------------------------------------------------
void vsSequencer::removeEntryFromList(UpdatableEntry *entry)
{
    // If the element to be removed is in the beginning.
    if (entry == updatableListHead)
    {
        updatableListHead = entry->next;
        if (updatableListHead)
        {
            updatableListHead->prev = NULL;
        }
        else
        {
            updatableListTail = NULL;
        }
    }
    // If the element to be removed is in the end.
    else if (entry == updatableListTail)
    {
        updatableListTail = entry->prev;
        if (updatableListTail)
        {
            updatableListTail->next = NULL;
        }
        else
        {
            updatableListHead = NULL;
        }
    }
    // Else the element to be removed is in the middle.
    else
    {
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
    }

    // Insure the removed node does not point to anything.
    entry->next = NULL;
    entry->prev = NULL;
}

