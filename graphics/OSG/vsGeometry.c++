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
#include "vsOSGNode.h++"

// ------------------------------------------------------------------------
// Default Constructor - Creates an OSG geode and geometry and connects
// them together, sets up empty geometry lists and configures for static
// operation (using display lists)
// ------------------------------------------------------------------------
vsGeometry::vsGeometry() 
          : vsGeometryBase()
{
    // Since the data in this geometry is intended to be relatively unchanging,
    // set the geometry's data variance to static
    osgGeometry->setDataVariance(osg::Object::STATIC);

    // Register this node and osg::Geode in the node map
    getMap()->registerLink(this, new vsOSGNode(osgGeode));
}

// ------------------------------------------------------------------------
// Destructor - Disconnects this node from its OSG counterpart and destroys
// both this node and the underlying OSG nodes
// ------------------------------------------------------------------------
vsGeometry::~vsGeometry()
{
    // Remove all parents
    detachFromParents();

    // Remove all attributes
    deleteAttributes();

    // Remove the link to the osg node from the object map
    getMap()->removeLink(this, VS_OBJMAP_FIRST_LIST);
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

