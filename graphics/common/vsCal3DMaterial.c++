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
//    VESS Module:  vsCal3DMaterial.c++
//
//    Description:  Class representing the material properties of a CAL 3D
//                  mesh.  This consists of a VESS material attribute and
//                  zero or more texture attributes.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsCal3DMaterial.h++"

// ------------------------------------------------------------------------
// Constructor.  Sets up an array to store textures, and creates a default
// material attribute
// ------------------------------------------------------------------------
vsCal3DMaterial::vsCal3DMaterial()
{
    // Initially, we have no textures
    textureCount = 0;

    // Create a default material attribute to use
    material = new vsMaterialAttribute();
    material->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_AMBIENT,
        0.3, 0.3, 0.3);
    material->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_DIFFUSE,
        0.7, 0.7, 0.7);
    material->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_SPECULAR,
        0.2, 0.2, 0.2);
    material->setColor(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_COLOR_EMISSIVE,
        0.0, 0.0, 0.0);
    material->setAlpha(VS_MATERIAL_SIDE_BOTH, 1.0);
    material->setShininess(VS_MATERIAL_SIDE_BOTH, 1.0);
    material->setColorMode(VS_MATERIAL_SIDE_BOTH, VS_MATERIAL_CMODE_NONE);
    material->ref();
}

// ------------------------------------------------------------------------
// Destructor.  Unreferences any material and texture attributes currently
// being stored
// ------------------------------------------------------------------------
vsCal3DMaterial::~vsCal3DMaterial()
{
    // Ditch the material
    vsObject::unrefDelete(material);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsCal3DMaterial::getClassName()
{
    return "vsCal3DMaterial";
}

// ------------------------------------------------------------------------
// Sets the material attribute to be used by this material
// ------------------------------------------------------------------------
void vsCal3DMaterial::setMaterial(vsMaterialAttribute *mat)
{
    // Unreference the old material
    vsObject::unrefDelete(material);

    // Store and reference the new material
    material = mat;
    material->ref();
}

// ------------------------------------------------------------------------
// Returns the current material attribute
// ------------------------------------------------------------------------
vsMaterialAttribute *vsCal3DMaterial::getMaterial()
{
    return material;
}

// ------------------------------------------------------------------------
// Sets the number of textures held by this material
// ------------------------------------------------------------------------
void vsCal3DMaterial::setTextureCount(int num)
{
    textureCount = num;
}

// ------------------------------------------------------------------------
// Returns the number of textures held by this material
// ------------------------------------------------------------------------
int vsCal3DMaterial::getTextureCount()
{
    return textureCount;
}

// ------------------------------------------------------------------------
// Sets the texture to be used by the given texture unit when rendering
// this material's mesh
// ------------------------------------------------------------------------
void vsCal3DMaterial::setTexture(int unit, vsTextureAttribute *tex)
{
    vsTextureAttribute *oldTex;

    // Replace any existing texture on the given unit with the new texture
    textures.setEntry(unit, tex);
}

// ------------------------------------------------------------------------
// Returns the texture attribute assigned to the given unit
// ------------------------------------------------------------------------
vsTextureAttribute *vsCal3DMaterial::getTexture(int unit)
{
    return (vsTextureAttribute *) textures.getEntry(unit);
}

