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
//    VESS Module:  vsCal3DMeshLoader.h++
//
//    Description:  Object for loading Cal3D mesh and material files.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CAL3D_MESH_LOADER_HPP
#define VS_CAL3D_MESH_LOADER_HPP

#include "vsObject.h++"
#include "vsArray.h++"
#include "vsComponent.h++"
#include "vsTextureAttribute.h++"
#include "vsGeometry.h++"
#include "atList.h++"

// Tags used to enclose the entire files, needed for the xml library
// to properly parse it.
#define VS_CAL3D_XML_MESH_BEGIN_TAG          "<VESS_CAL3D_MESH>"
#define VS_CAL3D_XML_MESH_END_TAG            "</VESS_CAL3D_MESH>"
#define VS_CAL3D_XML_MATERIAL_BEGIN_TAG      "<VESS_CAL3D_MATERIAL>"
#define VS_CAL3D_XML_MATERIAL_END_TAG        "</VESS_CAL3D_MATERIAL>"

#define VS_CAL3D_MESH_LOADER_MAX_INFLUENCES  4


class VESS_SYM vsCal3DMeshLoader : public vsObject
{
private:

    void                      parseXMLMaterial(char *filename);
    vsComponent               *parseXMLMesh(char *filename,
                                            vsComponent *rootNode);

    vsArray                   *materialList;
    int                       materialCount;
    atList                    *directoryList;
    
    char *                    findFile(char *filename);

VS_INTERNAL:

public:

                              vsCal3DMeshLoader();
    virtual                   ~vsCal3DMeshLoader();

    virtual const char        *getClassName();

    void                      clearMaterials();
    
    void                      addFilePath(const char *dirName);

    void                      loadMaterial(char *filename);
    vsComponent               *loadMesh(char *filename);
    vsComponent               *loadMesh(char *filename, vsComponent *rootNode);
};

#endif
