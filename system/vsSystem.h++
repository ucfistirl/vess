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

#include "vsPipe.h++"
#include "vsScreen.h++"
#include "vsDatabaseLoader.h++"
#include "vsObjectMap.h++"
#include "vsGraphicsState.h++"

#define MAX_PIPE_COUNT 10
#define MAX_SCREEN_COUNT 10

class vsSystem
{
private:

    int                 validObject;

    int                 screenCount;
    vsScreen            *screenArray[MAX_SCREEN_COUNT];
    vsPipe              *pipeArray[MAX_PIPE_COUNT];
    
    vsDatabaseLoader    *databaseLoader;
    
    vsObjectMap         *nodeMap;
    vsGraphicsState     *graphicsState;
    
    double              lastFrameTimestamp;
    double              lastFrameDuration;
    
    void                preFrameTraverse(vsNode *node);
    
    void                writeBlanks(FILE *outfile, int count);
    void                writeScene(vsNode *targetNode, FILE *outfile,
                                   int treeDepth, int *countArray);

VS_INTERNAL:

    static vsSystem    *systemObject;

    vsObjectMap        *getNodeMap();
    vsGraphicsState    *getGraphicsState();

public:

                        vsSystem(vsDatabaseLoader *fileLoader);
                        vsSystem(char *databaseFilename, char **nameList,
                                 char *windowTitle, int fullScreen,
                                 vsNode **sceneGraph, vsView **viewpoint,
                                 vsWindow **window);
                        ~vsSystem();

    vsPipe              *getPipe(int index);
    vsScreen            *getScreen(int index);
    
    vsDatabaseLoader    *getLoader();
    vsComponent         *loadDatabase(char *databaseFilename);
    
    void                drawFrame();
    
    double              getFrameTime();
    
    void                printScene(vsNode *targetNode, FILE *outputFile);
};

#endif
