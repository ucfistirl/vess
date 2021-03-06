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
//    VESS Module:  vsMovieWriter.c++
//
//    Description:  Class for writing a video file and outputting frames
//                  of image data.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMovieWriter.h++"
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
vsMovieWriter::vsMovieWriter(const char *format)
{
    // Initialize pointers.
    videoQueue = NULL;
    audioQueue = NULL;

    // Prepare ffmpeg for writing by acquiring all possible codecs.
    av_register_all();

    // Allocate the format context, which will store data.
    movieContext = avformat_alloc_context();
    if (movieContext == NULL)
    {
        fprintf(stderr, "vsMovieWriter::vsMovieWriter: Unable to allocate "
            "format context\n");
        return;
    }

    // Apply the format information to the output context.
    movieFormat = av_guess_format(format, NULL, NULL);
    if (movieFormat == NULL)
    {
        fprintf(stderr, "vsMovieWriter::vsMovieWriter: Could not guess a "
            "format based on extension type! (%s)\n", format);
        return;
    }

    // Store the format in the output context.
    movieContext->oformat = movieFormat;

    // Set up an encoder for the video data
    vCodec = avcodec_find_encoder(movieFormat->video_codec);
    if (vCodec != NULL)
    {
        // Create a new video stream using the above video codec
        vStream = avformat_new_stream(movieContext, vCodec);
        if (vStream == NULL)
        {
            fprintf(stderr, "vsMovieWriter::vsMovieWriter: Unable to "
                "create video stream!\n");
            return;
        }

        // Fetch the stream's index within the movie, as well as the
        // audio codec context
        vStreamIndex = vStream->index;
        vCodecContext = vStream->codec;

        // Fill in codec information with default values.
        vCodecContext->bit_rate = VS_MOVIE_WRITER_DEFAULT_BITRATE;
        vCodecContext->width = VS_MOVIE_WRITER_DEFAULT_WIDTH;
        vCodecContext->height = VS_MOVIE_WRITER_DEFAULT_HEIGHT;

        // The time base is expressed as a fraction and represents the time
        // for each frame. Only fixed-framerate writing should be attempted,
        // meaning frames will be spaced out in increments of 1 / framerate.
        vCodecContext->time_base.num = 1;
        vCodecContext->time_base.den = VS_MOVIE_WRITER_DEFAULT_FRAMERATE;

        // Set the GOP (MPEG group of pictures) size to 12.  This makes the
        // encoder emit a full frame (I frame) every 12 frames or so, with
        // the remaining frames being P or B frames (see the MPEG spec for a
        // full description of frame types)
        vCodecContext->gop_size = 12;

        // The vsVideoStream uses YUV420P as its base format.
        vCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

        // Don't put more than 2 B frames per group of pictures (this is an
        // image quality vs. compression ratio tradeoff)
        if (vCodecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
        {
            vCodecContext->max_b_frames = 2;
        }

        // FIXME: I'm also not sure why macroblock overflow is such a high risk.
        if (vCodecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO)
        {
            vCodecContext->mb_decision=2;
        }
    }
    else
    {
        printf("vsMovieWriter::vsMovieWriter:  Couldn't find a video codec "
             "suitable for the '%s' format!", format);
    }

    // Set up an encoder for the video data
    aCodec = avcodec_find_encoder(movieFormat->audio_codec);
    if (aCodec != NULL)
    {
        // Create a new audio stream using the above codec
        aStream = avformat_new_stream(movieContext, aCodec);
        if (aStream == NULL)
        {
            fprintf(stderr, "vsMovieWriter::vsMovieWriter: Unable to "
                "create audio stream!\n");
            return;
        }

        // Fetch the stream's index within the movie, as well as the
        // audio codec context
        aStreamIndex = aStream->index;;
        aCodecContext = aStream->codec;

        // Set the audio stream's ID to 1 (apparently, some codecs need this)
        aStream->id = 1;

        // FIXME: These should be able to be specified.
        aCodecContext->bit_rate = 96000;
        aCodecContext->sample_rate = 44100;
        aCodecContext->channels = 2;
    }
    else
    {
        printf("vsMovieWriter::vsMovieWriter:  Couldn't find an audio codec "
            "suitable for the '%s' format!", format);
    }

    // Initialize mutex and state variables to default values.
    pthread_mutex_init(&signalMutex, NULL);
    pthread_mutex_init(&writeMutex, NULL);
    ceaseSignal = false;
    writePaused = false;
    fileOpen = false;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMovieWriter::~vsMovieWriter()
{
    int i;

    // If a file is open, be sure to close it.
    if (fileOpen)
    {
        closeFile();
    }

    for (i = 0; i < movieContext->nb_streams; i++)
    {
        av_freep(&movieContext->streams[i]);
    }

    av_free(movieFormat);
    av_free(movieContext);

    if (videoQueue)
    {
        vsObject::unrefDelete(videoQueue);
    }

    if (audioQueue)
    {
        vsObject::unrefDelete(audioQueue);
    }

    // Destroy the mutex variables now that they are no longer needed.
    pthread_mutex_destroy(&signalMutex);
    pthread_mutex_destroy(&writeMutex);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsMovieWriter::getClassName()
{
    return "vsMovieWriter";
}

// ------------------------------------------------------------------------
// FIXME: Right now, this sets the one and only video queue... in the
// future should it dynamically add a new video stream for output?
// ------------------------------------------------------------------------
void vsMovieWriter::addVideoQueue(vsVideoQueue *queue)
{
    // Release the reference on the previously set queue.
    if (videoQueue)
    {
        videoQueue->yieldReference(videoReferenceID);
        vsObject::unrefDelete(videoQueue);
    }

    // Store and reference the video queue.
    videoQueue = queue;
    if (videoQueue)
    {
        // Reference and obtain a reference ID for the queue.
        videoQueue->ref();
        videoReferenceID = videoQueue->addReference();
    }
}

// ------------------------------------------------------------------------
// FIXME: Right now, this sets the one and only audio queue... in the
// future should it dynamically add a new audio stream for output?
// ------------------------------------------------------------------------
void vsMovieWriter::addAudioQueue(vsMultiQueue *queue)
{
    // Release the reference on the previously set queue.
    if (audioQueue)
    {
        audioQueue->yieldReference(audioReferenceID);
        vsObject::unrefDelete(audioQueue);
    }

    // Store and reference the audio queue.
    audioQueue = queue;
    if (audioQueue)
    {
        // Reference and obtain a reference ID for the queue.
        audioQueue->ref();
        audioReferenceID = audioQueue->addReference();
    }
}

// ------------------------------------------------------------------------
// Sets the size of frames rendered by the vsMovieWriter based on the
// provided enumerated type.
// ------------------------------------------------------------------------
void vsMovieWriter::setFrameSize(vsVideoFrameSize size)
{
    // Make sure a file isn't already open.
    if (fileOpen)
    {
        fprintf(stderr, "vsMovieWriter::setFrameSize: Frame size may not be "
            "set while a file is open.\n");
        return;
    }

    // Store the new frame size in the video context.
    switch (size)
    {
        case VS_VIDEO_SIZE_320X180:
        {
            vCodecContext->width = 320;
            vCodecContext->height = 180;
        }
        break;

        case VS_VIDEO_SIZE_320X240:
        {
            vCodecContext->width = 320;
            vCodecContext->height = 240;
        }
        break;

        case VS_VIDEO_SIZE_400X300:
        {
            vCodecContext->width = 400;
            vCodecContext->height = 300;
        }
        break;

        case VS_VIDEO_SIZE_640X360:
        {
            vCodecContext->width = 640;
            vCodecContext->height = 360;
        }
        break;

        case VS_VIDEO_SIZE_640X480:
        {
            vCodecContext->width = 640;
            vCodecContext->height = 480;
        }
        break;

        case VS_VIDEO_SIZE_800X600:
        {
            vCodecContext->width = 800;
            vCodecContext->height = 600;
        }
        break;

        case VS_VIDEO_SIZE_1024X576:
        {
            vCodecContext->width = 1024;
            vCodecContext->height = 576;
        }
        break;

        case VS_VIDEO_SIZE_1024X768:
        {
            vCodecContext->width = 1024;
            vCodecContext->height = 768;
        }
        break;

        case VS_VIDEO_SIZE_1280X960:
        {
            vCodecContext->width = 1280;
            vCodecContext->height = 960;
        }
        break;

        case VS_VIDEO_SIZE_1600X1200:
        {
            vCodecContext->width = 1600;
            vCodecContext->height = 1200;
        }
        break;

        default:
        {
            vCodecContext->width = VS_MOVIE_WRITER_DEFAULT_WIDTH;
            vCodecContext->height = VS_MOVIE_WRITER_DEFAULT_HEIGHT;
        }
        break;
    }
}

// ------------------------------------------------------------------------
// Returns the width of frames rendered by this vsMovieWriter.
// ------------------------------------------------------------------------
int vsMovieWriter::getWidth()
{
    return vCodecContext->width;
}

// ------------------------------------------------------------------------
// Returns the height of frames rendered by this vsMovieWriter.
// ------------------------------------------------------------------------
int vsMovieWriter::getHeight()
{
    return vCodecContext->height;
}

// ------------------------------------------------------------------------
// This method may be used to set the timing mode for the video.
// ------------------------------------------------------------------------
void vsMovieWriter::setTimingMode(vsVideoTimingMode mode)
{
    // Make sure a file isn't already open.
    if (fileOpen)
    {
        fprintf(stderr, "vsMovieWriter::setTimingMode: Timing mode may not be "
            "set while a file is open.\n");
        return;
    }

    // Update the timing mode.
    videoTimingMode = mode;
}

// ------------------------------------------------------------------------
// This method returns the timing mode employed for video frames.
// ------------------------------------------------------------------------
vsVideoTimingMode vsMovieWriter::getTimingMode()
{
    return videoTimingMode;
}

// ------------------------------------------------------------------------
// This method uses the context and format settings to open a movie file.
// ------------------------------------------------------------------------
bool vsMovieWriter::openFile(char *filename)
{
    // Make sure a file isn't already open.
    if (fileOpen)
    {
        fprintf(stderr, "vsMovieWriter::openFile: A file is already open... "
            "close it first.\n");
        return false;
    }

    // Copy the filename
    strncpy(movieContext->filename, filename, sizeof(movieContext->filename));

    // Make sure we have a video codec available
    if (vCodec == NULL)
    {
        fprintf(stderr, "vsMovieWriter::openFile: No video codec available!\n");
        return false;
    }

    // Attempt to open the codec.
    if (avcodec_open2(vCodecContext, vCodec, NULL) < 0)
    {
        fprintf(stderr, "vsMovieWriter::openFile: Unable to load codec!\n");
        return false;
    }

    // Determine whether a separate buffer is needed. In the unique case that
    // a RAW format is used, no distinct output buffer is necessary, as the
    // unencoded data may be written unmodified. In all other cases this buffer
    // holds the encoded data until it is written to the file.
    if (!(movieContext->oformat->flags & AVFMT_RAWPICTURE))
    {
        // FIXME: Set this size dynamically, fool.
        vOutputBufferSize = 400000;
        vOutputBuffer = (uint8_t *)malloc(vOutputBufferSize);
    }
    else
    {
        vOutputBufferSize = 0;
        vOutputBuffer = NULL;
    }

    // Allocate a frame for the RGB24-format data. This frame will hold the
    // data from the video queue in RGB24 format.
    rgbFrame = allocFrame(AV_PIX_FMT_RGB24, videoQueue->getWidth(),
        videoQueue->getHeight());
    if (rgbFrame == NULL)
    {
        // Free the video output buffer if exists.
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }

        // Print an error message and return.
        fprintf(stderr, "vsMovieWriter::openFile: "
            "Unable to allocate RGB image frame!\n");
        return false;
    }

    // A frame is also required for the final converted image (conversion not
    // to be confused with encoding).  Attempt to allocate the video frame.
    videoFrame = allocFrame(vCodecContext->pix_fmt, videoQueue->getWidth(),
        videoQueue->getHeight());
    if (videoFrame == NULL)
    {
        // Free the video output buffer if exists.
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }

        // Free the rgbFrame
        free(rgbFrame->data[0]);
        av_free(rgbFrame);

        // Print an error message and return.
        fprintf(stderr, "vsMovieWriter::openFile: "
            "Unable to allocate output image frame!\n");
        return false;
    }

    // Create a software scale context that will handle the image format
    // conversion and resampling of the input image to the video frame
    scaleContext = sws_getCachedContext(scaleContext,
        vCodecContext->width, vCodecContext->height, vCodecContext->pix_fmt,
        videoQueue->getWidth(), videoQueue->getHeight(), AV_PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);
    if (scaleContext == NULL)
    {
        // Free the video output buffer if exists.
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }

        // Now free the rgb frame and its data buffer.
        free(rgbFrame->data[0]);
        av_free(rgbFrame);

        // Now free the video frame.
        if (videoFrame)
        {
            free(videoFrame->data[0]);
            av_free(videoFrame);
        }

        // Print an error message and return.
        fprintf(stderr, "vsMovieWriter::openFile: "
            "Unable to initialize scale context!\n");
        return false;
    }

    // Make sure we have an audio codec available
    if (aCodec == NULL)
    {
        // Free the video output buffer if exists.
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }

        // Now free the rgb frame and its data buffer.
        free(rgbFrame->data[0]);
        av_free(rgbFrame);

        if (videoFrame)
        {
            free(videoFrame->data[0]);
            av_free(videoFrame);
        }

        // Free the scale context if it exists.
        if (scaleContext)
        {
            sws_freeContext(scaleContext);
        }

        fprintf(stderr, "vsMovieWriter::openFile: Unable to find audio "
            "codec!\n");
        return false;
    }

    // Attempt to open the codec.
    if (avcodec_open2(aCodecContext, aCodec, NULL) < 0)
    {
        // Free the video output buffer if exists.
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }

        // Now free the rgb frame and its data buffer.
        free(rgbFrame->data[0]);
        av_free(rgbFrame);

        if (videoFrame)
        {
            free(videoFrame->data[0]);
            av_free(videoFrame);
        }

        fprintf(stderr, "vsMovieWriter::openFile: Unable to load audio "
            "codec!\n");
        return false;
    }

    // Create an AVFrame to buffer audio for encoding
    audioFrame = av_frame_alloc();

    // Create the output buffer, into which encoded data will be written before
    // it goes out to the file.
    aOutputBufferSize = 200000;
    aOutputBuffer = (uint8_t *)malloc(aOutputBufferSize);

    // See if the chosen audio codec is capable of variable audio
    // frame sizes
    if (aCodecContext->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
    {
        // Pick a sane value for the frame size (this one was taken from
        // the ffmpeg example code)
        rawSampleSize = 10000;
    }
    else
    {
        // Use the frame size that the codec has set
        rawSampleSize = aCodecContext->frame_size;
    }

    // Allocate the buffer to hold simples that are read from the audio queue
    // before they encoded.
    rawSamples =
        (uint8_t *)malloc(rawSampleSize * 2 * aCodecContext->channels);
    nullSamples =
        (uint8_t *)calloc(rawSampleSize * 2 * aCodecContext->channels, 1);

    // Dump the format information out to the file.
    av_dump_format(movieContext, 0, filename, 1);

    // Attempt to open an output file in the case that an output file is used
    // for this format.
    if (!(movieFormat->flags & AVFMT_NOFILE))
    {
        if (avio_open(&movieContext->pb, filename, AVIO_FLAG_WRITE) < 0)
        {
            // Free the video output buffer if exists.
            if (vOutputBuffer)
            {
                free(vOutputBuffer);
            }

            // Now free the rgb frame and its data buffer.
            free(rgbFrame->data[0]);
            av_free(rgbFrame);

            // Free the video frame and its data buffer.
            if (videoFrame)
            {
                free(videoFrame->data[0]);
                av_free(videoFrame);
            }

            // Free the scale context if it exists.
            if (scaleContext)
            {
                sws_freeContext(scaleContext);
            }

            // Free the audio output buffer in all cases.
            free(aOutputBuffer);

            // Free the raw and null sample buffer memory.
            free(rawSamples);
            free(nullSamples);

            fprintf(stderr, "vsMovieWriter::openFile: "
                "Unable to open output file %s!\n", filename);
            return false;
        }
    }

    // We have officially opened the file.
    fileOpen = true;

    // Write the header for the stream.
    avformat_write_header(movieContext, NULL);

    // Lock the pause mutex and set the pause state to true to prevent the
    // write thread from proceeding immediately.
    pthread_mutex_lock(&writeMutex);
    writePaused = true;

    // Begin with time elapsed for neither video nor audio.
    videoElapsed = 0.0;
    audioElapsed = 0.0;

    // Finally, create the thread using the appropriate write loop for the
    // timing mode desired.
    if (videoTimingMode == VS_MW_TIMING_FIXED)
        pthread_create(&writeThread, NULL, writeLoopFixed, this);
    else
        pthread_create(&writeThread, NULL, writeLoopReal, this);

    // Return true, indicating the file was opened successfully.
    return true;
}

