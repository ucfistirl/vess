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
//    VESS Module:  vsMPEGReader.c++
//
//    Description:  Class for reading an MPEG file and outputting frames
//                  of image data
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "vsMPEGReader.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsMPEGReader::vsMPEGReader()
{
    // Initialize object variables
    mpegFile = NULL;
    imageWidth = 0;
    imageHeight = 0;
    timePerFrame = 0.0;
    outputBuffer = NULL;
    currentFrameTime = 0.0;
    totalFileTime = 0.0;
    playMode = VS_MPEG_STOPPED;
    mpegInfo = NULL;

    // Create the MPEG decoder object
    mpegDecoder = mpeg2_init();
    if (!mpegDecoder)
        printf("vsMPEGReader::vsMPEGReader: Unable to construct mpeg "
            "library object\n");
    else
        mpegInfo = mpeg2_info(mpegDecoder);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMPEGReader::~vsMPEGReader()
{
    // Close the currently active file, if any
    if (mpegFile)
        fclose(mpegFile);

    // Delete the MPEG decoder object
    if (mpegDecoder)
        mpeg2_close(mpegDecoder);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMPEGReader::getClassName()
{
    return "vsMPEGReader";
}

// ------------------------------------------------------------------------
// Set the MPEG reader to extract data from the given file
// ------------------------------------------------------------------------
bool vsMPEGReader::openFile(char *filename)
{
    // If there's no valid MPEG decoder for this object, abort
    if (!mpegDecoder)
        return false;

    // Close the current MPEG file
    closeFile();

    // Open the new MPEG file; signal an error and return false if there's
    // a problem
    mpegFile = fopen(filename, "r");
    if (!mpegFile)
    {
        printf("vsMPEGReader::openFile: Unable to open file '%s'\n", filename);
        return false;
    }

    // Reset the MPEG time parameters (We have to do this _before_ we call
    // readNextframe(), as that function won't work if the play mode is
    // STOPPED.)
    currentFrameTime = 0.0;
    totalFileTime = 0.0;
    playMode = VS_MPEG_PLAYING;

    // Prime the decoder by pulling in the first frame of the MPEG
    readNextFrame();

    // Obtain the MPEG size and speed from the decoder object
    imageWidth = mpegInfo->sequence->width;
    imageHeight = mpegInfo->sequence->height;
    timePerFrame = (double)(mpegInfo->sequence->frame_period) / 27000000.0;

    // Return success
    return true;
}

// ------------------------------------------------------------------------
// Close the current MPEG file
// ------------------------------------------------------------------------
void vsMPEGReader::closeFile()
{
    // If there's no valid MPEG decoder for this object, abort
    if (!mpegDecoder)
        return;

    // Close the file
    if (mpegFile)
    {
        fclose(mpegFile);
        mpegFile = NULL;
    }

    // Reset the image parameters
    imageWidth = 0;
    imageHeight = 0;
    timePerFrame = 0.0;
    currentFrameTime = 0.0;
    totalFileTime = 0.0;
    playMode = VS_MPEG_STOPPED;
}

// ------------------------------------------------------------------------
// Gets the width of a single frame of the MPEG
// ------------------------------------------------------------------------
int vsMPEGReader::getWidth()
{
    return imageWidth;
}

// ------------------------------------------------------------------------
// Gets the height of a single frame of the MPEG
// ------------------------------------------------------------------------
int vsMPEGReader::getHeight()
{
    return imageHeight;
}

// ------------------------------------------------------------------------
// Gets the required size of the MPEG frame storage area
// ------------------------------------------------------------------------
int vsMPEGReader::getDataSize()
{
    return (imageWidth * imageHeight * 3);
}

// ------------------------------------------------------------------------
// Gets the number of seconds each frame fo the MPEG should be displayed
// ------------------------------------------------------------------------
double vsMPEGReader::getTimePerFrame()
{
    return timePerFrame;
}

// ------------------------------------------------------------------------
// Sets the pointer to the buffer that the reader should store the MPEG
// frame images in. Automatically copies the current frame of the MPEG
// to the buffer, if it is running.
// ------------------------------------------------------------------------
void vsMPEGReader::setOutputBuffer(unsigned char *dataOutputBuffer)
{
    outputBuffer = dataOutputBuffer;

    if (playMode == VS_MPEG_PLAYING)
        copyFrame();
}

// ------------------------------------------------------------------------
// Sets the pointer to the buffer that the reader should store the MPEG
// frame images in
// ------------------------------------------------------------------------
unsigned char *vsMPEGReader::getOutputBuffer()
{
    return outputBuffer;
}

// ------------------------------------------------------------------------
// Forces the MPEG reader to read in the next frame. Does not change any of
// the time attributes. Also copies the frame image to the output data
// area, if that has been set.
// ------------------------------------------------------------------------
void vsMPEGReader::advanceFrame()
{
    // If there's no valid MPEG decoder for this object, abort
    if (!mpegDecoder)
        return;

    // Read in the next frame from the MPEG file
    readNextFrame();

    // Copy the data to the output area
    copyFrame();
}

// ------------------------------------------------------------------------
// Advances the MPEG timer by the specified amount. Will advance the MPEG
// to the next frame if the timer runs over the amount of allotted time for
// the current frame. Will only copy the frame data to the output buffer if
// the current frame changes.
// ------------------------------------------------------------------------
void vsMPEGReader::advanceTime(double seconds)
{
    bool frameAdvanced = false;

    // If there's no valid MPEG decoder for this object, abort
    if (!mpegDecoder)
        return;

    // Add the specified time to the MPEG timer
    currentFrameTime += seconds;
    totalFileTime += seconds;

    // If the time for the current frame is greater than the MPEG's
    // time-per-frame, then advance the frame
    while (currentFrameTime > timePerFrame)
    {
        currentFrameTime -= timePerFrame;
        readNextFrame();
        frameAdvanced = true;
    }

    // Copy the frame data over, if we advanced to a new one
    if (frameAdvanced)
        copyFrame();
}

// ------------------------------------------------------------------------
// Returns the total elapsed time for the MPEG
// ------------------------------------------------------------------------
double vsMPEGReader::getTotalTime()
{
    return totalFileTime;
}

// ------------------------------------------------------------------------
// Rewinds the MPEG back to the beginning
// ------------------------------------------------------------------------
void vsMPEGReader::restart()
{
    // If there's no valid MPEG decoder for this object, abort
    if (!mpegDecoder)
        return;

    // Make sure we have a file open already
    if (!mpegFile)
        return;

    // Reposition the file marker at the beginning
    rewind(mpegFile);

    // Reset the MPEG timers
    currentFrameTime = 0.0;
    totalFileTime = 0.0;

    // Mark the file as ready to play (we have to do this _before_ we call
    // advanceFrame(), as that function won't work if the play mode is
    // STOPPED.)
    playMode = VS_MPEG_PLAYING;

    // Re-prime the MPEG
    advanceFrame();
}

// ------------------------------------------------------------------------
// Returns the current mode (playing or stopped) for the MPEG
// ------------------------------------------------------------------------
int vsMPEGReader::getPlayMode()
{
    return playMode;
}

// ------------------------------------------------------------------------
// Private function
// Gets the next frame's worth of image information from the MPEG file
// ------------------------------------------------------------------------
void vsMPEGReader::readNextFrame()
{
    int mpegState;
    int readSize;
    int status;

    // Can't do anything if we're not in PLAYING mode
    if (playMode != VS_MPEG_PLAYING)
        return;

    // Read data from the MPEG file and hand it to the MPEG decoder object.
    // Keep doing this until the decoder object signals that we have a full
    // frame's worth of data.
    status = 0;
    do
    {
        mpegState = mpeg2_parse(mpegDecoder);

        switch (mpegState)
        {
            case -1:
                // Read in 4K of data and feed it to the decoder. This data is
                // stored in the object's data buffer, rather than in a local
                // variable, because the MPEG decoder object keeps pointers to
                // the data buffer.
                readSize = fread(mpegDataBuffer, 1, VS_MPEG_BUFFER_SIZE, mpegFile);
                mpeg2_buffer(mpegDecoder, mpegDataBuffer,
                    mpegDataBuffer + readSize);

                // If readSize comes back zero, then we're at the end-of-file
                if (readSize == 0)
                    status = -1;
                break;

            case STATE_SEQUENCE:
                // Specify that we want the output in 3-bytes-per-pixel RGB
                // format
                mpeg2_convert(mpegDecoder, convert_rgb24, NULL);
                break;

            case STATE_SLICE:
            case STATE_END:
                // If a pointer to the output image exists, then we're done
                if (mpegInfo->display_fbuf)
                    status = 1;
                break;
        }
    }
    while (status == 0);

    // If we exited the while loop by some other method than getting a full
    // frame (when readSize <= 0), then we must have hit the end of the MPEG
    // file. Set the play mode to STOPPED.
    if (status == -1)
        playMode = VS_MPEG_STOPPED;
}

// ------------------------------------------------------------------------
// Private function
// Copies the data in the MPEG decoder's internal buffer to the image data
// area specified for this reader object
// ------------------------------------------------------------------------
void vsMPEGReader::copyFrame()
{
    // If we're not PLAYING, then there's nothing to copy
    if (playMode != VS_MPEG_PLAYING)
        return;

    // Only copy the data if there is a place to copy it to
    if (outputBuffer)
    {
        // Copy the data, if it exists
        if (mpegInfo->display_fbuf)
            memcpy(outputBuffer, mpegInfo->display_fbuf->buf[0], getDataSize());
    }
}
