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

#ifndef VS_CHROMA_KEY_HPP
#define VS_CHROMA_KEY_HPP

#include "vsObject.h++"

enum vsChromaKeyEquationType
{
    VS_CHROMAKEY_DIFF_SUM         = 0,
    VS_CHROMAKEY_DIFF_SUM_SQUARED = 1,
    VS_CHROMAKEY_DIFF_LARGEST     = 2
};

class VESS_SYM vsChromaKey : public vsObject
{
private:

    unsigned char              keyRed, keyGreen, keyBlue;
    vsChromaKeyEquationType    keyEquation;
    int                        keyThreshold;

    int                        wordSize;

    // Computes the 'difference' between the given color and key color
    int                        calcDifference(unsigned char red,
                                              unsigned char green,
                                              unsigned char blue);

public:

    // Constructor/destructor
            vsChromaKey();
            ~vsChromaKey();

    // Inherited from vsObject
    virtual const char    *getClassName();

    // Set/get key color
    void    setKeyColor(unsigned char red, unsigned char green,
                        unsigned char blue);
    void    getKeyColor(unsigned char *red, unsigned char *green,
                        unsigned char *blue);

    // Set/get 'word size' of the image data
    void    setWordSize(int size);
    int     getWordSize();

    // Set/get color matching equation type and threshold
    void    setEquationType(vsChromaKeyEquationType equationType,
                            int threshold);
    void    getEquationType(vsChromaKeyEquationType *equationType,
                            int *threshold);

    // Image processing
    void    createAlphaFromColor(unsigned char *inputImage,
                                 int imageWidth, int imageHeight,
                                 unsigned char *outputImage);
    void    createAlphaFromColor(unsigned char *redChannel,
                                 unsigned char *greenChannel,
                                 unsigned char *blueChannel,
                                 int imageWidth, int imageHeight,
                                 unsigned char *outputAlphaChannel);

    void    modifyAlphaFromColor(unsigned char *image,
                                 int imageWidth, int imageHeight);
    void    modifyAlphaFromColor(unsigned char *redChannel,
                                 unsigned char *greenChannel,
                                 unsigned char *blueChannel,
                                 unsigned char *alphaChannel,
                                 int imageWidth, int imageHeight);

    void    combineImages(unsigned char *foregroundImage,
                          unsigned char *backgroundImage,
                          int imageWidth, int imageHeight,
                          unsigned char *outputImage);
};

#endif
