// File vsSystem.h++

#ifndef VS_SYSTEM_HPP
#define VS_SYSTEM_HPP

#include "vsPipe.h++"
#include "vsScreen.h++"
#include "vsDatabaseLoader.h++"
#include "vsObjectMap.h++"
#include "vsMatrix.h++"

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
    
    vsMatrix            coordSystemXform;

VS_INTERNAL:

    static vsSystem     *systemObject;

    vsObjectMap         *getNodeMap();
    
public:

                        vsSystem(vsDatabaseLoader *fileLoader);
                        ~vsSystem();

    vsPipe              *getPipe(int index);
    vsScreen            *getScreen(int index);
    
    vsDatabaseLoader    *getLoader();
    vsNode              *loadDatabase(char *databaseFilename);
    
    void                drawFrame();

    void                setCoordinateXform(vsMatrix newXform);
    vsMatrix            getCoordinateXform();
};

#endif
