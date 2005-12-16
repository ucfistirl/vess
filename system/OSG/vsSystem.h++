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
//    VESS Module:  vsSystem.h++
//
//    Description:  The main object in any VESS application. Exactly one
//                  of these objects should be in existance during the
//                  lifetime of the program.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SYSTEM_HPP
#define VS_SYSTEM_HPP

#include <osg/FrameStamp>
#include "vsPipe.h++"
#include "vsScreen.h++"
#include "vsTimer.h++"
#include "vsRemoteInterface.h++"
#include "vsTCPNetworkInterface.h++"
#include "vsSequencer.h++"

enum  vsMultiprocessMode
{
    VS_MPROC_DEFAULT,
    VS_MPROC_SINGLE,
    VS_MPROC_MULTI
};

class VS_SYSTEM_DLL vsSystem
{
private:

    bool                validObject;
    bool                isInitted;

    double              lastFrameDuration;
    
    // OSG's FrameStamp class, used to enumerate frames and keep
    // time between SceneView objects
    osg::FrameStamp     *osgFrameStamp;

    // Current frame number
    int                 frameNumber;

    // Total elapsed simulation time
    double              simTime;
    
    vsRemoteInterface   *remoteInterface;
    vsSequencer         *rootSequencer;

    void                preFrameTraverse(vsNode *node);
    
public:

    static vsSystem     *systemObject;

                        vsSystem();
                        ~vsSystem();

    void                setMultiprocessMode(int mpMode);
    
    void                addExtension(char *fileExtension);

    void                init();
    void                simpleInit(char *databaseFilename, char *windowName,
                                   bool fullScreen, vsNode **sceneGraph,
                                   vsView **viewpoint, vsWindow **window);

    vsSequencer         *getSequencer();

    void                drawFrame();
};

#endif
