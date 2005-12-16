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
//    VESS Module:  vsImage.c++
//
//    Description:  Representation of a 2D image
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
    // Need to this for the JPEG library headers to work properly
    #define __WIN32__
    #define XMD_H
#endif

extern "C"
{
    #include "jpeglib.h"
}

#include "vsImage.h++"

// ---------------------------------------------------------------------------
// Constructor:
// Creates a blank/empty image
// ---------------------------------------------------------------------------
vsImage::vsImage( )
    : data( NULL ), width( 0 ), height( 0 ), imageFormat( VS_IMAGE_FORMAT_RGB )
{
}

// ---------------------------------------------------------------------------
// Constructor:
// Create a new image with the given information (width, height, imageFormat,
// data). This duplicates the given data internally, i.e. it does NOT retain a
// pointer to the data passed in.
// ---------------------------------------------------------------------------
vsImage::vsImage( int newWidth, int newHeight, vsImageFormat newImageFormat,
        const unsigned char * newData )
    : width( newWidth ), height( newHeight ), imageFormat( newImageFormat )
{
    int dataSize = getDataSize();

    if( dataSize > 0 )
    {
        data = new unsigned char[ dataSize ];

        memcpy( data, newData, dataSize );
    }
}

// ---------------------------------------------------------------------------
// Constructor:
// Copy Constructor - duplicates a vsImage
// ---------------------------------------------------------------------------
vsImage::vsImage( vsImage & image )
{
    width = image.width;
    height = image.height;
    imageFormat = image.imageFormat;

    int dataSize = getDataSize();

    if( dataSize > 0 )
    {
        data = new unsigned char[ dataSize ];

        memcpy( data, image.data, dataSize );
    }
}

vsImage::vsImage( FILE * input )
{
    // load the given filename here
    loadFromFile( input );
}

// ---------------------------------------------------------------------------
// Destructor:
// Free up all resources used
// ---------------------------------------------------------------------------
vsImage::~vsImage()
{
    clear();
}

// ---------------------------------------------------------------------------
// Return the class name
// ---------------------------------------------------------------------------
const char * vsImage::getClassName()
{
    return "vsImage";
}

// ---------------------------------------------------------------------------
// Clears the current image and returns to a blank/empty image
// ---------------------------------------------------------------------------
void vsImage::clear()
{
    // Empty out our image data buffer
    if( data != NULL )
        delete [] data;
    data = NULL;

    // Reset the size of the image
    width = height = 0;
}

// ---------------------------------------------------------------------------
// What format is the image presently in
// ---------------------------------------------------------------------------
vsImageFormat vsImage::getImageFormat()
{
    return imageFormat;
}

// ---------------------------------------------------------------------------
// How many bytes per pixel does the present image format use?
// ---------------------------------------------------------------------------
int vsImage::getBytesPerPixel()
{
    switch( imageFormat )
    {
        case VS_IMAGE_FORMAT_RGB:
            return 3;
    }

#ifdef VESS_DEBUG
    fprintf(stderr,"vsImage::getBytesPerPixel() - invalid image format!\n");
#endif

    return 0;
}

// ---------------------------------------------------------------------------
// What is the current height (in pixels) of the image? (y axis)
// ---------------------------------------------------------------------------
int vsImage::getHeight()
{
    return height;
}

// ---------------------------------------------------------------------------
// What is the current width (in pixels) of the image? (x axis)
// ---------------------------------------------------------------------------
int vsImage::getWidth()
{
    return width;
}

// ---------------------------------------------------------------------------
// How many bytes does the image need?
// ---------------------------------------------------------------------------
int vsImage::getDataSize()
{
    return width * height * getBytesPerPixel();
}

// ---------------------------------------------------------------------------
// Get access to the raw image data
// ---------------------------------------------------------------------------
const unsigned char * vsImage::getData()
{
    return data;
}

// ---------------------------------------------------------------------------
// Duplicates the raw image data and returns a pointer to the duplicated data.
// NOTE: the returned pointer *MUST* be freed using "free()"
// ---------------------------------------------------------------------------
unsigned char * vsImage::cloneData()
{
    unsigned char * newData = NULL;
 
    if( data != NULL )
    {
        newData = (unsigned char *) malloc(getDataSize());

        memcpy( newData, data, getDataSize() );
    }

    return newData;
}

// ---------------------------------------------------------------------------
// Flip the image around the horizontal axis (i.e. this puts the top row at
// the bottom and vice versa)
// ---------------------------------------------------------------------------
void vsImage::flipVertical()
{
    int y, rowStride;

    unsigned char * tempBuffer;

    if( data != NULL )
    {
        // How wide is each row?
        rowStride = width * getBytesPerPixel();

        // Allocate a new image buffer that we will copy all the data into
        tempBuffer = new unsigned char[ getDataSize() ];

        // Flip each row one at a time
        for( y=0; y<height; y++ )
            memcpy( &tempBuffer[ (height-y-1)*rowStride ],
                    &data[ y*rowStride ], rowStride );

        // Clear out the old image buffer
        delete [] data;

        // Set the new image buffer
        data = tempBuffer;
    }
}