// ------------------------------------------------------------------------
// This method uses the context and format settings to open a movie file.
// ------------------------------------------------------------------------
void vsMovieWriter::closeFile()
{
    if (!fileOpen)
    {
        fprintf(stderr, "vsMovieWriter::closeFile: No file open!\n");
        return;
    }

    // Signal the thread to close.
    pthread_mutex_lock(&signalMutex);
    ceaseSignal = true;

    // Allow the thread to move forward.
    pthread_mutex_unlock(&signalMutex);

    // Unpause the write thread if it had previously been paused.
    if (writePaused)
    {
        // Release the mutex that had prevented the thread from moving forward.
        pthread_mutex_unlock(&writeMutex);
        writePaused = false;
    }

    // Wait until the thread officially closes.
    pthread_join(writeThread, NULL);

    // Write the trailer.
    av_write_trailer(movieContext);

    // Close the movie file.
    if (vStream)
    {
        // Close the codec associated with this context. Note that this leaves
        // the context intact.
        avcodec_close(vStream->codec);

        // Technically there should always be an rgbFrame, but in case the
        // input stream type changes from rgb format to something else this
        // check may be necessary.
        if (rgbFrame)
        {
            // Now free the rgb frame and its data buffer.
            free(rgbFrame->data[0]);
            av_free(rgbFrame);
        }

        // Do the same for the video frame
        if (videoFrame)
        {
            free(videoFrame->data[0]);
            av_free(videoFrame);
        }

        // Free the scale context
        if (scaleContext)
        {
            sws_freeContext(scaleContext);
        }

        // Free the output buffer
        if (vOutputBuffer)
        {
            free(vOutputBuffer);
        }
    }

    // Clean up the audio stream data
    if (aStream)
    {
        avcodec_close(aStream->codec);
        free(rawSamples);
        free(nullSamples);
        free(aOutputBuffer);
    }

    // Officially close the output file.
    if (!(movieFormat->flags & AVFMT_NOFILE))
        avio_close(movieContext->pb);

    // Mark that the file has been closed successfully.
    fileOpen = false;
}

