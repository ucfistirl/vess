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
//    VESS Module:  
//
//    Description:  
//
//    Author(s):    
//
//------------------------------------------------------------------------
#ifndef VS_CAL3D_MATERIAL_HPP
#define VS_CAL3D_MATERIAL_HPP

#include "vsObject.h++"
#include "vsArray.h++"
#include "vsMaterialAttribute.h++"
#include "vsTextureAttribute.h++"

class vsCal3DMaterial : public vsObject
{
protected:

    vsMaterialAttribute    *material;
    int                    textureCount;
    vsArray                textures;

public:

                           vsCal3DMaterial();
                           ~vsCal3DMaterial();

    virtual const char     *getClassName();

    void                   setMaterial(vsMaterialAttribute *mat);
    vsMaterialAttribute    *getMaterial();

    void                   setTextureCount(int num);
    int                    getTextureCount();

    void                   setTexture(int unit, vsTextureAttribute *tex);
    vsTextureAttribute     *getTexture(int unit);
};

#endif
