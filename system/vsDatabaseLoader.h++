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
//    VESS Module:  vsDatabaseLoader.h++
//
//    Description:  Object for loading scene databases from files
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_DATABASE_LOADER_HPP
#define VS_DATABASE_LOADER_HPP

#include <Performer/pf/pfNode.h>
#include <Performer/pfdb/pfflt.h>
#include "vsNode.h++"
#include "vsGrowableArray.h++"

#define VS_DATABASE_MODE_NAME_XFORM 1

enum vsDatabaseType
{
    VS_DATABASE_TYPE_DEFAULT,
    VS_DATABASE_TYPE_FLT
};

enum vsDatabaseUnits
{
    VS_DATABASE_UNITS_METERS,
    VS_DATABASE_UNITS_FEET,
    VS_DATABASE_UNITS_KILOMETERS
};

struct vsdbMatrixBlock
{
    char        magicString[4];
    pfMatrix    aboveMatrix;
    pfMatrix    belowMatrix;
};

class vsDatabaseLoader
{
private:

    vsGrowableArray    nodeNames;
    int                nodeNameCount;

    int                unitMode;

    int                importantXformMode;

    int                inittedFlag;

    int                classifyExtension(char *name);

    void               fixPerformerFltDOF(pfNode *node);
    void               fixGeodes(pfNode *targetGraph);
    void               replaceBillboards(pfNode *targetGraph);

VS_INTERNAL:

    void           init();

    static void    fltLoaderCallback(pfNode *node, int mgOp, int *cbs,
                                     COMMENTcb *comment, void *userData);

    int            importanceCheck(pfNode *targetNode);

public:

                   vsDatabaseLoader();
                   ~vsDatabaseLoader();

    void           initExtension(char *fileExtension);
    
    void           addImportantNodeName(char *newName);
    void           clearNames();

    void           setUnits(int databaseUnit);

    void           addPath(char *filePath);
    void           clearPath();

    void           setLoaderMode(int whichMode, int modeVal);
    int            getLoaderMode(int whichMode);

    vsComponent    *loadDatabase(char *databaseFilename);
};

#endif
