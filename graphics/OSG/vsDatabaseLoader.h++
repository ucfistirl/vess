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

#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "vsGrowableArray.h++"

#define VS_DATABASE_MODE_NAME_XFORM    0x01
#define VS_DATABASE_MODE_NAME_ALL      0x02
#define VS_DATABASE_MODE_AUTO_UNLIT    0x04

enum VS_GRAPHICS_DLL vsDatabaseUnits
{
    VS_DATABASE_UNITS_METERS,
    VS_DATABASE_UNITS_FEET,
    VS_DATABASE_UNITS_KILOMETERS
};

class VS_GRAPHICS_DLL vsDatabaseLoader
{
private:

    vsGrowableArray    nodeNames;
    int                nodeNameCount;
    
    char               *loaderFilePath;

    int                unitMode;

    int                loaderModes;

    int                importanceCheck(osg::Node *targetNode);

    vsNode             *convertGeode(osg::Geode *geode,
                                     vsObjectMap *attrMap);
    void               convertAttrs(vsNode *node, osg::StateSet *stateSet,
                                    vsObjectMap *attrMap);
    void               convertLOD(vsComponent *lodComponent,
                                  osg::LOD *osgLOD);
    void               convertDecal(vsComponent *decalComponent,
                                    double *offsetValues, int offsetValuesSize);

    int                copyData(vsGeometry *targetGeometry, int targetDataType,
                                int startIdx,
                                osg::PrimitiveSet *osgPrimitiveSet,
                                int sourceBinding, osg::Array *sourceArray,
                                osg::IndexArray *indexArray);

VS_INTERNAL:

    vsNode      *convertNode(osg::Node *node, vsObjectMap *nodeMap,
                             vsObjectMap *attrMap);

public:

                   vsDatabaseLoader();
    virtual        ~vsDatabaseLoader();

    void           addImportantNodeName(char *newName);
    void           clearNames();

    void           setUnits(int databaseUnit);

    void           addPath(char *filePath);
    void           clearPath();
    const char     *getPath();

    void           setLoaderMode(int whichMode, int modeVal);
    int            getLoaderMode(int whichMode);

    vsComponent    *loadDatabase(char *databaseFilename);
};

#endif