// ------------------------------------------------------------------------
// Returns whether the movie writer is currently in a paused state.
// ------------------------------------------------------------------------
bool vsMovieWriter::isPaused()
{
    return writePaused;
}

// ------------------------------------------------------------------------
// This method unpauses the movie writer if it had been paused, clearing
// out any data that may have accumulated during the pause state.
// ------------------------------------------------------------------------
void vsMovieWriter::startResume()
{
    // Unpause the write thread if it had previously been paused.
    if (writePaused)
    {
        // Clear the streams out so that data accumulated during the pause
        // isn't written in.
        if (videoQueue)
        {
            // Dequeue all of the images. The clear() method cannot be used
            // here because it is important to keep track of the most recent
            // video timestamp.
            while (videoQueue->dequeue(NULL, &curVideoTimestamp,
                videoReferenceID));
        }

        // Clear the streams out so that data accumulated during the pause
        // isn't written in.
        if (audioQueue)
        {
            // Clear all of the sound data.
            audioQueue->clear(audioReferenceID);
        }

        // Release the mutex that had prevented the thread from moving forward.
        pthread_mutex_unlock(&writeMutex);
        writePaused = false;
    }
}

// ------------------------------------------------------------------------
// Pauses the writer if it is not paused already, preventing frames from
// being written while the pause state persists.
// ------------------------------------------------------------------------
void vsMovieWriter::pause()
{
    // Unpause the write thread if it had previously been paused.
    if (!writePaused)
    {
        // Lock the mutex to prevent the thread from moving forward.
        pthread_mutex_lock(&writeMutex);
        writePaused = true;
    }
}

