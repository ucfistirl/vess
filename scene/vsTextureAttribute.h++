// File vsTextureAttribute.h++

#ifndef VS_TEXTURE_ATTRIBUTE_HPP
#define VS_TEXTURE_ATTRIBUTE_HPP

#include <Performer/pr/pfTexture.h>
#include "vsAttribute.h++"

enum vsTextureDirection
{
    VS_TEXTURE_DIRECTION_S,
    VS_TEXTURE_DIRECTION_T,
    VS_TEXTURE_DIRECTION_ALL
};

enum vsTextureBoundaryMode
{
    VS_TEXTURE_BOUNDARY_REPEAT,
    VS_TEXTURE_BOUNDARY_CLAMP
};

enum vsTextureApplyMode
{
    VS_TEXTURE_APPLY_DECAL,
    VS_TEXTURE_APPLY_MODULATE,
    VS_TEXTURE_APPLY_REPLACE
};

enum vsTextureDataFormat
{
    VS_TEXTURE_DFORMAT_INTENSITY,
    VS_TEXTURE_DFORMAT_INTENSITY_ALPHA,
    VS_TEXTURE_DFORMAT_RGB,
    VS_TEXTURE_DFORMAT_RGBA
};

enum vsTextureMagnificationFilter
{
    VS_TEXTURE_MAGFILTER_NEAREST,
    VS_TEXTURE_MAGFILTER_LINEAR
};

enum vsTextureMinificationFilter
{
    VS_TEXTURE_MINFILTER_NEAREST,
    VS_TEXTURE_MINFILTER_LINEAR,
    VS_TEXTURE_MINFILTER_MIPMAP_NEAREST,
    VS_TEXTURE_MINFILTER_MIPMAP_LINEAR
};

class vsTextureAttribute : public vsAttribute
{
private:

    pfTexture    *performerTexture;
    pfTexEnv     *performerTexEnv;
    
    pfTexture    *savedTexture;
    pfTexEnv     *savedTexEnv;

VS_INTERNAL:

                    vsTextureAttribute(pfTexture *texObject,
                                       pfTexEnv *texEnvObject);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();

public:

                   vsTextureAttribute();
                   ~vsTextureAttribute();

    virtual int    getAttributeType();
    
    void           setImage(unsigned char *imageData, int xSize, int ySize,
                            int dataFormat);
    void           getImage(unsigned char **imageData, int *xSize, int *ySize,
                            int *dataFormat);
    
    void           loadImageFromFile(char *filename);

    void           setBoundaryMode(int whichDirection, int boundaryMode);
    int            getBoundaryMode(int whichDirection);
    
    void           setApplyMode(int applyMode);
    int            getApplyMode();
    
    void           setMagFilter(int newFilter);
    int            getMagFilter();
    void           setMinFilter(int newFilter);
    int            getMinFilter();
};

#endif
