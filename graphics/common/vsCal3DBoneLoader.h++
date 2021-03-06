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
//    VESS Module:  vsCal3DBoneLoader.h++
//
//    Description:  Object for loading Cal3D skeleton files.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CAL3D_BONE_LOADER_HPP
#define VS_CAL3D_BONE_LOADER_HPP

#include "atArray.h++"
#include "atList.h++"
#include "atString.h++"

#include "vsObject.h++"
#include "vsSkeleton.h++"
#include "vsComponent.h++"

// Tags used to enclose the entire file, needed for the xml library
// to properly parse it.
#define VS_CAL3D_XML_SKELETON_BEGIN_TAG "<VESS_CAL3D_SKELETON>"
#define VS_CAL3D_XML_SKELETON_END_TAG   "</VESS_CAL3D_SKELETON>"


class VESS_SYM vsCal3DBoneLoader : public vsObject
{
private:

    atList             directoryList;
    atArray            *boneSpaceMatrixList;

    vsComponent        *getRootBone(vsComponent *current);
    vsSkeleton         *parseXML(char *filename);
    atString           findFile(atString filename);

public:

                       vsCal3DBoneLoader();
    virtual            ~vsCal3DBoneLoader();

    virtual const char *getClassName();
    
    void               addFilePath(const char *dirName);

    vsSkeleton         *loadSkeleton(char *filename);

    atArray            *getBoneSpaceMatrixList();
};

#endif
