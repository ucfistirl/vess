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
//    VESS Module:  vsMovieReader.c++
//
//    Description:  Class for reading a video file and outputting frames
//                  of image data
//
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsMovieReader.h++"
#include "vsSoundManager.h++"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
    #include <unistd.h>
#endif

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsMovieReader::vsMovieReader()
{
    // Initialize object variables
    movieFile = NULL;
    videoCodecContext = NULL;
    videoCodec = NULL;
    videoStreamIndex = -1;
    audioCodecContext = NULL;
    audioCodec = NULL;
    audioStreamIndex = -1;
    imageWidth = 0;
    imageHeight = 0;
    timePerFrame = 0.0;
    outputBuffer = NULL;
    currentTime = 0.0;
    totalFileTime = 0.0;
    playMode = VS_MOVIE_STOPPED;
    memset(audioBuffer, 0, sizeof(audioBuffer));
    audioBufferSize = 0;
    scaleContext = NULL;

    // Initialize packet queues
    videoQueue = new vsMoviePacketQueue;
    videoQueue->head = NULL;
    videoQueue->tail = NULL;
    videoQueue->packetCount = 0;
    audioQueue = new vsMoviePacketQueue;
    audioQueue->head = NULL;
    audioQueue->tail = NULL;
    audioQueue->packetCount = 0;

    // Initialize the sound stream
    soundStream = NULL;

    // Register all ffmpeg videoCodec objects
    av_register_all();

    // Allocate AVFrame structures to hold the current video and audio frames
    videoFrame = avcodec_alloc_frame();
    audioFrame = avcodec_alloc_frame();

    // Allocate an AVPicture structure to hold the RGB converted frame.
    // We can't actually allocate the pixel buffers yet, because we don't
    // know the size of the image
    rgbFrame = (AVPicture *)malloc(sizeof(AVPicture));
    memset(rgbFrame, 0, sizeof(rgbFrame));

    // Create the file and queue mutex objects
    pthread_mutex_init(&fileMutex, NULL);
    pthread_mutex_init(&queueMutex, NULL);
    pthread_mutex_init(&audioMutex, NULL);

    // Create the file and audio threads
    pthread_create(&fileThread, NULL, fileThreadFunc, this);
    pthread_create(&audioThread, NULL, audioThreadFunc, this);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMovieReader::~vsMovieReader()
{
    // Set the playMode to QUIT to inform the threads to terminate
    playMode = VS_MOVIE_QUIT;

    // Wait for the threads to finish
    pthread_join(fileThread, NULL);
    pthread_join(audioThread, NULL);

    // Delete the video decoder object
    if (videoCodecContext)
        avcodec_close(videoCodecContext);

    // Delete the audio decoder object
    if (audioCodecContext)
        avcodec_close(audioCodecContext);

    // Close the currently active file, if any
    if (movieFile)
        av_close_input_file(movieFile);

    // Flush and remove the packet queues
    flushQueue(videoQueue);
    delete videoQueue;
    flushQueue(audioQueue);
    delete audioQueue;

    // Free the current frame structures
    av_free(videoFrame);
    av_free(audioFrame);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMovieReader::getClassName()
{
    return "vsMovieReader";
}

// ------------------------------------------------------------------------
// Set the video reader to extract data from the given file
// ------------------------------------------------------------------------
bool vsMovieReader::openFile(char *filename)
{
    int errorCode;

    // Close the current video file
    closeFile();

    // Acquire the file mutex
    pthread_mutex_lock(&fileMutex);

    // Open the new video file; signal an error and return false if there's
    // a problem
    errorCode = av_open_input_file(&movieFile, filename, NULL, 0, NULL);
    if (errorCode < 0)
    {
        printf("vsMovieReader::openFile: Unable to open file '%s'\n", filename);
        return false;
    }

    // Get the codec information from the file
    errorCode = av_find_stream_info(movieFile);
    if (errorCode < 0)
    {
        printf("vsMovieReader::openFile: Unable to determine codec "
            "properties in file '%s'\n", filename);
        return false;
    }

    // Start with a default frame rate of 30 fps.  If we don't find a 
    // video stream in this file, the audio buffer size will be based on
    // this rate
    timePerFrame = 1.0 / 30.0;

    // Try to find a video stream in the file.  Start the search with the 
    // first stream.
    videoStreamIndex = 0;

    // Get the video codec context for this stream
    videoCodecContext = movieFile->streams[videoStreamIndex]->codec;

    // Keep searching the file for a video stream until we find it, or
    // until we run out of streams
    while ((videoStreamIndex < movieFile->nb_streams) &&
           (videoCodecContext->codec_type != CODEC_TYPE_VIDEO))
    {
        // Try the next video stream
        videoStreamIndex++;

        // If this stream index is valid, examine the video codec context
        if (videoStreamIndex < movieFile->nb_streams)
            videoCodecContext = movieFile->streams[videoStreamIndex]->codec;
    }

    // If we didn't find a viable video codec context, reset the video 
    // variables to indicate this
    if (videoStreamIndex >= movieFile->nb_streams)
    {
        videoCodecContext = NULL;
        videoStream = NULL;
        videoCodec = NULL;
        videoStreamIndex = -1;
        scaleContext = NULL;
    }
    else
    {
        // We've found a video stream, now initialize the codecs and see if we
        // can decode it.
        videoStream = movieFile->streams[videoStreamIndex];

        // Find the appropriate video codec.
        videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);
        if (videoCodec == NULL)
        {
            printf("vsMovieReader::openFile: Unable to find appropriate video "
                "decoder!\n");

            // Reset the video variables to indicate no video.
            videoCodecContext = NULL;
            videoStream = NULL;
            videoCodec = NULL;
            videoStreamIndex = -1;
            scaleContext = NULL;
        }
        else
        {
            // Initialize the video codec.
            errorCode = avcodec_open(videoCodecContext, videoCodec);
            if (errorCode < 0)
            {
                printf("vsMovieReader::openFile: Unable to initialize video "
                    "decoder!\n");

                // Reset the video variables to indicate no video.
                videoCodecContext = NULL;
                videoStream = NULL;
                videoCodec = NULL;
                videoStreamIndex = -1;
            }
            else
            {
                // Obtain the video size.
                imageWidth = videoCodecContext->width;
                imageHeight = videoCodecContext->height;

                // Now that we know the width and height of the image, 
                // allocate the pixel buffer for the rgbFrame structure.  
                // We're allocating a single plane for RGB triplets (the same 
                // format that OpenGL textures like).
                rgbFrame->data[0] = (uint8_t *)malloc(getDataSize());
                rgbFrame->data[1] = NULL;
                rgbFrame->data[2] = NULL;
                rgbFrame->data[3] = NULL;
                rgbFrame->linesize[0] = imageWidth * 3;
                rgbFrame->linesize[1] = 0;
                rgbFrame->linesize[2] = 0;
                rgbFrame->linesize[3] = 0;

                // Get the video frame rate.  If it's not available, then
                // we'll stick with the default 30 fps rate.
                if (av_q2d(videoStream->time_base) > 1.0E-9)
                {
                    timePerFrame = av_q2d(videoStream->time_base);

                    // Some video files have their timestamps in fractional
                    // milliseconds (for example, 1/30000, instead of 1/30).
                    // See if the frame time is less than some arbitrary 
                    // number (say 1/100 sec), and multiply by 1000 if so.
                    if (timePerFrame < 0.01)
                    {
                        timePerFrame *= 1000.0;
                    }
                }

                // Create a swscale context so we can convert the image format
                // to a format we can use
                scaleContext = sws_getCachedContext(scaleContext,
                   videoCodecContext->width, videoCodecContext->height,
                   videoCodecContext->pix_fmt, imageWidth, imageHeight,
                   PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            }
        }
    }


    // Try to find an audio stream in the file.  Start the search with the 
    // first stream.
    audioStreamIndex = 0;

    // Get the audio codec context for this stream.
    audioCodecContext = movieFile->streams[audioStreamIndex]->codec;

    // Keep searching the file for a audio stream until we find it, or
    // until we run out of streams.
    while ((audioStreamIndex < movieFile->nb_streams) &&
           (audioCodecContext->codec_type != CODEC_TYPE_AUDIO))
    {
        // Try the next audio stream.
        audioStreamIndex++;

        // If this stream index is valid, examine the audio codec context
        if (audioStreamIndex < movieFile->nb_streams)  
            audioCodecContext = movieFile->streams[audioStreamIndex]->codec;
    }

    // If we didn't find a viable audio codec context, reset the audio 
    // variables to indicate this
    if (audioStreamIndex >= movieFile->nb_streams)
    {
        audioCodecContext = NULL;
        audioStream = NULL;
        audioCodec = NULL;
        audioStreamIndex = -1;
    }
    else
    {
        // We've found a audio stream, now initialize the codecs and see if
        // we can decode it.
        audioStream = movieFile->streams[audioStreamIndex];

        // Find the appropriate audio codec
        audioCodec = avcodec_find_decoder(audioCodecContext->codec_id);
        if (audioCodec == NULL)
        {
            printf("vsMovieReader::openFile: Unable to find appropriate audio "
                "decoder!\n");

            // Reset the audio variables to indicate no audio
            audioCodecContext = NULL;
            audioStream = NULL;
            audioCodec = NULL;
            audioStreamIndex = -1;
        }
        else
        {
            // Initialize the audio codec
            errorCode = avcodec_open(audioCodecContext, audioCodec);
            if (errorCode < 0)
            {
                printf("vsMovieReader::openFile: Unable to initialize audio "
                    "decoder!\n");

                // Reset the audio variables to indicate no audio
                audioCodecContext = NULL;
                audioCodec = NULL;
                audioStreamIndex = -1;
            }
            else
            {
                // Get the format of the audio stream
                sampleRate = audioCodecContext->sample_rate;
                channelCount = audioCodecContext->channels;

                // ffmpeg always decodes 16-bit audio
                sampleSize = 2;

                // Compute the number of audio samples per frame of video,
                // then quadruple it to determine a good audio buffer size.
                // This will keep the audio stream from starving while we're
                // busy decoding video.
                samplesPerFrame = (int)(timePerFrame * sampleRate) * 
                    sampleSize * channelCount * 4;

                // Acquire the audio mutex
                pthread_mutex_lock(&audioMutex);

                // Create the vsSoundStream to carry the audio data to the
                // application-provided sound source
                if (channelCount > 1)
                {
                    soundStream = 
                        new vsSoundStream(samplesPerFrame,
                            VS_SBUF_FORMAT_STEREO16, sampleRate);
                }
                else
                {
                    soundStream = 
                        new vsSoundStream(samplesPerFrame,
                            VS_SBUF_FORMAT_MONO16, sampleRate);
                }

                // Reference the sound stream
                soundStream->ref();

                // Release the audio mutex
                pthread_mutex_unlock(&audioMutex);
            }
        }
    }

    // Release the file mutex
    pthread_mutex_unlock(&fileMutex);

    // See if we got at least one kind of stream
    if (videoCodecContext || audioCodecContext)
    {
        // Attempt to calculate the total file time. Initialize the duration
        // to the value of the format context.
        totalFileTime = movieFile->duration * av_q2d(AV_TIME_BASE_Q);

        // Check the length of each stream
        for (int i = 0; i < movieFile->nb_streams; i++)
        {
            // Calculate the duration according to this stream, in its local
            // time base
            double streamDuration = movieFile->streams[i]->duration *
                av_q2d(movieFile->streams[i]->time_base);

            // If this stream is longer than any previously encountered,
            // store it as the new total file duration
            if (streamDuration > totalFileTime)
                totalFileTime = streamDuration;
        }

        // Reset the video time parameters (We have to do this _before_ we 
        // call readNextframe(), as that function won't work if the play 
        // mode is STOPPED.)
        currentTime = 0.0;
        lastTimeStamp = 0.0;
        lastFrameInterval = timePerFrame;
        playMode = VS_MOVIE_PLAYING;

        // Prime the decoder by pulling in the first frame of the video
        readNextFrame();

        // Return true to indicate that we successfully opened the file
        return true;
    }
    else
    {
        printf("vsMovieReader::openFile:  Unable to find a video or "
            "audio stream\n");
    }

    // Return false to indicate that we couldn't open the file or that we
    // have nothing to play
    return false;
}

// ------------------------------------------------------------------------
// Close the current video file
// ------------------------------------------------------------------------
void vsMovieReader::closeFile()
{
    // Set the state to STOPPED
    playMode = VS_MOVIE_STOPPED;

    // Acquire the file mutex
    pthread_mutex_lock(&fileMutex);

    // Close the video codec context
    if (videoCodecContext)
    {
        avcodec_close(videoCodecContext);
        videoCodec = NULL;
        videoCodecContext = NULL;
        videoStreamIndex = -1;

        // Also free the software scale context
        sws_freeContext(scaleContext);
        scaleContext = NULL;
    }

    // Close the audio codec context
    if (audioCodecContext)
    {
        avcodec_close(audioCodecContext);
        audioCodec = NULL;
        audioCodecContext = NULL;
        audioStreamIndex = -1;
    }

    // Close the input file
    if (movieFile)
    {
        av_close_input_file(movieFile);
        movieFile = NULL;
    }

    // Release the file mutex
    pthread_mutex_unlock(&fileMutex);

    // Delete the sound stream object if one exists
    pthread_mutex_lock(&audioMutex);
    if (soundStream != NULL)
    {
        vsObject::unrefDelete(soundStream);
        soundStream = NULL;
    }
    pthread_mutex_unlock(&audioMutex);

    // Flush the packet queues
    flushQueue(videoQueue);
    flushQueue(audioQueue);

    // Empty the audio buffer
    audioBufferSize = 0;

    // Deallocate the pixel buffers for the rgbFrame structure
    if (rgbFrame->data[0] != NULL)
    {
        free(rgbFrame->data[0]);
        rgbFrame->data[0] = NULL;
    }
    rgbFrame->linesize[0] = 0;

    // Reset the image parameters
    imageWidth = 0;
    imageHeight = 0;
    timePerFrame = 0.0;
    currentTime = 0.0;
    totalFileTime = 0.0;

    // Reset the audio parameters
    sampleSize = 0;
    sampleRate = 0;
    channelCount = 0;
}

// ------------------------------------------------------------------------
// Gets the width of a single frame of the video
// ------------------------------------------------------------------------
int vsMovieReader::getWidth()
{
    return imageWidth;
}

// ------------------------------------------------------------------------
// Gets the height of a single frame of the video
// ------------------------------------------------------------------------
int vsMovieReader::getHeight()
{
    return imageHeight;
}

// ------------------------------------------------------------------------
// Gets the required size of the video frame storage area
// ------------------------------------------------------------------------
int vsMovieReader::getDataSize()
{
    return (imageWidth * imageHeight * 3);
}

// ------------------------------------------------------------------------
// Gets the number of seconds each frame fo the video should be displayed
// ------------------------------------------------------------------------
double vsMovieReader::getTimePerFrame()
{
    return timePerFrame;
}

// ------------------------------------------------------------------------
// Returns the total time for the video
// ------------------------------------------------------------------------
double vsMovieReader::getTotalTime()
{
    return totalFileTime;
}

// ------------------------------------------------------------------------
// Returns the elapsed time for the video
// ------------------------------------------------------------------------
double vsMovieReader::getCurrentTime()
{
    return currentTime;
}

// ------------------------------------------------------------------------
// Sets the pointer to the buffer that the reader should store the video
// frame images in. Automatically copies the current frame of the video
// to the buffer, if it is running.
// ------------------------------------------------------------------------
void vsMovieReader::setVideoBuffer(unsigned char *dataOutputBuffer)
{
    outputBuffer = dataOutputBuffer;

    if ((playMode == VS_MOVIE_PLAYING) || (playMode == VS_MOVIE_EOF))
        copyFrame();
}

// ------------------------------------------------------------------------
// Gets the pointer to the buffer that the reader will store the video
// frame images in
// ------------------------------------------------------------------------
unsigned char *vsMovieReader::getVideoBuffer()
{
    return outputBuffer;
}

// ------------------------------------------------------------------------
// Gets the vsSoundStream that is carrying the movie's audio data
// ------------------------------------------------------------------------
vsSoundStream *vsMovieReader::getSoundStream()
{
    return soundStream;
}

// ------------------------------------------------------------------------
// Forces the video reader to read in the next frame. Does not change any of
// the time attributes. Also copies the frame image to the output data
// area, if that has been set.
// ------------------------------------------------------------------------
void vsMovieReader::advanceFrame()
{
    // If there's no valid video decoder for this object, abort
    if (!videoCodecContext && !audioCodecContext)
        return;

    // Read in the next frame from the video file
    readNextFrame();

    // Copy the data to the output area
    copyFrame();

    // We need to update the currentTime value to the timestamp of the
    // latest frame so that advanceTime will still work after this call
    currentTime = lastTimeStamp;
}

// ------------------------------------------------------------------------
// Advances the video timer by the specified amount. Will advance the video
// to the next frame if the timer runs over the amount of allotted time for
// the current frame. Will only copy the frame data to the output buffer if
// the current frame changes.
// ------------------------------------------------------------------------
void vsMovieReader::advanceTime(double seconds)
{
    bool frameAdvanced;

    // If there's no valid video decoder for this object, abort
    if (!videoCodecContext && !audioCodecContext)
        return;

    // Add the specified time to the video timer
    currentTime += seconds;

    // We haven't yet needed to move forward a frame
    frameAdvanced = false;

    // Read frames from the stream as long as the frame's timestamp is
    // less than the current file time
    while ((lastTimeStamp < currentTime) && 
           ((playMode == VS_MOVIE_PLAYING) || (playMode == VS_MOVIE_EOF)))
    {
        readNextFrame();
        frameAdvanced = true;
    }

    // Clip the total file time if it ran off the end of the movie.
    if (lastTimeStamp < currentTime)
        currentTime = lastTimeStamp;

    // Copy the frame data over, if we advanced to a new one
    if (frameAdvanced)
        copyFrame();
}

// ------------------------------------------------------------------------
// Attempts to jump to a specific timestamp (in seconds)
// ------------------------------------------------------------------------
void vsMovieReader::jumpToTime(double seconds)
{
    int64_t targetTimeStamp;

    // If there's no valid video decoder for this object, abort
    if (!videoCodecContext && !audioCodecContext)
        return;

    // Make sure we have a file open already
    if (!movieFile)
        return;

    // Flush both queues
    flushQueue(videoQueue);
    flushQueue(audioQueue);

    // Empty the audio buffer
    audioBufferSize = 0;

    // Acquire the file mutex
    pthread_mutex_lock(&fileMutex);

    // Make sure we have a valid stream to seek within
    if (videoStreamIndex >= 0)
    {
        // Calculate the timestamp in stream units
        targetTimeStamp = (int64_t)(seconds / av_q2d(videoStream->time_base));

        // Jump to the desired frame using the video stream as the basis
        av_seek_frame(movieFile, videoStreamIndex, targetTimeStamp, 0);

        // Flush the internal buffers of the video stream
        // TODO: Why don't we have to do this for the audio stream?
        avcodec_flush_buffers(videoCodecContext);
    }
    else if (audioStreamIndex >= 0)
    {
        // Calculate the timestamp in stream units
        targetTimeStamp = (int64_t)(seconds / av_q2d(audioStream->time_base));

        // Jump to the desired frame using the audio stream as the basis
        av_seek_frame(movieFile, audioStreamIndex, targetTimeStamp, 0);
    }

    // Release the file mutex
    pthread_mutex_unlock(&fileMutex);

    // Reset the video timers
    currentTime = seconds;
    lastTimeStamp = 0.0;
    lastFrameInterval = 0.0;

    // Mark the file as ready to play (we have to do this _before_ we call
    // advanceFrame(), as that function won't work if the play mode is
    // STOPPED.)
    playMode = VS_MOVIE_PLAYING;

    // Re-prime the video
    forceReadFrame();
    advanceFrame();
}

// ------------------------------------------------------------------------
// Rewinds the video back to the beginning
// ------------------------------------------------------------------------
void vsMovieReader::restart()
{ 
    // If there's no valid video decoder for this object, abort
    if (!videoCodecContext && !audioCodecContext)
        return;

    // Make sure we have a file open already
    if (!movieFile)
        return;

    // Flush both queues
    flushQueue(videoQueue);
    flushQueue(audioQueue);

    // Empty the audio buffer
    audioBufferSize = 0;

    // Acquire the file mutex
    pthread_mutex_lock(&fileMutex);

    // Make sure we have a valid stream to seek to the beginning of
    if ((videoStreamIndex >= 0) || (audioStreamIndex >= 0))
    {
        av_seek_frame(movieFile, -1, 0, 0);
        if (videoCodecContext != NULL)
            avcodec_flush_buffers(videoCodecContext);
    }

    // Release the file mutex
    pthread_mutex_unlock(&fileMutex);

    // Reset the video timers
    currentTime = 0.0;
    lastTimeStamp = 0.0;
    lastFrameInterval = timePerFrame;

    // Mark the file as ready to play (we have to do this _before_ we call
    // advanceFrame(), as that function won't work if the play mode is
    // STOPPED.)
    playMode = VS_MOVIE_PLAYING;

    // Re-prime the video
    forceReadFrame();
    advanceFrame();
}

// ------------------------------------------------------------------------
// Returns the current mode (playing or stopped) for the video
// ------------------------------------------------------------------------
int vsMovieReader::getPlayMode()
{
    return playMode;
}

// ------------------------------------------------------------------------
// Private function
// ------------------------------------------------------------------------
void vsMovieReader::forceReadFrame()
{
    int readStatus;
    AVPacket moviePacket;
    bool needVideo;

    // Can't do anything if we're not in a PLAYING mode
    if ((playMode != VS_MOVIE_PLAYING) && (playMode != VS_MOVIE_EOF))
        return;

    // Get the next packets from the queue
    if (videoCodecContext != NULL)
    {
        // Indicate that we still need to read at least one video frame
        needVideo = true;

        // Acquire the file mutex
        pthread_mutex_lock(&fileMutex);

        // Try to read a packet
        readStatus = av_read_frame(movieFile, &moviePacket);
        while ((readStatus >= 0) && (needVideo))
        {
            // If we got a valid packet, enqueue it on the appropriate queue
            if ((moviePacket.stream_index == videoStreamIndex) &&
                (videoCodecContext != NULL))
            {
                // Place the packet on the video queue
                enqueuePacket(videoQueue, &moviePacket);

                // We no longer need a video frame
                needVideo = false;
            }
            else if ((moviePacket.stream_index == audioStreamIndex) &&
                     (audioCodecContext != NULL))
            {
                // Place the packet on the audio queue
                enqueuePacket(audioQueue, &moviePacket);
            }
            else
            {
                // This packet comes from an unknown stream. Discard it.
                av_free_packet(&moviePacket);
            }
        }

        // Release the file mutex
        pthread_mutex_unlock(&fileMutex);
    }
}

// ------------------------------------------------------------------------
// Private function
// Gets the next frame's worth of image information from the video file
// ------------------------------------------------------------------------
void vsMovieReader::readNextFrame()
{
    int readSize;
    int gotPicture;
    AVPacket moviePacket;
    AVPacket decodePacket;
    unsigned char *audioBufferPtr;
    int size, outputSize;
    unsigned char *dataPtr;
    double timeStamp;

    // Can't do anything if we're not in a PLAYING mode
    if ((playMode != VS_MOVIE_PLAYING) && (playMode != VS_MOVIE_EOF))
        return;

    // Get the next video packet from the queue
    if (videoCodecContext != NULL)
    {
        if (dequeuePacket(videoQueue, &moviePacket))
        {
            // Compute the packet timestamp
            if (moviePacket.pts == AV_NOPTS_VALUE)
            {
                timeStamp = lastTimeStamp + lastFrameInterval;
            }
            else
            {
                timeStamp = moviePacket.pts * av_q2d(videoStream->time_base);
                lastFrameInterval = timeStamp - lastTimeStamp;
            }

            // Allocate a video frame and decode the video packet
            readSize = avcodec_decode_video2(videoCodecContext, videoFrame,
                &gotPicture, &moviePacket);

            // If the video codec gave us a full picture, output it now
            if ((readSize >= 0) && (gotPicture))
            {
                // Specify that we want the output in 3-bytes-per-pixel RGB
                // format
                sws_scale(scaleContext, videoFrame->data, videoFrame->linesize,
                    0, imageHeight, rgbFrame->data, rgbFrame->linesize);

                // Release the packet
                av_free_packet(&moviePacket);
            }

            // Remember the time stamp
            lastTimeStamp = timeStamp;
        }
        else if (playMode == VS_MOVIE_EOF)
        {
            // If we've hit the end of the file, and there are no more packets
            // queued, then we need to stop playing
            playMode = VS_MOVIE_STOPPED;
        }
    }

    // See if we need to decode more audio
    while ((audioCodecContext != NULL) && 
           (audioBufferSize < samplesPerFrame) &&
           (audioQueue->packetCount > 0))
    {
        // Get a packet from the audio queue
        if (dequeuePacket(audioQueue, &moviePacket))
        {
            // The audio AVPacket might contain multiple frames worth of
            // data (depending on the codec in use).  Set up a temporary
            // AVPacket to keep track of our decoding progress (only the
            // data and size fields are needed for the decode process)
            decodePacket.data = moviePacket.data;
            decodePacket.size = moviePacket.size;

            // Decode the packet data
            audioBufferPtr = (unsigned char *)&audioBuffer[audioBufferSize];
            while (decodePacket.size > 0)
            {
                // Lock the audio mutex
                pthread_mutex_lock(&audioMutex);

                // Calculate the available space in the audio buffer
                outputSize = sizeof(audioBuffer) - audioBufferSize;

                // Decode a chunk of the packet's data
                readSize = 
                    avcodec_decode_audio3(audioCodecContext, 
                        (short *)audioBufferPtr, &outputSize, &decodePacket);

                // If we hit an error, bail out of this frame
                if ((readSize < 0) || (outputSize < 0))
                {
                    decodePacket.size = 0;
                }
                else
                {
                    // Update the amount of data left to read as
                    // well as the input and output data pointers
                    decodePacket.data += readSize;
                    decodePacket.size -= readSize;
                    audioBufferPtr += outputSize;
                    audioBufferSize += outputSize;
                }

                // Unlock the audio mutex
                pthread_mutex_unlock(&audioMutex);
            }

            // Release the packet
            av_free_packet(&moviePacket);
        }
    }

    // If we've run out of audio data, we need to stop playing
    if ((playMode == VS_MOVIE_EOF) && (audioQueue->packetCount == 0) &&
        (audioBufferSize < samplesPerFrame))
    {
        playMode = VS_MOVIE_STOPPED;
    }
}

// ------------------------------------------------------------------------
// Private function
// Copies the data in the video decoder's internal buffer to the image data
// area specified for this reader object
// ------------------------------------------------------------------------
void vsMovieReader::copyFrame()
{
    // If we're not PLAYING, then there's nothing to copy
    if ((playMode != VS_MOVIE_PLAYING) && (playMode != VS_MOVIE_EOF))
        return;

    // Only copy the data if there is a place to copy it to
    if (outputBuffer)
    {
        // Copy the data, if it exists
        if (rgbFrame->data[0] != NULL)
            memcpy(outputBuffer, rgbFrame->data[0], getDataSize());
    }
}

// ------------------------------------------------------------------------
// Private function
// Adds an ffmpeg packet to the given packet queue
// ------------------------------------------------------------------------
void vsMovieReader::enqueuePacket(vsMoviePacketQueue *queue, AVPacket *packet)
{
    AVPacketList *packetQueueEntry;

    // Duplicate the packet to keep it from being invalidated by
    // the next av_read_frame() call
    av_dup_packet(packet);

    // Create a new packet queue entry
    packetQueueEntry = (AVPacketList *)malloc(sizeof(AVPacketList));
    packetQueueEntry->pkt = *packet;
    packetQueueEntry->next = NULL;

    // Lock the queue mutex
    pthread_mutex_lock(&queueMutex);

    // Add the packet to the video packet queue
    if (queue->tail == NULL)
        queue->head = packetQueueEntry;
    else
        queue->tail->next = packetQueueEntry;

    // Update the tail of the queue
    queue->tail = packetQueueEntry;

    // Update the packet count
    queue->packetCount++;

    // Unlock the queue mutex
    pthread_mutex_unlock(&queueMutex);
}

// ------------------------------------------------------------------------
// Private function
// Dequeues an ffmpeg packet from the given packet queue
// ------------------------------------------------------------------------
bool vsMovieReader::dequeuePacket(vsMoviePacketQueue *queue, AVPacket *packet)
{
    AVPacketList *packetQueueEntry;

    // Lock the queue mutex
    pthread_mutex_lock(&queueMutex);

    // Try to get the packet from the head of the queue
    packetQueueEntry = queue->head;
    if (packetQueueEntry != NULL)
    {
        // Update the queue
        queue->head = packetQueueEntry->next;
        if (queue->head == NULL)
            queue->tail = NULL;
        queue->packetCount--;

        // Extract the packet from the queue element to the packet structure
        // provided and free the element
        *packet = packetQueueEntry->pkt;
        free(packetQueueEntry);

        // Unlock the queue mutex
        pthread_mutex_unlock(&queueMutex);

        // Return true to indicate a successful dequeue
        return true;
    }
    else
    {
        // Unlock the queue mutex
        pthread_mutex_unlock(&queueMutex);

        // Return false to indicate an empty queue
        return false;
    }
}

// ------------------------------------------------------------------------
// Private function
// Flushes the given packet queue
// ------------------------------------------------------------------------
void vsMovieReader::flushQueue(vsMoviePacketQueue *queue)
{
    AVPacketList *packetQueueEntry;
    AVPacket packet;

    // Lock the queue mutex
    pthread_mutex_lock(&queueMutex);

    // Remove and free each packet in turn
    while ((queue->head != NULL) && (queue->packetCount > 0))
    {
        // Try to dequeue a packet, free its memory if successful
        packetQueueEntry = queue->head;

        if (packetQueueEntry != NULL)
        {
            // Update the queue
            queue->head = packetQueueEntry->next;
            if (queue->head == NULL)
                queue->tail = NULL;
            queue->packetCount--;

            // Extract the packet from the queue element, free the element 
            // and free the packet structure as well
            packet = packetQueueEntry->pkt;
            free(packetQueueEntry);
            av_free_packet(&packet);
        }
    }

    // Reset all queue pointers and counters
    queue->head = queue->tail = NULL;
    queue->packetCount = 0;

    // Unlock the queue mutex
    pthread_mutex_unlock(&queueMutex);
}

// ------------------------------------------------------------------------
// Private function
// Thread function to handle reading from the movie file and keeping the
// packet queues full
// ------------------------------------------------------------------------
void *vsMovieReader::fileThreadFunc(void *readerObject)
{
    vsMovieReader *instance;
    AVPacket moviePacket;
    int readStatus;
    bool needVideo, needAudio;
    
    // Initialize readStatus
    readStatus = 0;

    // Get the instance of the reader object from the parameter
    instance = (vsMovieReader *)readerObject;

    // Keep looping until we're signaled to quit by the main thread
    while (instance->playMode != VS_MOVIE_QUIT)
    {
        // If we're currently playing a file, make sure the queues are
        // full.  
        if (instance->playMode == VS_MOVIE_PLAYING)
        {
            // Initialize the read status to zero (no error)
            readStatus = 0;

            // Initialize the flags indicating whether or not we need
            // video and audio
            needVideo = (instance->videoCodecContext != NULL) && 
                (instance->videoQueue->packetCount < 
                    VS_MOVIE_PACKET_QUEUE_SIZE);
            needAudio = (instance->audioCodecContext != NULL) && 
                (instance->audioQueue->packetCount < 
                    VS_MOVIE_PACKET_QUEUE_SIZE);

            // Keep reading until we hit a read error, or we read enough
            // video and/or audio
            while ((readStatus >= 0) && ((needVideo) || (needAudio)))
            {
                // Acquire the file mutex
                pthread_mutex_lock(&instance->fileMutex);

                // Try to read a packet
                readStatus = av_read_frame(instance->movieFile, &moviePacket);
                if (readStatus >= 0)
                {
                    // If we got a valid packet, enqueue it on the appropriate
                    // queue
                    if ((moviePacket.stream_index == 
                            instance->videoStreamIndex) &&
                        (instance->videoCodecContext != NULL))
                    {
                        // Place the packet on the video queue
                        instance->enqueuePacket(instance->videoQueue, 
                            &moviePacket);
                    }
                    else if ((moviePacket.stream_index == 
                                 instance->audioStreamIndex) &&
                             (instance->audioCodecContext != NULL))
                    {
                        // Place the packet on the audio queue
                        instance->enqueuePacket(instance->audioQueue, 
                            &moviePacket);
                    }
                    else
                    {
                        // Throw the packet away
                        av_free_packet(&moviePacket);
                    }
                }

                // Update the video and audio flags
                needVideo = (instance->videoCodecContext != NULL) && 
                    (instance->videoQueue->packetCount < 
                        VS_MOVIE_PACKET_QUEUE_SIZE);
                needAudio = (instance->audioCodecContext != NULL) && 
                    (instance->audioQueue->packetCount < 
                        VS_MOVIE_PACKET_QUEUE_SIZE);

                // Release the file mutex
                pthread_mutex_unlock(&instance->fileMutex);
            }
        }

        // If we hit the end of the file, tell the main thread
        if ((readStatus < 0) && (instance->playMode == VS_MOVIE_PLAYING))
            instance->playMode = VS_MOVIE_EOF;

        // Sleep for a while to yield the processor to other threads
        usleep(10000);
    }
    
    return NULL;
}

// ------------------------------------------------------------------------
// Private function
// Thread function to handle decoding of audio packets 
// ------------------------------------------------------------------------
void *vsMovieReader::audioThreadFunc(void *readerObject)
{
    vsMovieReader *instance;

    // Get the instance of the reader object from the parameter
    instance = (vsMovieReader *)readerObject;

    // Keep looping until we're signaled to quit by the main thread
    while (instance->playMode != VS_MOVIE_QUIT)
    {
        // If we're currently playing audio, see if we need to queue a
        // new buffer on the audio stream
        if ((instance->playMode == VS_MOVIE_PLAYING) ||
            (instance->playMode == VS_MOVIE_EOF))
        {
            // Check if it's time to update the audio stream
            while ((instance->soundStream != NULL) && 
                   (instance->soundStream->isBufferReady()) && 
                   (instance->audioBufferSize >= instance->samplesPerFrame))
            {
                // Lock the audio mutex
                pthread_mutex_lock(&instance->audioMutex);

                // Copy the data from the local audio buffer to the sound 
                // stream
                instance->soundStream->queueBuffer(instance->audioBuffer);

                // Slide the data in the local buffer down and update the
                // buffer size
                memmove(instance->audioBuffer, 
                    &instance->audioBuffer[instance->samplesPerFrame], 
                    instance->audioBufferSize - instance->samplesPerFrame);
                instance->audioBufferSize -= instance->samplesPerFrame;

                // Unlock the audio mutex
                pthread_mutex_unlock(&instance->audioMutex);
            }
        }

        // Sleep for a while
        usleep(10000);
    }
    
    return NULL;
}