// ------------------------------------------------------------------------
// This value returns the current duration of video that has been rendered
// since the file has been opened.
// ------------------------------------------------------------------------
double vsMovieWriter::getTimeElapsed()
{
    double timeElapsed;

    // If writing has been paused, it is safe to use the current video time,
    // since it will not be changing.
    if (writePaused)
    {
        timeElapsed = videoElapsed;
    }
    else
    {
        // Lock the thread in place, pausing it momentarily.
        pthread_mutex_lock(&writeMutex);

        // Store the return value.
        timeElapsed = videoElapsed;

        // Release the mutex that had prevented the thread from moving forward.
        pthread_mutex_unlock(&writeMutex);
    }

    // Return the value.
    return timeElapsed;
}

// ------------------------------------------------------------------------
// Private VESS Function
// This method allocates an AVFrame of the desired format and dimensions,
// or NULL if such a frame cannot be allocated. Note that the data buffer
// for the frame is allocated and set as well.
// ------------------------------------------------------------------------
AVFrame *vsMovieWriter::allocFrame(AVPixelFormat format, int width, int height)
{
    AVFrame *frame;
    uint8_t *imageBuffer;
    int imageBufferSize;

    // Attempt to allocate the frame.
    frame = av_frame_alloc();
    if (frame == NULL)
    {
        return NULL;
    }

    // Determine the size of an image of this format and these dimensions.
    imageBufferSize = avpicture_get_size(format, width, height);
    imageBuffer = (uint8_t *)malloc(imageBufferSize);
    if (imageBuffer == NULL)
    {
        // Free the allocated frame before returning NULL.
        av_free(frame);
        return NULL;
    }

    // Assign the allocated image buffer to the frame and return it.
    avpicture_fill((AVPicture *)frame, imageBuffer, format, width, height);

    return frame;
}

