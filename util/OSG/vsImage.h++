//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2003, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsImage.h++
//
//    Description:  Representation of a 2D image
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#ifndef VS_IMAGE_HPP
#define VS_IMAGE_HPP

#include "vsObject.h++"

enum vsImageFormat
{
    VS_IMAGE_FORMAT_RGB
};

class VS_UTIL_DLL vsImage : public vsObject
{
    protected:
        // Like OpenGL, we will store the lower left corner first
        unsigned char * data;

        // The width (x-axis) and height (y-axis) of the image
        int             width, height;

        vsImageFormat   imageFormat;

    public:
        vsImage( );
        vsImage( int newWidth, int newHeight, vsImageFormat newImageFormat,
                const unsigned char * newData );
        vsImage( vsImage & image );
        vsImage( FILE * input );

        virtual         ~vsImage();

        const char *    getClassName();

        // Clear the image (resets to an empty 0x0 image)
        void            clear();

        // Get the size of the image in bytes
        int             getDataSize();

        // Get information about the image format
        int             getBytesPerPixel();
        vsImageFormat   getImageFormat();

        // Get width & height
        int             getWidth();
        int             getHeight();

        // Some image adjustments
        void            flipHorizontal();
        void            flipVertical();

        // Access the raw image data - cloneData() duplicates the memory and
        // returns a pointer to it that must be freed with "delete []"
        const unsigned char * getData( );
        unsigned char * cloneData();

        // (Save|Load) the image (to|from) the given file streams
        void            loadFromFile( FILE * input );
        void            saveToFile( FILE * output );
};

#endif
