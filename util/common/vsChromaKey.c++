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
//    VESS Module:  vsChromaKey.h++
//
//    Description:  Chromatic key color based image transparency and
//                  substitution class
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdlib.h>
#include "vsChromaKey.h++"

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
vsChromaKey::vsChromaKey()
{
    // Default key color is pure blue
    keyRed = 0;
    keyGreen = 0;
    keyBlue = 255;

    // Default equation is SUM
    keyEquation = VS_CHROMAKEY_DIFF_SUM;

    // Default threshold is 10
    keyThreshold = 10;

    // Default word size is 1 (no padding)
    wordSize = 1;
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
vsChromaKey::~vsChromaKey()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsChromaKey::getClassName()
{
    return "vsChromaKey";
}

//------------------------------------------------------------------------
// Set the key color of the object. This is the color that each pixel of
// every input image is checked against to determine whether that pixel
// should be made transparent.
//------------------------------------------------------------------------
void vsChromaKey::setKeyColor(unsigned char red, unsigned char green,
    unsigned char blue)
{
    keyRed = red;
    keyGreen = green;
    keyBlue = blue;
}

//------------------------------------------------------------------------
// Get the key color of the object. This is the color that each pixel of
// every input image is checked against to determine whether that pixel
// should be made transparent.
//------------------------------------------------------------------------
void vsChromaKey::getKeyColor(unsigned char *red, unsigned char *green,
    unsigned char *blue)
{
    if (red)
        *red = keyRed;
    if (green)
        *green = keyGreen;
    if (blue)
        *blue = keyBlue;
}

//------------------------------------------------------------------------
// Set the 'word size' of the image data. Input image rows are assumed to
// be multiples of this size (assumes padding at the end of each row if
// the data does not reach the end of a multiple), and output rows are
// padded to this size if their data does not reach the end of a multiple.
//------------------------------------------------------------------------
void vsChromaKey::setWordSize(int size)
{
    if (size > 0)
        wordSize = size;
}

//------------------------------------------------------------------------
// Get the 'word size' of the image data. Input image rows are assumed to
// be multiples of this size (assumes padding at the end of each row if
// the data does not reach the end of a multiple), and output rows are
// padded to this size if their data does not reach the end of a multiple.
//------------------------------------------------------------------------
int vsChromaKey::getWordSize()
{
    return wordSize;
}

//------------------------------------------------------------------------
// Set/get the color matching equation type and threshold. The equation
// type specifies what sort of computation the object performs to
// determine the 'distance' each pixel is from the key color. The
// threshold value specifies the color 'distance' within which pixels are
// considered to be equavilent to the key color.
//------------------------------------------------------------------------
void vsChromaKey::setEquationType(vsChromaKeyEquationType equationType,
    int threshold)
{
    keyEquation = equationType;
    keyThreshold = threshold;
}

//------------------------------------------------------------------------
// Set/get the color matching equation type and threshold. The equation
// type specifies what sort of computation the object performs to
// determine the 'distance' each pixel is from the key color. The
// threshold value specifies the color 'distance' within which pixels are
// considered to be equavilent to the key color.
//------------------------------------------------------------------------
void vsChromaKey::getEquationType(vsChromaKeyEquationType *equationType,
    int *threshold)
{
    if (equationType)
        *equationType = keyEquation;
    if (threshold)
        *threshold = keyThreshold;
}

//------------------------------------------------------------------------
// Takes an RGB image (three bytes per pixel), and creates an RGBA image,
// where the value of the alpha channel is based on the proximity of each
// pixel's color to this object's key color.
//------------------------------------------------------------------------
void vsChromaKey::createAlphaFromColor(unsigned char *inputImage,
    int imageWidth, int imageHeight, unsigned char *outputImage)
{
    int row, column;
    unsigned char red, green, blue, alpha;
    unsigned char *inPtr, *outPtr;
    int inRowExtra, outRowExtra;
    int colorDiff;

    // Set the running pointers to the start of the image buffers
    inPtr = inputImage;
    outPtr = outputImage;

    // Calculate the amount that the row lengths exceed the word size
    inRowExtra = (imageWidth * 3) % wordSize;
    outRowExtra = (imageWidth * 4) % wordSize;

    // Loop through all the rows of the image
    for (row = 0; row < imageHeight; row++)
    {
        // Loop through every pixel of the row
        for (column = 0; column < imageWidth; column++)
        {
            // Grab the pixel data
            red = *(inPtr++);
            green = *(inPtr++);
            blue = *(inPtr++);

            // Determine the 'difference' between the pixel color and this
            // object's key color
            colorDiff = calcDifference(red, green, blue);

            // If the difference is below the threshold, then make the pixel
            // transparent. Otherwise, it should be opaque.
            alpha = ((colorDiff <= keyThreshold) ? 0 : 255);

            // Write the pixel data to the output buffer
            *(outPtr++) = red;
            *(outPtr++) = green;
            *(outPtr++) = blue;
            *(outPtr++) = alpha;
        }

        // Pad the image buffers, if needed
        if (inRowExtra != 0)
            inPtr += (wordSize - inRowExtra);
        if (outRowExtra != 0)
            outPtr += (wordSize - outRowExtra);
    }
}

//------------------------------------------------------------------------
// Takes three separate grayscale images, each one representing a single
// channel of an RGB image, and constructs a fourth grayscale image
// representing the alpha channel of that image. The alpha values are
// based on the proximity of each pixel's color to this object's key
// color.
//------------------------------------------------------------------------
void vsChromaKey::createAlphaFromColor(unsigned char *redChannel,
    unsigned char *greenChannel, unsigned char *blueChannel, int imageWidth,
    int imageHeight, unsigned char *outputAlphaChannel)
{
    int row, column;
    unsigned char red, green, blue, alpha;
    unsigned char *redPtr, *greenPtr, *bluePtr, *outPtr;
    int inRowExtra, outRowExtra;
    int colorDiff;

    // Set the running pointers to the start of the image buffers
    redPtr = redChannel;
    greenPtr = greenChannel;
    bluePtr = blueChannel;
    outPtr = outputAlphaChannel;

    // Calculate the amount that the row lengths exceed the word size
    inRowExtra = (imageWidth * 1) % wordSize;
    outRowExtra = (imageWidth * 1) % wordSize;

    // Loop through all the rows of the image
    for (row = 0; row < imageHeight; row++)
    {
        // Loop through every pixel of the row
        for (column = 0; column < imageWidth; column++)
        {
            // Grab the pixel data
            red = *(redPtr++);
            green = *(greenPtr++);
            blue = *(bluePtr++);

            // Determine the 'difference' between the pixel color and this
            // object's key color
            colorDiff = calcDifference(red, green, blue);

            // If the difference is below the threshold, then make the pixel
            // transparent. Otherwise, it should be opaque.
            alpha = ((colorDiff <= keyThreshold) ? 0 : 255);

            // Write the pixel data to the output buffer
            *(outPtr++) = alpha;
        }

        // Pad the image buffers, if needed
        if (inRowExtra != 0)
        {
            redPtr += (wordSize - inRowExtra);
            greenPtr += (wordSize - inRowExtra);
            bluePtr += (wordSize - inRowExtra);
        }
        if (outRowExtra != 0)
            outPtr += (wordSize - outRowExtra);
    }
}

//------------------------------------------------------------------------
// Takes an RGBA image (four bytes per pixel), and modifies the alpha
// values such that the pixels of the image that have colors close to the
// key color of this object have their alpha values reduced.
//------------------------------------------------------------------------
void vsChromaKey::modifyAlphaFromColor(unsigned char *image, int imageWidth,
    int imageHeight)
{
    int row, column;
    unsigned char red, green, blue, alpha;
    unsigned char *imagePtr;
    int rowExtra;
    int colorDiff;

    // Set the running pointers to the start of the image buffers
    imagePtr = image;

    // Calculate the amount that the row length exceeds the word size
    rowExtra = (imageWidth * 4) % wordSize;

    // Loop through all the rows of the image
    for (row = 0; row < imageHeight; row++)
    {
        // Loop through every pixel of the row
        for (column = 0; column < imageWidth; column++)
        {
            // Grab the pixel data
            red = *(imagePtr++);
            green = *(imagePtr++);
            blue = *(imagePtr++);

            // Determine the 'difference' between the pixel color and this
            // object's key color
            colorDiff = calcDifference(red, green, blue);

            // If the difference is below the threshold, then make the pixel
            // transparent. Otherwise, it should be opaque.
            alpha = ((colorDiff <= keyThreshold) ? 0 : 255);

            // Write the pixel data to the output buffer (if the value changed)
            if (alpha == 0)
                *(imagePtr++) = alpha;
            else
                imagePtr++;
        }

        // Pad the image buffer, if needed
        if (rowExtra != 0)
            imagePtr += (wordSize - rowExtra);
    }
}

//------------------------------------------------------------------------
// Takes four separate grayscale images, each one representing a single
// channel of an RGBA image, and modifies the alpha values of the image
// such that the pixels of the image that have colors close to the key
// color of this object have their alpha values reduced.
//------------------------------------------------------------------------
void vsChromaKey::modifyAlphaFromColor(unsigned char *redChannel,
    unsigned char *greenChannel, unsigned char *blueChannel,
    unsigned char *alphaChannel, int imageWidth, int imageHeight)
{
    int row, column;
    unsigned char red, green, blue, alpha;
    unsigned char *redPtr, *greenPtr, *bluePtr, *outPtr;
    int inRowExtra, outRowExtra;
    int colorDiff;

    // Set the running pointers to the start of the image buffers
    redPtr = redChannel;
    greenPtr = greenChannel;
    bluePtr = blueChannel;
    outPtr = alphaChannel;

    // Calculate the amount that the row lengths exceed the word size
    inRowExtra = (imageWidth * 1) % wordSize;
    outRowExtra = (imageWidth * 1) % wordSize;

    // Loop through all the rows of the image
    for (row = 0; row < imageHeight; row++)
    {
        // Loop through every pixel of the row
        for (column = 0; column < imageWidth; column++)
        {
            // Grab the pixel data
            red = *(redPtr++);
            green = *(greenPtr++);
            blue = *(bluePtr++);

            // Determine the 'difference' between the pixel color and this
            // object's key color
            colorDiff = calcDifference(red, green, blue);

            // If the difference is below the threshold, then make the pixel
            // transparent. Otherwise, it should be opaque.
            alpha = ((colorDiff <= keyThreshold) ? 0 : 255);

            // Write the pixel data to the output buffer (if the value changed)
            if (alpha == 0)
                *(outPtr++) = alpha;
            else
                outPtr++;
        }

        // Pad the image buffers, if needed
        if (inRowExtra != 0)
        {
            redPtr += (wordSize - inRowExtra);
            greenPtr += (wordSize - inRowExtra);
            bluePtr += (wordSize - inRowExtra);
        }
        if (outRowExtra != 0)
            outPtr += (wordSize - outRowExtra);
    }
}

//------------------------------------------------------------------------
// Takes two images in RGB format, a 'foreground' image and a 'background'
// image, and combines them into a third, composite image. The pixels of
// the composite image are the same as the pixels of the foreground image,
// except where those pixels are close to the key color of this object; in
// that case, the pixels from the background image are factored in.
//------------------------------------------------------------------------
void vsChromaKey::combineImages(unsigned char *foregroundImage,
    unsigned char *backgroundImage, int imageWidth, int imageHeight,
    unsigned char *outputImage)
{
    int row, column;
    unsigned char foreRed, foreGreen, foreBlue;
    unsigned char backRed, backGreen, backBlue;
    unsigned char outRed, outGreen, outBlue;
    unsigned char *forePtr, *backPtr, *outPtr;
    int inRowExtra, outRowExtra;
    int colorDiff;

    // Set the running pointers to the start of the image buffers
    forePtr = foregroundImage;
    backPtr = backgroundImage;
    outPtr = outputImage;

    // Calculate the amount that the row lengths exceed the word size
    inRowExtra = (imageWidth * 3) % wordSize;
    outRowExtra = (imageWidth * 3) % wordSize;

    // Loop through all the rows of the image
    for (row = 0; row < imageHeight; row++)
    {
        // Loop through every pixel of the row
        for (column = 0; column < imageWidth; column++)
        {
            // Grab the pixel data
            foreRed = *(forePtr++);
            foreGreen = *(forePtr++);
            foreBlue = *(forePtr++);

            backRed = *(backPtr++);
            backGreen = *(backPtr++);
            backBlue = *(backPtr++);

            // Determine the 'difference' between the pixel color and this
            // object's key color
            colorDiff = calcDifference(foreRed, foreGreen, foreBlue);

            // If the difference is below the threshold, then make the pixel
            // match the background pixel. Otherwise, it should be the
            // foreground color.
            if (colorDiff <= keyThreshold)
            {
                outRed = backRed;
                outGreen = backGreen;
                outBlue = backBlue;
            }
            else
            {
                outRed = foreRed;
                outGreen = foreGreen;
                outBlue = foreBlue;
            }

            // Write the pixel data to the output buffer
            *(outPtr++) = outRed;
            *(outPtr++) = outGreen;
            *(outPtr++) = outBlue;
        }

        // Pad the image buffers, if needed
        if (inRowExtra != 0)
        {
            forePtr += (wordSize - inRowExtra);
            backPtr += (wordSize - inRowExtra);
        }
        if (outRowExtra != 0)
            outPtr += (wordSize - outRowExtra);
    }
}

//------------------------------------------------------------------------
// Private function
// Computes the 'difference' between the given color and key color
//------------------------------------------------------------------------
int vsChromaKey::calcDifference(unsigned char red, unsigned char green,
    unsigned char blue)
{
    int result = -1;
    int dr, dg, db;

    switch (keyEquation)
    {
        // Sum of differences
        case VS_CHROMAKEY_DIFF_SUM:
            result = abs(red - keyRed)
                   + abs(green - keyGreen)
                   + abs(blue - keyBlue);
            break;

        // Sum of differences squared
        case VS_CHROMAKEY_DIFF_SUM_SQUARED:
            result = AT_SQR(red - keyRed)
                   + AT_SQR(green - keyGreen)
                   + AT_SQR(blue - keyBlue);
            break;

        // Largest difference only
        case VS_CHROMAKEY_DIFF_LARGEST:
            dr = abs(red - keyRed);
            dg = abs(green - keyGreen);
            db = abs(blue - keyBlue);
            result = ((dr > dg) ? dr : dg);
            result = ((result > db) ? result : db);
            break;
    }

    return result;
}
