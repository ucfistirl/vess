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
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"

#define VS_DATABASE_MODE_NAME_XFORM 0x01
#define VS_DATABASE_MODE_NAME_ALL   0x02
#define VS_DATABASE_MODE_AUTO_UNLIT 0x04

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

class vsDatabaseLoader : public vsObject
{
private:

    vsGrowableArray    nodeNames;
    int                nodeNameCount;
    
    char               *loaderFilePath;

    int                unitMode;

    int                loaderModes;

    int                importanceCheck(pfNode *targetNode);

    vsNode             *convertGeode(pfGeode *geode, vsObjectMap *attrMap);
    void               convertAttrs(vsGeometry *geometry, 
                                    pfGeoState *geoState,
                                    vsObjectMap *attrMap);

    pfGeoSet           *inflateFlatGeometry(pfGeoSet *geoSet);

    void               copyData(vsGeometry *targetGeometry, int targetDataType,
                                pfGeoSet *geoSet, int sourceBinding,
                                void *sourceArray, ushort *indexArray);

    int                classifyExtension(char *name);
    void               prepExtension(char *fileExtension);
    
    void               fixPerformerFltDOF(pfNode *node);
    void               fixGeodes(pfNode *targetGraph);
    void               replaceBillboards(pfNode *targetGraph);

VS_INTERNAL:

    static void    fltLoaderCallback(pfNode *node, int mgOp, int *cbs,
                                     COMMENTcb *comment, void *userData);

    vsNode         *convertNode(pfNode *node, vsObjectMap *nodeMap,
                                vsObjectMap *attrMap);


public:

                       vsDatabaseLoader();
    virtual            ~vsDatabaseLoader();
 
    virtual const char *getClassName();

    void               addImportantNodeName(char *newName);
    void               clearNames();

    void               setUnits(int databaseUnit);

    void               addPath(char *filePath);
    void               clearPath();
    const char         *getPath();

    void               setLoaderMode(int whichMode, int modeVal);
    int                getLoaderMode(int whichMode);

    vsComponent        *loadDatabase(char *databaseFilename);
};

#endif