// ------------------------------------------------------------------------
// Static VESS Internal Function
// This is the write thread loop that draws each and every frame in the
// vsVideoQueue, ignoring timestamps. It will always be used in raw format
// mode, and may be used for other purposes like the writing of constant-
// framerate simulations.
// ------------------------------------------------------------------------
void *vsMovieWriter::writeLoopFixed(void *userData)
{
    vsMovieWriter *writer;

    // Store the pointer to the movie writer object.
    writer = (vsMovieWriter *)userData;

    // Sieze the semaphore used to signal that the function should cease.
    pthread_mutex_lock(&writer->signalMutex);

    // Read from the device until signalled otherwise.
    while (!writer->ceaseSignal)
    {
        // Release the signal semaphore for now.
        pthread_mutex_unlock(&writer->signalMutex);

        // Attempt to lock the pause mutex. If the main application has locked
        // this mutex, then this loop will be stalled until it is unpaused and
        // the video and audio streams will be allowed to cycle.
        pthread_mutex_lock(&writer->writeMutex);

        // See which of the video or audio streams is currently more advanced.
        // If the video timestamp is ahead of the audio timestamp, attempt to
        // write audio samples to catch up.
        if (writer->videoElapsed > writer->audioElapsed)
        {
            if (writer->audioQueue->dequeue((void *)writer->rawSamples,
                writer->rawSampleSize * writer->aCodecContext->channels * 2,
                writer->audioReferenceID))
            {
                // Write some audio data.
                writer->writeSamples(writer->rawSamples);

                // Update the amount of audio data elapsed based on the time
                // the audio stream expects the next data to occur.
                writer->audioElapsed +=
                    (double)writer->rawSampleSize / 22050.0;
            }
/*
            // FIXME: Define audio starvation behavior here.
            else
            {
                // Write some null audio data.
                writer->writeSamples(writer->nullSamples);
            }
*/
        }
        else
        {
            // Either the audio stream has caught up to the video stream or the
            // audio stream is starved. In either case, process the next video
            // step if it exists.

            // Check the timestamp on the next frame.
            if (writer->videoQueue->dequeue((char *)writer->rgbFrame->data[0],
                NULL, writer->videoReferenceID))
            {
                // Draw the next frame of video data.
                writer->writeFrame();

                // Update the timestamp, using the time for the next expected
                // frame from the video stream.
                writer->videoElapsed = (double)writer->vStream->pts.val *
                    (writer->vStream->time_base.num /
                    writer->vStream->time_base.den);
            }
        }

        // Unlock the pause mutex now that the writing has been managed.
        pthread_mutex_unlock(&writer->writeMutex);

        // Take control of the signal semaphore again for the loop check.
        pthread_mutex_lock(&writer->signalMutex);
    }

    // Release the signal semaphore, as its signal has been passed.
    pthread_mutex_unlock(&writer->signalMutex);

    // Exit the thread so the stream can be closed.
    pthread_exit(NULL);

    // Return NULL to make the Windows compiler happy
    return NULL;
}

