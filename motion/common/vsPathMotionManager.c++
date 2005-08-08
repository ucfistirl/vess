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
//    VESS Module:  vsPathMotionManager.c++
//
//    Description:  A manager to control several vsPathMotion objects
//                  simultaneously.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsPathMotionManager.h++"

//------------------------------------------------------------------------
// Constructor
// Initializes private variables
//------------------------------------------------------------------------
vsPathMotionManager::vsPathMotionManager()
{
    // Set current play status to STOPPED
    currentPlayMode = VS_PATH_STOPPED;

    // Set default cycling mode to restart, set target iterations to one,
    // and initialize the current iteration to zero
    cycleMode = VS_PATH_CYCLE_RESTART;
    cycleCount = 1;
    currentCycleCount = 0;

    pathMotionSequencer = new vsSequencer();
    pathMotionCount = 0;
}

//------------------------------------------------------------------------
// Copy constructor.
//------------------------------------------------------------------------
vsPathMotionManager::vsPathMotionManager(vsPathMotionManager *original)
{
    int index;
    vsPathMotion *newPathMotion;
    
    // Copy the current play mode.
    currentPlayMode = original->currentPlayMode;
   
    // Copy the cycle information.
    cycleMode = original->cycleMode;
    cycleCount = original->cycleCount;
    currentCycleCount = original->currentCycleCount;
   
    // Go through the list of vsPathMotion's in the original manager and
    // use their copy constructors to make ours.
    pathMotionSequencer = new vsSequencer();
    pathMotionCount = 0;
    
    // Create all the new vsPathMotion's.
    for (index = 0; index < original->pathMotionCount; index++)
    { 
       // Create the new vsPathMotion as a copy of the original at this index.
       newPathMotion = new vsPathMotion(original->getPathMotion(index));
       
       // Add it to our list.
       addPathMotion(newPathMotion);
    }
}

