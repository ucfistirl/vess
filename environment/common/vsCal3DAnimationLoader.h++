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
//    VESS Module:  vsCal3DAnimationLoader.h++
//
//    Description:  
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CAL3D_ANIMATION_LOADER
#define VS_CAL3D_ANIMATION_LOADER

#include "vsObject.h++"
#include "vsSequencer.h++"
#include "vsSkeletonKinematics.h++"

// Tags used to enclose the entire file, needed for the xml library
// to properly parse it.
#define VS_CAL3D_XML_ANIMATION_BEGIN_TAG "<VESS_CAL3D_ANIMATION>"
#define VS_CAL3D_XML_ANIMATION_END_TAG   "</VESS_CAL3D_ANIMATION>"

class VS_ENVIRONMENT_DLL vsCal3DAnimationLoader : public vsObject
{
private:

    vsSequencer         *parseXML(char *filename,
                                  vsSkeletonKinematics *skeletonKinematics);

VS_INTERNAL:

public:

                        vsCal3DAnimationLoader();
    virtual             ~vsCal3DAnimationLoader();

    virtual const char  *getClassName();

    vsSequencer         *loadAnimation(char *filename, vsSkeletonKinematics
                                       *skeletonKinematics);
};

#endif
