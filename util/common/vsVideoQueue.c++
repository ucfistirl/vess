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
//    VESS Module:  vsVideoQueue.c++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsVideoQueue.h++"
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------------------
// Constructor
// This constructor initializes a video queue to hold 'capacity' RGB images
// each of dimensions 'width' X 'height'.
// ------------------------------------------------------------------------
vsVideoQueue::vsVideoQueue(int width, int height, int capacity)
{
    // Store the stream properties.
    streamWidth = width;
    streamHeight = height;

    // For now, the number of bytes per pixel is only based on RGB format.
    bytesPerPixel = 3;

    // Calculate the number of bytes required for each complete image.
    bytesPerImage = streamWidth * streamHeight * bytesPerPixel;

    // Store the queue capacity and create the ring buffer.
    if (capacity <= 0)
    {
        bufferCapacity = bytesPerImage + sizeof(double);
    }
    else
    {
        bufferCapacity = capacity * (bytesPerImage + sizeof(double));
    }

    // Create the ring buffer and initialize the tail at byte 0.
    bufferTail = 0;
    ringBuffer = (unsigned char *)malloc(bufferCapacity);

    // Begin with zero references.
    totalRefCount = 0;
    referenceListHead = NULL;
}

// ------------------------------------------------------------------------
// Destructor - This destructor does nothing because the parent class
// destructor frees all memory.
// ------------------------------------------------------------------------
vsVideoQueue::~vsVideoQueue()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsVideoQueue::getClassName()
{
    return "vsVideoQueue";
}

// ------------------------------------------------------------------------
// Returns the width of images stored in this video stream.
// ------------------------------------------------------------------------
int vsVideoQueue::getWidth()
{
    return streamWidth;
}

// ------------------------------------------------------------------------
// Returns the height of images stored in this video stream.
// ------------------------------------------------------------------------
int vsVideoQueue::getHeight()
{
    return streamHeight;
}

// ------------------------------------------------------------------------
// Returns the number of bytes per pixel in this image. Currently this is
// fixed at 3 (one each for red, blue, and green).
// ------------------------------------------------------------------------
int vsVideoQueue::getBytesPerPixel()
{
    return bytesPerPixel;
}

// ------------------------------------------------------------------------
// Returns the number of bytes per complete image.
// ------------------------------------------------------------------------
int vsVideoQueue::getBytesPerImage()
{
    return bytesPerImage;
}

// ------------------------------------------------------------------------
// This method will copy the data from the image with the given timestamp.
// ------------------------------------------------------------------------
void vsVideoQueue::enqueue(char *image, double timestamp)
{
    // Enqueue the timestamp and then the image data.
    vsMultiQueue::enqueue(&timestamp, sizeof(double));
    vsMultiQueue::enqueue(image, bytesPerImage);
}

// ------------------------------------------------------------------------
// This method will fill the provided pointers with the image and timestamp
// data of the first frame in the queue and return true, or return false if
// there is no data in the list. This method removes the data from the
// queue.
// ------------------------------------------------------------------------
bool vsVideoQueue::dequeue(char *image, double *timestamp, int id)
{
    return (readBuffer(timestamp, 0, sizeof(double), id, true) &&
        readBuffer(image, 0, bytesPerImage, id, true));
}

// ------------------------------------------------------------------------
// This method will fill the provided pointers with the image and timestamp
// data of the first frame in the queue and return true, or return false if
// there is no data in the list. This method leaves the data in the queue.
// ------------------------------------------------------------------------
bool vsVideoQueue::peek(char *image, double *timestamp, int id)
{
    // See if the timestamp value is requested.
    if (timestamp)
    {
        // Attempt to read the timestamp from the buffer, returning false if
        // the operation fails.
        if (!readBuffer(timestamp, 0, sizeof(double), id, false))
        {
            return false;
        }
    }

    // See if the image data is requested.
    if (image)
    {
        // Attempt to read the timestamp from the buffer, returning false if
        // the operation fails.
        if (!readBuffer(image, sizeof(double), bytesPerImage, id, false))
        {
            return false;
        }
    }

    // Return true, indicating that all requested operations succeeded.
    return true;
}