//------------------------------------------------------------------------
// Destructor
// Deletes privately allocated variables
//------------------------------------------------------------------------
vsPathMotionManager::~vsPathMotionManager()
{
    delete pathMotionSequencer;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPathMotionManager::getClassName()
{
    return "vsPathMotionManager";
}

//------------------------------------------------------------------------
// Sets the path repetition mode
//------------------------------------------------------------------------
void vsPathMotionManager::setCycleMode(int mode)
{
    int index;

    cycleMode = mode;

    // Set all the vsPathMotion's cycle mode.
    for (index = 0; index < pathMotionCount; index++)
    {
        ((vsPathMotion *)
            pathMotionSequencer->getUpdatable(index))->setCycleMode(cycleMode);
    }
}

//------------------------------------------------------------------------
// Sets the number of times to cycle through the path before stopping.
// The constant VS_PATH_REPEAT_FOREVER directs the path to run an infinite
// number of times.
//------------------------------------------------------------------------
void vsPathMotionManager::setCycleCount(int cycles)
{
    int index;

    if (cycleCount >= 0)
    {
        cycleCount = cycles;

        // Set all the vsPathMotion's cycle count.
        for (index = 0; index < pathMotionCount; index++)
        {
            ((vsPathMotion *)
                pathMotionSequencer->getUpdatable(index))->setCycleCount(
                cycles);
        }
    }
    else
        printf("vsPathMotionManager::setCycleCount: Invalid cycle count (%d)\n",
            cycles);
}

//------------------------------------------------------------------------
// Gets the path repetition mode
//------------------------------------------------------------------------
int vsPathMotionManager::getCycleMode()
{
    return cycleMode;
}

//------------------------------------------------------------------------
// Gets the number of times to cycle through the path before stopping.
// A return value of VS_PATH_REPEAT_FOREVER indicates that the path is set
// to run indefinitely.
//------------------------------------------------------------------------
int vsPathMotionManager::getCycleCount()
{
    return cycleCount;
}

//------------------------------------------------------------------------
// Starts the path motion
//------------------------------------------------------------------------
void vsPathMotionManager::startResume()
{
    int index;

    currentPlayMode = VS_PATH_PLAYING;

    // Set all the vsPathMotions to playing.
    for (index = 0; index < pathMotionCount; index++)
    {
        ((vsPathMotion *)
            pathMotionSequencer->getUpdatable(index))->startResume();
    }
}

//------------------------------------------------------------------------
// Pauses the path motions
//------------------------------------------------------------------------
void vsPathMotionManager::pause()
{
    int index;

    currentPlayMode = VS_PATH_PAUSED;

    // Set all the vsPathMotions to pause.
    for (index = 0; index < pathMotionCount; index++)
    {
        ((vsPathMotion *) pathMotionSequencer->getUpdatable(index))->pause();
    }
}

//------------------------------------------------------------------------
// Stops the path motions, resetting it back to the beginning
//------------------------------------------------------------------------
void vsPathMotionManager::stop()
{
    int index;

    currentPlayMode = VS_PATH_STOPPED;

    // Set all the vsPathMotions to stop.
    for (index = 0; index < pathMotionCount; index++)
    {
        ((vsPathMotion *) pathMotionSequencer->getUpdatable(index))->stop();
    }
}

//------------------------------------------------------------------------
// Gets the current play mode of all the vsPathMotions for this object.
//------------------------------------------------------------------------
int vsPathMotionManager::getPlayMode()
{
    return currentPlayMode;
}

//------------------------------------------------------------------------
// Updates the sequencer containing all the vsPathMotion objects.
//------------------------------------------------------------------------
void vsPathMotionManager::update()
{
    pathMotionSequencer->update();
}

//------------------------------------------------------------------------
// Updates the list of PathMotion's with a delta (changed) time.
//------------------------------------------------------------------------
void vsPathMotionManager::update(double deltaTime)
{
    int index;
    
    // Loop through the list
    for (index = 0; index < pathMotionCount; index++)
    {   
        // Update the path motion with the delta time.
        ((vsPathMotion *) pathMotionSequencer->getUpdatable(index))->
               update(deltaTime);
    }
}

//------------------------------------------------------------------------
// Add a vsPathMotion to the list.
//------------------------------------------------------------------------
void vsPathMotionManager::addPathMotion(vsPathMotion *pathMotion)
{
    pathMotionSequencer->addUpdatable(pathMotion, "");
    pathMotionCount = pathMotionSequencer->getUpdatableCount();
}

//------------------------------------------------------------------------
// Remove a vsPathMotion from the list.
//------------------------------------------------------------------------
void vsPathMotionManager::removePathMotion(vsPathMotion *pathMotion)
{
    pathMotionSequencer->removeUpdatable(pathMotion);
    pathMotionCount = pathMotionSequencer->getUpdatableCount();
}

//------------------------------------------------------------------------
// Get the vsPathMotion at the specified index.
//------------------------------------------------------------------------
vsPathMotion *vsPathMotionManager::getPathMotion(int index)
{
    return ((vsPathMotion *) pathMotionSequencer->getUpdatable(index));
}

//------------------------------------------------------------------------
// Get the vsPathMotion at the specified index.
//------------------------------------------------------------------------
int vsPathMotionManager::getPathMotionCount()
{
    return pathMotionCount;
}

//------------------------------------------------------------------------
// Check whether all of the PathMotion's are stopped or not.
//------------------------------------------------------------------------
bool vsPathMotionManager::isDone()
{
    int index;
    
    // Loop through the list
    for (index = 0; index < pathMotionCount; index++)
    {
        // If this path motion is still continuing, then the path motion as a
        // whole is still going, and therefore this group is not done yet.
        if (((vsPathMotion *) pathMotionSequencer->getUpdatable(index))->
               getPlayMode() != VS_PATH_STOPPED)
        {
            return false;
        }
    }
    
    // Since none of the PathMotion's were not stopped, then this group of
    // PathMotion's is finished.
    return true;
}