// ------------------------------------------------------------------------
// Static VESS Internal Function
// This is the write thread loop that relies on timestamps for enqueued
// video data to generate appropriate spacing between frames. Note that it
// is invalid for any frame to have an invalid timestamp within the
// vsVideoQueue when this timing mode is employed.
// ------------------------------------------------------------------------
void *vsMovieWriter::writeLoopReal(void *userData)
{
    vsMovieWriter *writer;
    bool videoBufferFull;
    double videoTimeBase;
    double videoPhase;
    double nextVideoTimestamp;

    // Store the pointer to the movie writer object.
    writer = (vsMovieWriter *)userData;

    // Calculate the video time base from the stream. This is the amount of
    // time that each frame is on the screen.
    videoTimeBase = (double)(writer->vStream->time_base.num) /
        (double)(writer->vStream->time_base.den);
    videoBufferFull = false;

    // Sieze the semaphore used to signal that the function should cease.
    pthread_mutex_lock(&writer->signalMutex);

    // Read from the device until signalled otherwise.
    while (!writer->ceaseSignal)
    {
        // Release the signal semaphore for now.
        pthread_mutex_unlock(&writer->signalMutex);

        // Attempt to lock the pause mutex. If the main application has locked
        // this mutex, then this loop will be stalled until it is unpaused and
        // the video and audio streams will be allowed to cycle.
        pthread_mutex_lock(&writer->writeMutex);

        // See which of the video or audio streams is currently more advanced.
        // If the video timestamp is ahead of the audio timestamp, attempt to
        // write audio samples to catch up.
        if (writer->videoElapsed > writer->audioElapsed)
        {
            if (writer->audioQueue->dequeue((void *)writer->rawSamples,
                writer->rawSampleSize * writer->aCodecContext->channels * 2,
                writer->audioReferenceID))
            {
                // Write some audio data.
                writer->writeSamples(writer->rawSamples);

                // Update the amount of audio data elapsed based on the time
                // the audio stream expects the next data to occur.
                writer->audioElapsed +=
                    (double)writer->rawSampleSize / 22050.0;
            }
/*
            // FIXME: Define audio starvation behavior here.
            else
            {
                // Write some null audio data.
                writer->writeSamples(writer->nullSamples);
            }
*/
        }
        else
        {
            // Either the audio stream has caught up to the video stream or the
            // audio stream is starved. In either case, process the next video
            // step if it exists.

            // See if the video buffer has not yet been filled, in which case
            // the first frame of data discovered needs to be enqueued.
            if (!videoBufferFull)
            {
                // Get the data.
                if (writer->videoQueue->dequeue(
                    (char *)writer->rgbFrame->data[0],
                    &(writer->curVideoTimestamp), writer->videoReferenceID))
                {
                    // Write out this first frame of data.
                    writer->writeFrame();

                    // Update the timestamp, using the time for the next
                    // expected frame from the video stream.
                    writer->videoElapsed += videoTimeBase;

                    // The buffer has an image to use for subsequent frames, so
                    // mark that the buffer is full. The phase is arbitrarily
                    // initialized to 0.0 so that future frames' spacing will
                    // be based on the timing of this first one.
                    videoBufferFull = true;
                    videoPhase = 0.0;
                }
            }
            else
            {
                // See if the current data should be drawn. Note that the data
                // may be drawn for several consecutive frames if the phase
                // justifies it.
                if (videoPhase > videoTimeBase)
                {
                    // Write the frame and update the phase to accommodate.
                    writer->writeFrame();
                    videoPhase -= videoTimeBase;

                    // Update the timestamp, using the time for the next
                    // expected frame from the video stream.
                    writer->videoElapsed += videoTimeBase;
                }
                else
                {
                    // Grab as much data as possible until the phase exceeds
                    // the time base.
                    if (writer->videoQueue->peek(NULL, &nextVideoTimestamp,
                        writer->videoReferenceID))
                    {
                        if ((nextVideoTimestamp - writer->curVideoTimestamp) +
                            videoPhase > videoTimeBase)
                        {
                            // Draw the current frame data, as the next image
                            // is timestamped for the following frame.
                            writer->writeFrame();

                            // Update the timestamp, using the time for the
                            // next expected frame from the video stream.
                            writer->videoElapsed += videoTimeBase;

                            // Subtract videoTimeBase worth of time out of the
                            // phase to accommodate for this new frame,
                            // temporarily making the phase negative. This will
                            // be corrected when the difference in timestamps
                            // is indiscriminantly added back into the phase.
                            videoPhase -= videoTimeBase;
                        }

                        // Read the new data into the frame.
                        writer->videoQueue->dequeue(
                            (char *)writer->rgbFrame->data[0], NULL,
                            writer->videoReferenceID);

                        // Update the phase and time.
                        videoPhase +=
                            (nextVideoTimestamp - writer->curVideoTimestamp);
                        writer->curVideoTimestamp = nextVideoTimestamp;
                    }
                }
            }
        }

        // Unlock the pause mutex now that the writing has been managed.
        pthread_mutex_unlock(&writer->writeMutex);

        // Take control of the signal semaphore again for the loop check.
        pthread_mutex_lock(&writer->signalMutex);
    }

    // Release the signal semaphore, as its signal has been passed.
    pthread_mutex_unlock(&writer->signalMutex);

    // Exit the thread so the stream can be closed.
    pthread_exit(NULL);

    // Return NULL to make the Windows compiler happy
    return NULL;
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This is the method that actually draws a frame out to the file.
// ------------------------------------------------------------------------
void vsMovieWriter::writeFrame()
{
    AVPacket videoPacket;
    int videoPacketSize;
    int gotPacket;

    // Convert and resample the data in the output frame.
    sws_scale(scaleContext, videoFrame->data, videoFrame->linesize,
        0, vCodecContext->height, rgbFrame->data, rgbFrame->linesize);

    // The write procedure is dependent upon the picture format.
    if (movieContext->oformat->flags & AVFMT_RAWPICTURE)
    {
        // Initialize the packet.
        av_init_packet(&videoPacket);
 
        // In this mode, all frames are key frames, the index of
        // which is simply fetched from the stream.
        videoPacket.flags |= AV_PKT_FLAG_KEY;
        videoPacket.stream_index = vStream->index;

        // The size and data are taken straight from the frame.
        videoPacket.size = sizeof(AVPicture);
        videoPacket.data = (uint8_t *)videoFrame;
 
        // Write the next video frame.
        av_interleaved_write_frame(movieContext, &videoPacket);
    }
    else
    {
        // Create a packet for the output buffer
        av_init_packet(&videoPacket);
        videoPacket.size = 0;
        videoPacket.data = NULL;

        // Attempt to encode the image, storing the final size.
        videoPacketSize =
           avcodec_encode_video2(vCodecContext, &videoPacket, videoFrame,
              &gotPacket);

        // The size will be zero if the image was buffered.
        if ((videoPacketSize > 0) && (gotPacket))
        {
            // Use the timestamp from the codec context.
            videoPacket.pts = av_rescale_q(vCodecContext->coded_frame->pts,
                vCodecContext->time_base, vStream->time_base);
 
            // Determine if this was a key frame.
            if (vCodecContext->coded_frame->key_frame)
                videoPacket.flags |= AV_PKT_FLAG_KEY;
            videoPacket.stream_index = vStream->index;
 
            // Finally, write out the frame itself.
            av_interleaved_write_frame(movieContext, &videoPacket);

            // Clean up
            av_packet_unref(&videoPacket);
        }
    }
}

// ------------------------------------------------------------------------
// VESS Internal Function
// This is the method that actually writes audio data out to the file.
// ------------------------------------------------------------------------
void vsMovieWriter::writeSamples(void *samples)
{
    AVPacket audioPacket;
    int channels, bytesPerSample, bytesPerFrame;
    int gotPacket;
    int errorCode;

    // Initialize the packet.
    av_init_packet(&audioPacket);
    audioPacket.data = NULL;
    audioPacket.size = 0;

    // Fetch some audio parameters
    channels = aCodecContext->channels;
    bytesPerSample = av_get_bytes_per_sample(aCodecContext->sample_fmt);
    bytesPerFrame = channels * bytesPerSample;

    // Fill the audio frame with sample data
    audioFrame->nb_samples = rawSampleSize;
    avcodec_fill_audio_frame(audioFrame, channels, aCodecContext->sample_fmt,
        (uint8_t *) samples, bytesPerFrame, 1);
 
    // Attempt to encode the samples, storing the final size.
    errorCode =
        avcodec_encode_audio2(aCodecContext, &audioPacket, audioFrame, 
            &gotPacket);

    // See if we encoded successfully
    if ((errorCode >= 0) && (gotPacket))
    {
        // Configure the packet using the new data.
        audioPacket.stream_index = aStream->index;

        // Write the compressed samples out to the movie file.
        av_interleaved_write_frame(movieContext, &audioPacket);
    }
}