// ---------------------------------------------------------------------------
// Flip the image around the vertical axis (i.e. this puts the left row at
// the right and vice versa)
// ---------------------------------------------------------------------------
void vsImage::flipHorizontal()
{
    int x, y, pixelSize, rowStride;

    unsigned char * tempBuffer;

    if( data != NULL )
    {
        // Size of one pixel
        pixelSize = getBytesPerPixel();

        // how wide is a row (in bytes)
        rowStride = width * pixelSize;

        // allocate a new image buffer that we will copy all the data into
        tempBuffer = new unsigned char[ getDataSize() ];

        // Flip each pixel (size of one pixel = pixelSize) one at a time
        // This can't be very efficient... but it's the obvious way
        // and this is how OSG does it too
        for( y=0; y<height; y++ )
        {
            for( x=0; x<width; x++ )
            {
                memcpy( &tempBuffer[ (y*rowStride)+((width-x-1)*pixelSize) ],
                    &data[ (y*rowStride)+(x*pixelSize) ], pixelSize );
            }
        }

        // Clear out the old image buffer
        delete [] data;

        // Set the new image buffer
        data = tempBuffer;
    }
}

// ---------------------------------------------------------------------------
// Save the image to a file (just jpeg right now)
// ---------------------------------------------------------------------------
void vsImage::saveToFile( FILE * output )
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];

    // How many bytes wide is each of our rows
    int rowStride = width * getBytesPerPixel();

    // save to jpeg only works for RGB formats which isn't a problem right
    // now since we only support RGB images. But if we ever needed to
    // support alpha values... we'd need to drop them before writing the
    // image to libjpeg

    cinfo.err = jpeg_std_error( &jerr );

    // Allocate/Initialize the cinfo structure
    jpeg_create_compress( &cinfo );

    // Tell libjpeg to output the results to the output file stream
    jpeg_stdio_dest( &cinfo, output );

    cinfo.image_width = width;
    cinfo.image_height = height;

    // Tell libjpeg we're using an RGB format
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults( &cinfo );

    jpeg_set_quality( &cinfo, 75, TRUE );

    // Start compressing the JPEG
    jpeg_start_compress( &cinfo, TRUE );

    for( int row=0; row<height; row++ )
    {
        // Mark a pointer to our data (libjpeg expects the first row to be
        // from the top left corner of the picture while we store the lower
        // left corner of the picture first - see note in header file)
        row_pointer[0] = & data[ (height-row-1) * rowStride ];

        // Pass a single row to libjpeg at a time
        jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }

    // Finish the compression
    jpeg_finish_compress( &cinfo );

    // Free the data in the cinfo structure
    jpeg_destroy_compress( &cinfo );
}

// ---------------------------------------------------------------------------
// Load the image from a file (just jpeg right now)
// ---------------------------------------------------------------------------
void vsImage::loadFromFile( FILE * input )
{
    struct jpeg_decompress_struct dinfo;
    struct jpeg_error_mgr jerr;

    int rowStride, row, dataSize;

    // This is where we'll stuff our image data while it is loading
    // Setting 1 to a number greater than 1 would allow us to read
    // more than 1 scanline (row) at once, but let's keep things
    // simple for now
    JSAMPARRAY buffer;
    buffer = new JSAMPROW[ 1 ];

    // Remove any existing image
    clear();

    dinfo.err = jpeg_std_error( &jerr );

    // Allocate a jpeg_decompress_struct
    jpeg_create_decompress( &dinfo );

    // Tell the libjpeg that we're loading from a file
    jpeg_stdio_src( &dinfo, input );

    // Read the header of the jpeg to determine its properties
    jpeg_read_header( &dinfo, TRUE );

    // Save some important header info
    width = dinfo.image_width;
    height = dinfo.image_height;
    imageFormat = VS_IMAGE_FORMAT_RGB;
    dataSize = getDataSize();

    // Force libjpeg to give us an RGB image
    dinfo.out_color_space = JCS_RGB;
    dinfo.output_components = 3;

    // Calculate the length of each row (scanline)
    rowStride = width * 3;

    // Allocate a place to put the row data
    buffer[0] = new JSAMPLE[ rowStride ];
    
    // Allocate the final destination for the data
    data = new unsigned char[ dataSize ];

    // Start the decompression
    jpeg_start_decompress( &dinfo );

    // Read each row (scanline) one at a time
    for( row=0; row<height; row++ )
    {
        jpeg_read_scanlines( &dinfo, buffer, 1 );

        // Because we want to store the lower left part of the data first, store
        // the read data backwards (because JPEG's store the upper right part of
        // the data first)
        memcpy( & data[ (height-row-1) * rowStride ], buffer[0], rowStride );
    }

    // Indicate that the decompression is over
    jpeg_finish_decompress( &dinfo );

    // Deallocate the decompression structure
    jpeg_destroy_decompress( &dinfo );

    // Clean up our temporary data buffers
    delete [] (buffer[0]);
    delete [] buffer;
}
