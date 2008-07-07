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
//    VESS Module:  vsGeometry.c++
//
//    Description:  vsGeometryBase subclass that handles static geometry
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include "vsGeometry.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates an OSG geode and geometry and connects
// them together, sets up empty geometry lists and configures for static
// operation (using display lists)
// ------------------------------------------------------------------------
vsGeometry::vsGeometry() 
          : vsGeometryBase()
{
    int loop;

    // Initialize the number of parents to zero
    parentCount = 0;

    // Create an osg::Geode
    osgGeode = new osg::Geode();
    osgGeode->ref();

    // Create an osg::Geometry node to contain the Geode
    osgGeometry = new osg::Geometry();
    osgGeometry->ref();
    osgGeode->addDrawable(osgGeometry);

    // Create the various data arrays
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        allocateDataArray(loop);

    // Since the data in this geometry is intended to be relatively unchanging,
    // set the geometry's data variance to static
    osgGeometry->setDataVariance(osg::Object::STATIC);

    // Initialize other values
    indexList = NULL;
    indexListSize = 0;
    lengthsList = NULL;
    primitiveCount = 0;
    primitiveType = VS_GEOMETRY_TYPE_POINTS;

    // Enable lighting on this Geometry and set the render bin to default
    enableLighting();
    renderBin = -1;

    // Register this node and osg::Geode in the node map
    getMap()->registerLink(this, osgGeode);
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its OSG counterpart and destroys
// both this node and the underlying OSG nodes
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    int loop;

    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // If we're using vertex indices, unreference the index list now
    if (indexList)
        free(indexList);

    // Destroy the data lists
    for (loop = 0; loop < VS_GEOMETRY_LIST_COUNT; loop++)
        dataList[loop]->unref();

    // Remove the link to the osg node from the object map
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);

    // Unlink and destroy the OSG objects
    osgGeometry->unref();
    osgGeode->unref();

    // If we've created a primitive lengths list, free this now
    if (lengthsList)
        free(lengthsList);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsGeometry::getClassName()
{
    return "vsGeometry";
}

// ------------------------------------------------------------------------
// Retrieves the type of this node
// ------------------------------------------------------------------------
int vsGeometry::getNodeType()
{
    return VS_NODE_TYPE_GEOMETRY;
}

