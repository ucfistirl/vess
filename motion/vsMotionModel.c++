#include "vsMotionModel.h++"
#include <sys/time.h>

// ------------------------------------------------------------------------
// Constructor for the vsMotionModel class
// ------------------------------------------------------------------------
vsMotionModel::vsMotionModel()
{
    lastTime = 0.0;
    getTimeInterval();
}

// ------------------------------------------------------------------------
// Destructor for the vsMotionModel class
// ------------------------------------------------------------------------
vsMotionModel::~vsMotionModel()
{
}

// ------------------------------------------------------------------------
// Returns the interval between now and the last time this function was 
// called
// ------------------------------------------------------------------------
double vsMotionModel::getTimeInterval()
{
    struct timeval tv;
    double         currentTime;
    double         deltaTime;

    gettimeofday(&tv, NULL);

    currentTime = tv.tv_sec + tv.tv_usec / 1E6;
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    return deltaTime;
}
