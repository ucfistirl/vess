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

#include "vsObject.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"
#include "atList.h++"
#include "vsGLSLUniform.h++"
#include "atList.h++"

#define VS_DATABASE_MODE_NAME_XFORM      0x01
#define VS_DATABASE_MODE_NAME_ALL        0x02
#define VS_DATABASE_MODE_AUTO_UNLIT      0x04
#define VS_DATABASE_MODE_AUTOGEN_NORMALS 0x08

enum vsDatabaseUnits
{
    VS_DATABASE_UNITS_METERS,
    VS_DATABASE_UNITS_FEET,
    VS_DATABASE_UNITS_KILOMETERS
};

class VESS_SYM vsDatabaseLoader : public vsObject
{
private:

    atList    nodeNames;
    
    char      *loaderFilePath;

    int       unitMode;

    int       loaderModes;

    bool      importanceCheck(osg::Node *targetNode);

    char      *stringDup(char *from);

    vsNode    *convertGeode(osg::Geode *geode,
                            vsObjectMap *attrMap);
    void      convertAttrs(vsNode *node, osg::StateSet *stateSet,
                           vsObjectMap *attrMap);
    void      convertLOD(vsComponent *lodComponent,
                         osg::LOD *osgLOD);
    void      convertDecal(vsComponent *decalComponent,
                           double *offsetValues, int offsetValuesSize);

    int       copyData(vsGeometry *targetGeometry, int targetDataType,
                       int startIdx,
                       osg::PrimitiveSet *osgPrimitiveSet,
                       int sourceBinding, osg::Array *sourceArray,
                       osg::IndexArray *indexArray);

    void      copyUniformValues(vsGLSLUniform *uniform,
                                osg::Uniform *osgUniform);

VS_INTERNAL:

    vsNode      *convertNode(osg::Node *node, vsObjectMap *nodeMap,
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
    atList             *getPathList();

    void               setLoaderMode(int whichMode, bool modeVal);
    bool               getLoaderMode(int whichMode);

    vsComponent        *loadDatabase(char *databaseFilename);
};

#endif
