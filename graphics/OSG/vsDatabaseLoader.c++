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
//    VESS Module:  vsDatabaseLoader.c++
//
//    Description:  Object for loading scene databases from files
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/LOD>
#include <osg/Sequence>
#include <osg/Switch>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/DOFTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Billboard>
#include <osg/Fog>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include "vsDatabaseLoader.h++"
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsLODAttribute.h++"
#include "vsSequenceAttribute.h++"
#include "vsSwitchAttribute.h++"
#include "vsTransformAttribute.h++"
#include "vsBillboardAttribute.h++"
#include "vsFogAttribute.h++"
#include "vsMaterialAttribute.h++"
#include "vsTextureAttribute.h++"
#include "vsBackfaceAttribute.h++"
#include "vsTransparencyAttribute.h++"
#include "vsWireframeAttribute.h++"
#include "vsShadingAttribute.h++"
#include "vsDecalAttribute.h++"
#include "vsOptimizer.h++"

// ------------------------------------------------------------------------
// Constructor - Adds the given file extension as the first in the loader's
// list of file extensions. Initializes the list of important node names.
// ------------------------------------------------------------------------
vsDatabaseLoader::vsDatabaseLoader() : nodeNames(0, 50)
{
    nodeNameCount = 0;
    
    // Get the default loader path from the environment variable; the path
    // variable is intiialized to something just to give the clearPath()
    // function something to delete.
    loaderFilePath = stringDup(".");
    clearPath();
    
    unitMode = VS_DATABASE_UNITS_METERS;

    loaderModes = VS_DATABASE_MODE_AUTO_UNLIT;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDatabaseLoader::~vsDatabaseLoader()
{
    // Delete the important names list
    clearNames();

    // Delete the loader path
    free(loaderFilePath);
}

// ------------------------------------------------------------------------
// Replaces strdup because, for some reason, that function is incompatible
// in Windows
// ------------------------------------------------------------------------
char *vsDatabaseLoader::stringDup(char *from)
{
    char *to;

    // Allocate the memory
    to = (char *) malloc(sizeof(char) * (strlen(from) + 1));

    // Copy the string
    strcpy(to, from);

    return to;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsDatabaseLoader::getClassName()
{
    return "vsDatabaseLoader";
}

// ------------------------------------------------------------------------
// Adds the given node name to the loader's list of "important" node names.
// Nodes with names appearing in the loader's name list are given special
// attention during the database loading process.
// ------------------------------------------------------------------------
void vsDatabaseLoader::addImportantNodeName(char *newName)
{
    // Allocate space for and duplicate the given name
    nodeNames[nodeNameCount] = stringDup(newName);

    // Check for failure
    if (!(nodeNames[nodeNameCount]))
        printf("vsDatabaseLoader::addImportantNodeName: Error allocating "
            "space for node name string\n");
    else
        nodeNameCount++;
}

// ------------------------------------------------------------------------
// Completely erases the loader's list of "important" node names.
// ------------------------------------------------------------------------
void vsDatabaseLoader::clearNames()
{
    int loop;
    
    // Delete each name in the list
    for (loop = 0; loop < nodeNameCount; loop++)
        free(nodeNames[loop]);

    nodeNameCount = 0;
}

// ------------------------------------------------------------------------
// Sets the unit translation scale used when loading in a database. Only
// has an effect if set before the database is loaded.
// ------------------------------------------------------------------------
void vsDatabaseLoader::setUnits(int databaseUnit)
{
    unitMode = databaseUnit;
}

// ------------------------------------------------------------------------
// Adds the given directory path to the search path list for loading new
// databases
// ------------------------------------------------------------------------
void vsDatabaseLoader::addPath(char *filePath)
{
    char *fullPath;

    // Allocate memory for the new path string, which much be large enough
    // to hold the old path plus the length of the new path
    fullPath = (char *)
        (malloc(strlen(filePath) + strlen(loaderFilePath) + 5));
    if (!fullPath)
    {
        printf("vsDatabaseLoader::addPath: Memory allocation error\n");
        return;
    }

    // Create the new file path, which is the old path plus the new
    // directory, separated by the path separator character
    strcpy(fullPath, loaderFilePath);
    
    // Path separator varies by platform
    #ifdef WIN32
        strcat(fullPath, ";");
    #else
        strcat(fullPath, ":");
    #endif
    
    // Add the new path
    strcat(fullPath, filePath);
    
    // Delete the old path string and store the new one
    free(loaderFilePath);
    loaderFilePath = fullPath;
}

// ------------------------------------------------------------------------
// Clears the directory path
// ------------------------------------------------------------------------
void vsDatabaseLoader::clearPath()
{
    char *envPath;

    // Delete the old path's memory
    free(loaderFilePath);

    // Attempt to get the default path from the environment; if not found,
    // default to just this directory.
    if (envPath = getenv("OSG_FILE_PATH"))
        loaderFilePath = stringDup(envPath);
    else if (envPath = getenv("OSGFILEPATH"))
        loaderFilePath = stringDup(envPath);
    else
        loaderFilePath = stringDup(".");
}

// ------------------------------------------------------------------------
// Returns the current directory path
// ------------------------------------------------------------------------
const char *vsDatabaseLoader::getPath()
{
    return loaderFilePath;
}

// ------------------------------------------------------------------------
// Sets the specified loader mode to the given value
// ------------------------------------------------------------------------
void vsDatabaseLoader::setLoaderMode(int whichMode, bool modeVal)
{
    // OR the mode in if we're adding it, ~AND it out if we're removing it
    if (modeVal)
        loaderModes |= whichMode;
    else
        loaderModes &= (~whichMode);
}

// ------------------------------------------------------------------------
// Retrieves the value of the specified loader mode
// ------------------------------------------------------------------------
bool vsDatabaseLoader::getLoaderMode(int whichMode)
{
    // Check the desired mode mask against our mode variable
    if (loaderModes & whichMode)
        return true;
    else
        return false;
}

// ------------------------------------------------------------------------
// Creates a VESS scene graph from the geometric data stored within the
// given named database file. The database file must have an extension
// that was added as a valid extension to the vsSystem object before
// init() was called.
// ------------------------------------------------------------------------
vsComponent *vsDatabaseLoader::loadDatabase(char *databaseFilename)
{
    vsNode *dbRoot;
    vsComponent *result;
    osg::Node *osgScene;
    vsObjectMap *nodeMap, *attrMap;
    vsOptimizer *optimizer;

    // Set the file search path
    (osgDB::Registry::instance())->setDataFilePathList(loaderFilePath);
    
    // Load the specified file into an OSG scene graph
    osgScene = osgDB::readNodeFile(databaseFilename);
    if (!osgScene)
    {
        printf("vsDatabaseLoader::loadDatabase: Load of '%s' failed\n",
            databaseFilename);
        return NULL;
    }
    osgScene->ref();

    // Create the object maps required for the OSG to VESS conversion
    // process to work
    nodeMap = new vsObjectMap();
    attrMap = new vsObjectMap();

    // Convert the OSG scene graph into a VESS one
    dbRoot = convertNode(osgScene, nodeMap, attrMap);
    
    // Dispose of the OSG scene graph, as it is no longer needed; all of
    // it's information is in the VESS scene now.
    osgScene->unref();
    
    // Dispose of the object maps
    delete nodeMap;
    delete attrMap;
    
    // Run the geometry-merging pass of the optimizer over the new
    // VESS scene, as the vsGeometries created by the OSG to VESS
    // conversion process are very inefficient
    optimizer = new vsOptimizer();
    optimizer->setOptimizations(VS_OPTIMIZER_MERGE_GEOMETRY);
    optimizer->optimize(dbRoot);
    delete optimizer;
    
    // Package the resulting database into its own component and return
    result = new vsComponent();
    result->addChild(dbRoot);

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Checks to see if the given node's name is part of the loader's list of
// 'important' node names, or if the node is a Transform and the user has
// specified that Transforms are automatically important. The name check is
// case sensitive.
// ------------------------------------------------------------------------
bool vsDatabaseLoader::importanceCheck(osg::Node *targetNode)
{
    int loop;
    const char *targetName;
    
    // The node is automatically important if the 'all' mode is set
    if (loaderModes & VS_DATABASE_MODE_NAME_ALL)
        return true;

    // Get the name from the OSG Node
    targetName = targetNode->getName().c_str();

    // Check the node's name against the list of important names
    for (loop = 0; loop < nodeNameCount; loop++)
        if (!strcmp((char *)(nodeNames[loop]), targetName))
            return true;

    // Check for a transform and the transforms-are-important enable
    if ((loaderModes & VS_DATABASE_MODE_NAME_XFORM) && 
        (dynamic_cast<osg::Transform *>(targetNode) != NULL))
        return true;

    return false;
}

// ------------------------------------------------------------------------
// Private function
// Converts an OSG tree, rooted at the specified node, into a VESS tree.
// ------------------------------------------------------------------------
vsNode *vsDatabaseLoader::convertNode(osg::Node *node, vsObjectMap *nodeMap,
    vsObjectMap *attrMap)
{
    vsNode *result;
    vsComponent *newComponent;
    osg::Group *osgGroup;
    vsNode *child;
    int loop, sloop;

    bool needsDecal;
    osg::StateSet *osgStateSet;
    osg::PolygonOffset *osgPolyOffset;
    double *offsetArray;
    int offsetArraySize;
    double offsetFactor;
    double offsetUnits;

    // LOD vars
    osg::LOD *lodGroup;

    // Sequence vars
    osg::Sequence *sequenceGroup;
    vsSequenceAttribute *sequenceAttr;
    float speed;
    int nReps;
    osg::Sequence::LoopMode loopMode;
    int begin, end;
            
    // Switch vars
    osg::Switch *switchGroup;
    vsSwitchAttribute *switchAttr;
            
    // Transform vars
    vsTransformAttribute *xformAttr;
    osg::Matrix osgMat;
    vsMatrix xformMat, tempMat;
    osg::DOFTransform *dofXformGroup;
    osg::MatrixTransform *matrixXformGroup;
    osg::PositionAttitudeTransform *posAttXformGroup;

    if (!node)
        return NULL;
    
    // Determine if we've seen (and converted) this node before; just
    // return the already-converted node if we have.
    result = (vsNode *)(nodeMap->mapSecondToFirst(node));
    if (result)
        return result;

    // First, determine what type of OSG Node we're dealing with here
    if (dynamic_cast<osg::Geode *>(node))
    {
        // Geodes (and Billboards) are handled by a separate function
        result = convertGeode((osg::Geode *)node, attrMap);
    }
    else if (dynamic_cast<osg::Group *>(node))
    {
        osgGroup = (osg::Group *)node;

        // This is a group (or subtype); start with a vsComponent and
        // go from there
        newComponent = new vsComponent();
        
        // Decal setup
        needsDecal = false;
        offsetArray = (double *)
            (malloc(sizeof(double) * osgGroup->getNumChildren()));
        offsetArraySize = 0;
        
        // Handle the children of this group _first_, as some of the
        // attributes need to check the number of children on the group
        // as part of their sanity checking. Also gather decal-specific
        // data while we're at it.
        for (loop = 0; loop < (int)osgGroup->getNumChildren(); loop++)
        {
            // Recurse on the loop'th child, and add the result of that as
            // a child of this component
            child = convertNode(osgGroup->getChild(loop), nodeMap, attrMap);
            newComponent->addChild(child);
            
            // Check for a PolygonOffset attribute on the group's
            // StateSet; store its offset data if it exists.
            osgStateSet = osgGroup->getChild(loop)->getStateSet();
            if (osgStateSet)
            {
                // Check for a polygon offset, for decal purposes
                osgPolyOffset = dynamic_cast<osg::PolygonOffset *>
                    (osgStateSet->getAttribute(
                    osg::StateAttribute::POLYGONOFFSET));
                if (osgPolyOffset)
                {
                    // Get the factor and units from the PolygonOffset,
                    // combine them into a single offset value, and store
                    // that value in our offsets array for later use.
                    offsetFactor = osgPolyOffset->getFactor();
                    offsetUnits = osgPolyOffset->getUnits();
                    
                    offsetArray[offsetArraySize++] =
                        -((offsetFactor * 10.0) + offsetUnits);
                
                    // If the final computed offset is non-zero, then we're
                    // going to need to put a decal attribute on the
                    // component at some point later
                    if (!(VS_EQUAL(offsetArray[offsetArraySize-1], 0.0)))
                        needsDecal = true;
                }
                else
                    offsetArray[offsetArraySize++] = 0.0;
            }
            else
                offsetArray[offsetArraySize++] = 0.0;
        }

        // Determine which group subtype (if any) that the group is,
        // provided that subtype is one of the ones that we can handle.
        if (dynamic_cast<osg::LOD *>(node))
        {
            // LOD, cast the node to an osg::LOD
            lodGroup = (osg::LOD *)node;

            // Converting an LOD node is potentially very complex, due to
            // how OSG handles them. Hand the conversion process off to a
            // dedicated function.
            convertLOD(newComponent, lodGroup);
        }
        else if (dynamic_cast<osg::Sequence *>(node))
        {
            // Sequence must be checked for before Switch, because in
            // OSG, Sequence is derived from Switch

            // Cast the node to an osg::Sequence and create a sequence
            // attribute
            sequenceGroup = (osg::Sequence *)node;
            sequenceAttr = new vsSequenceAttribute();

            // Add the attribute _before_ setting its data, because the
            // attribute checks the number of children on the component
            // before setting any values
            newComponent->addAttribute(sequenceAttr);

            // Set the loop mode (forward or swing) on the sequence
            // attribute to match the setting in the database file
            sequenceGroup->getInterval(loopMode, begin, end);
            if (loopMode == osg::Sequence::SWING)
                sequenceAttr->setCycleMode(VS_SEQUENCE_CYCLE_SWING);
            else
                sequenceAttr->setCycleMode(VS_SEQUENCE_CYCLE_FORWARD);
        }
        else if (dynamic_cast<osg::Switch *>(node))
        {
            // Switch, cast the node to an osg::Switch and create a switch
            // attribute
            switchGroup = (osg::Switch *)node;
            switchAttr = new vsSwitchAttribute();

            // Add the attribute _before_ setting its data, because the
            // attribute checks the number of children on the component
            // before setting any values
            newComponent->addAttribute(switchAttr);

            // Copy the Switch settings to the vsSwitchAttribute
            for (loop = 0; loop < newComponent->getChildCount(); loop++)
            {
                if (switchGroup->getValue(loop))
                    switchAttr->enableOne(loop);
                else
                    switchAttr->disableOne(loop);
            }
        }
        else if (dynamic_cast<osg::Transform *>(node))
        {
            // Transform, create a transform attribute and add it to the
            // component
            xformAttr = new vsTransformAttribute();
            newComponent->addAttribute(xformAttr);

            // There are three different types of transforms in OSG;
            // handle them all separately
            
            if (dynamic_cast<osg::DOFTransform *>(node))
            {
                // DOFTransform, cast the node to a DOFTransform node
                dofXformGroup = (osg::DOFTransform *)node;

                // Set the pre-transform data
                osgMat = dofXformGroup->getInversePutMatrix();
                for (loop = 0; loop < 4; loop++)
                    for (sloop = 0; sloop < 4; sloop++)
                        xformMat[loop][sloop] = osgMat(sloop, loop);
                xformAttr->setPreTransform(xformMat);

                // Set the post-transform data
                osgMat = dofXformGroup->getPutMatrix();
                for (loop = 0; loop < 4; loop++)
                    for (sloop = 0; sloop < 4; sloop++)
                        xformMat[loop][sloop] = osgMat(sloop, loop);
                xformAttr->setPostTransform(xformMat);
            }
            else if (dynamic_cast<osg::MatrixTransform *>(node))
            {
                // MatrixTransform, cast the node to an MatrixTransform node
                matrixXformGroup = (osg::MatrixTransform *)node;

                // Set the transform data
                osgMat = matrixXformGroup->getMatrix();
                for (loop = 0; loop < 4; loop++)
                    for (sloop = 0; sloop < 4; sloop++)
                        xformMat[loop][sloop] = osgMat(sloop, loop);
                xformAttr->setPreTransform(xformMat);
            }
            else if (dynamic_cast<osg::PositionAttitudeTransform *>(node))
            {
                // PositionAttitudeTransform, cast the node appropriately
                posAttXformGroup = (osg::PositionAttitudeTransform *)node;

                // * Create a transformation matrix by interpreting the
                // position and attitude data

                osg::Vec3 position, centerPoint;
                osg::Quat rotation;
                
                // Translate to the center of rotation
                centerPoint = posAttXformGroup->getPivotPoint();
                xformMat.setTranslation(-(centerPoint[0]), -(centerPoint[1]),
                    -(centerPoint[2]));

                // Add the rotation
                rotation = posAttXformGroup->getAttitude();
                tempMat.setQuatRotation(vsQuat(rotation[0], rotation[1],
                    rotation[2], rotation[3]));
                xformMat = tempMat * xformMat;
                
                // Translate back; this translation is assumed to include
                // both the reverse translation to the center point, and
                // the translation to the desired location
                position = posAttXformGroup->getPosition();
                tempMat.setTranslation(position[0], position[1],
                    position[2]);
                xformMat = tempMat * xformMat;
                
                xformAttr->setDynamicTransform(xformMat);
            }
        } // if (dynamic_cast<osg::Transform *>(node))
        
        // Create a decal attribute on the new component if needed
        if (needsDecal)
            convertDecal(newComponent, offsetArray, offsetArraySize);
        
        // Clean up after the decal work
        free(offsetArray);

        // Now handle the contents of the OSG Node's StateSet
        if (node->getStateSet())
            convertAttrs(newComponent, node->getStateSet(), attrMap);
        
        result = newComponent;
    } // if (dynamic_cast<osg::Geode *>(node))

    // Return NULL if the conversion didn't go correctly (in which case
    // we'll have a NULL result here)
    if (!result)
        return NULL;

    // Lastly, convert the stuff that's not specific to the node type:
    // name and (intersection) mask.

    // * Only copy the node name if the node is 'important'
    if (importanceCheck(node))
        result->setName(node->getName().c_str());

    // Copy the node's node mask and use it for the vsNode's intersect
    // value
    result->setIntersectValue(node->getNodeMask());

    // Store the result of this operation in the node map, so that we
    // don't try to re-convert this node if we run across it again
    // * New: Only store the result if the node is a geometry; due to
    // the fact that components can only have one parent, we don't
    // want to try adding the same component to a second parent if
    // we run across it again, but rather we'll just treat it as one
    // we've never seen before and convert it again.
    if (result->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        nodeMap->registerLink(result, node);

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Converts the given OSG Geode into a VESS tree
// ------------------------------------------------------------------------
vsNode *vsDatabaseLoader::convertGeode(osg::Geode *geode, vsObjectMap *attrMap)
{
    // Each OSG Geode contains any number of Geometries, which themselves
    // can contain any number of PrimitiveSets. This structure converts
    // into a vsComponent (the Geode), with any number of child
    // vsComponents (the Geometries), each with any number of child
    // vsGeometries (the PrimitiveSets).

    vsComponent *geodeComponent, *childComponent;
    vsGeometry *geometry;
    int loop, sloop, tloop;
    osg::Geometry *osgGeometry;
    osg::Billboard *osgBillboard;
    vsBillboardAttribute *billboardAttr;
    osg::PrimitiveSet *osgPrimitiveSet;
    int primSetType, primCount, vertCount;
    osg::DrawArrayLengths *osgDrawLengthsPrim;

    osg::Vec2 osgVec2;
    osg::Vec3 osgVec3;
    osg::Vec4 osgVec4;
    int normalMark, colorMark, texCoordMark;

    bool needsDecal;
    osg::StateSet *osgStateSet;
    osg::PolygonOffset *osgPolyOffset;
    double *offsetArray;
    int offsetArraySize;
    double offsetFactor;
    double offsetUnits;

    // Create the component that represents the osg Geode
    geodeComponent = new vsComponent();

    // Convert the attributes on the Geode's StateSet into vess attributes
    // on the new component
    if (geode->getStateSet())
        convertAttrs(geodeComponent, geode->getStateSet(), attrMap);

    // If the OSG Geode is actually a Billboard, add a vsBillboardAttribute
    // to the master component
    if (dynamic_cast<osg::Billboard *>(geode))
    {
        osgBillboard = (osg::Billboard *)geode;
        billboardAttr = new vsBillboardAttribute();
        
        // Copy the billboard parameters
        switch (osgBillboard->getMode())
        {
            case osg::Billboard::POINT_ROT_EYE:
                billboardAttr->setMode(VS_BILLBOARD_ROT_POINT_EYE);
                break;
            case osg::Billboard::POINT_ROT_WORLD:
                billboardAttr->setMode(VS_BILLBOARD_ROT_POINT_WORLD);
                break;
            case osg::Billboard::AXIAL_ROT:
                billboardAttr->setMode(VS_BILLBOARD_ROT_AXIS);
                break;
        }
        
        osgVec3 = osgBillboard->getNormal();
        billboardAttr->setFrontDirection(vsVector(osgVec3[0], osgVec3[1],
            osgVec3[2]));
        
        osgVec3 = osgBillboard->getAxis();
        if (VS_EQUAL(osgVec3.length(),0.0))
            billboardAttr->setAxis(vsVector(0.0, 0.0, 1.0));
        else
            billboardAttr->setAxis(vsVector(osgVec3[0], osgVec3[1],
                osgVec3[2]));

        geodeComponent->addAttribute(billboardAttr);
    }

    // Decal stuff
    needsDecal = false;
    offsetArray = (double *)(malloc(sizeof(double) * geode->getNumDrawables()));
    offsetArraySize = 0;

    // Convert each osg::Geometry into one or more vsGeometry objects (one
    // per PrimitiveSet)
    for (loop = 0; loop < (int)geode->getNumDrawables(); loop++)
    {
        // Obtain the loop'th Geometry. (If it's not a Geometry, ignore it.)
        osgGeometry = dynamic_cast<osg::Geometry *>(geode->getDrawable(loop));
        if (osgGeometry)
        {
            // Create a new component to represent the Geometry
            childComponent = new vsComponent();
            geodeComponent->addChild(childComponent);

            // Handle the geometry's state set, if it has one
            osgStateSet = osgGeometry->getStateSet();
            if (osgStateSet)
            {
                // Convert the attributes on the geometry's state set
                convertAttrs(childComponent, osgStateSet, attrMap);
                
                // Check for a polygon offset, for decal purposes
                osgPolyOffset = dynamic_cast<osg::PolygonOffset *>
                    (osgStateSet->getAttribute(
                    osg::StateAttribute::POLYGONOFFSET));
                if (osgPolyOffset)
                {
                    // Get the factor and units from the PolygonOffset,
                    // combine them into a single offset value, and store
                    // that value in our offsets array for later use.
                    offsetFactor = osgPolyOffset->getFactor();
                    offsetUnits = osgPolyOffset->getUnits();
                    
                    offsetArray[offsetArraySize++] =
                        -((offsetFactor * 10.0) + offsetUnits);
                
                    // If the final computed offset is non-zero, then we're
                    // going to need to put a decal attribute on the
                    // component at some point later
                    if (!(VS_EQUAL(offsetArray[offsetArraySize-1], 0.0)))
                        needsDecal = true;
                }
                else
                    offsetArray[offsetArraySize++] = 0.0;
            }
            else
                offsetArray[offsetArraySize++] = 0.0;
        
            // Start at the beginning of each data list
            normalMark = 0;
            colorMark = 0;
            texCoordMark = 0;
        
            // For each primitive set on the Geometry, create a vsGeometry
            // that contains the same information
            for (sloop = 0; sloop < (int)osgGeometry->getNumPrimitiveSets(); 
                sloop++)
            {
                // Create a new vsGeometry and get the next PrimitiveSet
                // from the osg::Geometry object
                geometry = new vsGeometry();
                osgPrimitiveSet = osgGeometry->getPrimitiveSet(sloop);
                
                // * Type
                primSetType = osgPrimitiveSet->getMode();
                switch (primSetType)
                {
                    case osg::PrimitiveSet::POINTS:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
                        break;
                    case osg::PrimitiveSet::LINES:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINES);
                        break;
                    case osg::PrimitiveSet::LINE_STRIP:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_STRIPS);
                        break;
                    case osg::PrimitiveSet::LINE_LOOP:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_LOOPS);
                        break;
                    case osg::PrimitiveSet::TRIANGLES:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRIS);
                        break;
                    case osg::PrimitiveSet::TRIANGLE_STRIP:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_STRIPS);
                        break;
                    case osg::PrimitiveSet::TRIANGLE_FAN:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_FANS);
                        break;
                    case osg::PrimitiveSet::QUADS:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
                        break;
                    case osg::PrimitiveSet::QUAD_STRIP:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUAD_STRIPS);
                        break;
                    case osg::PrimitiveSet::POLYGON:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POLYS);
                        break;
                    default:
                        geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
                        break;
                }
                
                // * Primitive Count & Primitive Lengths
                primCount = osgPrimitiveSet->getNumPrimitives();
                vertCount = osgPrimitiveSet->getNumIndices();
                geometry->setPrimitiveCount(primCount);

                // If this is a variable-length type, copy the lengths
                if ((primSetType != osg::PrimitiveSet::POINTS) &&
                    (primSetType != osg::PrimitiveSet::LINES) &&
                    (primSetType != osg::PrimitiveSet::TRIANGLES) &&
                    (primSetType != osg::PrimitiveSet::QUADS))
                {
                    if (osgPrimitiveSet->getType() ==
                        osg::PrimitiveSet::DrawArrayLengthsPrimitiveType)
                    {
                        osgDrawLengthsPrim =
                            (osg::DrawArrayLengths *)osgPrimitiveSet;
                        for (tloop = 0; tloop < primCount; tloop++)
                            geometry->setPrimitiveLength(tloop,
                                (*osgDrawLengthsPrim)[tloop]);
                    }
                    else
                        geometry->setPrimitiveLength(0, vertCount);
                }

                // * Vertex Coordinates
                copyData(geometry, VS_GEOMETRY_VERTEX_COORDS, 0,
                    osgPrimitiveSet, osg::Geometry::BIND_PER_VERTEX,
                    osgGeometry->getVertexArray(),
                    osgGeometry->getVertexIndices());
                
                // * Normals
                normalMark += copyData(geometry, VS_GEOMETRY_NORMALS,
                    normalMark, osgPrimitiveSet,
                    osgGeometry->getNormalBinding(),
                    osgGeometry->getNormalArray(), 
                    osgGeometry->getNormalIndices());
                
                // Check for the presence of normals; if none, and the
                // appropriate loader mode is set, then disable
                // lighting on this geometry
                if ((loaderModes & VS_DATABASE_MODE_AUTO_UNLIT) &&
                    (geometry->getBinding(VS_GEOMETRY_NORMALS) ==
                        VS_GEOMETRY_BIND_NONE))
                    geometry->disableLighting();

                // * Colors
                colorMark += copyData(geometry, VS_GEOMETRY_COLORS,
                    colorMark, osgPrimitiveSet,
                    osgGeometry->getColorBinding(),
                    osgGeometry->getColorArray(), 
                    osgGeometry->getColorIndices());

                // * Texture Coordinates
                texCoordMark += copyData(geometry, VS_GEOMETRY_TEXTURE_COORDS,
                    texCoordMark, osgPrimitiveSet,
                    osg::Geometry::BIND_PER_VERTEX,
                    osgGeometry->getTexCoordArray(0), 
                    osgGeometry->getTexCoordIndices(0));
                
                // Add the new vsGeometry object to the OSG Geometry's 
                // vsComponent
                childComponent->addChild(geometry);
            } // loop (sloop < osgGeometry->getNumPrimitiveSets())
        } // if (osgGeometry)
    } // loop (loop < geode->getNumDrawables())
    
    // Add a decal attribute to the resulting component if needed
    if (needsDecal)
        convertDecal(geodeComponent, offsetArray, offsetArraySize);

    // We're done with the polygon offset values
    free(offsetArray);

    // Return the component
    return geodeComponent;
}

// ------------------------------------------------------------------------
// Private function
// Converts the contents of the given OSG StateSet into attributes and
// attaches them to the given vsNode
// ------------------------------------------------------------------------
void vsDatabaseLoader::convertAttrs(vsNode *node, osg::StateSet *stateSet,
    vsObjectMap *attrMap)
{
    const osg::StateSet::RefAttributePair *osgRefAttrPair;
    unsigned int overrideFlag;

    osg::Fog *osgFog;
    vsFogAttribute *vsFogAttr;

    osg::Material *osgMaterial;
    vsMaterialAttribute *vsMaterialAttr;

    osg::Texture *osgTexture;
    osg::TexEnv *osgTexEnv;
    vsTextureAttribute *vsTextureAttr;

    vsTransparencyAttribute *vsTransparencyAttr;

    osg::CullFace *osgCullFace;
    int cullfaceMode;
    vsBackfaceAttribute *vsBackfaceAttr;

    osg::ShadeModel *osgShadeModel;
    vsShadingAttribute *vsShadingAttr;

    osg::PolygonMode *osgPolyMode;
    vsWireframeAttribute *vsWireframeAttr;
    
    // Fog
    osgFog = dynamic_cast<osg::Fog *>
        (stateSet->getAttribute(osg::StateAttribute::FOG));
    if (osgFog)
    {
        // Create a new fog attribute on the node
        vsFogAttr = new vsFogAttribute();
        node->addAttribute(vsFogAttr);

        // Copy the fog data from the osg Fog
        switch (osgFog->getMode())
        {
            case osg::Fog::LINEAR:
                vsFogAttr->setEquationType(VS_FOG_EQTYPE_LINEAR);
                break;
            case osg::Fog::EXP:
                vsFogAttr->setEquationType(VS_FOG_EQTYPE_EXP);
                break;
            case osg::Fog::EXP2:
                vsFogAttr->setEquationType(VS_FOG_EQTYPE_EXP2);
                break;
        }

        osg::Vec4 fogColor;
        fogColor = osgFog->getColor();
        vsFogAttr->setColor(fogColor[0], fogColor[1], fogColor[2]);

        double fogNear, fogFar;
        fogNear = osgFog->getStart();
        fogFar = osgFog->getEnd();
        vsFogAttr->setRanges(fogNear, fogFar);

        // Check the status of the override flag
        osgRefAttrPair = stateSet->getAttributePair(osg::StateAttribute::FOG);
        overrideFlag = osgRefAttrPair->second;
        if (overrideFlag & osg::StateAttribute::OVERRIDE)
            vsFogAttr->setOverride(true);
    }

    // Material
    osgMaterial = dynamic_cast<osg::Material *>
        (stateSet->getAttribute(osg::StateAttribute::MATERIAL));
    if (osgMaterial)
    {
        // Check for a previous encounter with this material
        vsMaterialAttr = (vsMaterialAttribute *)
            (attrMap->mapSecondToFirst(osgMaterial));
        
        if (!vsMaterialAttr)
        {
            // Haven't found this one before; create a new VESS material
            // attribute around it
            vsMaterialAttr = new vsMaterialAttribute(osgMaterial);
            
            // Check the status of the override flag
            osgRefAttrPair =
                stateSet->getAttributePair(osg::StateAttribute::MATERIAL);
            overrideFlag = osgRefAttrPair->second;
            if (overrideFlag & osg::StateAttribute::OVERRIDE)
                vsMaterialAttr->setOverride(true);

            // Record that we've seen this material, in case it comes up again
            attrMap->registerLink(vsMaterialAttr, osgMaterial);
        }

        // Recognized or not, add the material to this node
        node->addAttribute(vsMaterialAttr);
    }
    
    // Texture
    // Note here that we're dynamic-casting to a Texture2D object, not just
    // any Texture type. If the texture isn't a Texture2D, the cast will
    // fail and return a NULL, which will make the function think that there's
    // no texture at all. Since VESS currently only supports 2-dimensional
    // textures anyway, this is considered acceptable behavior.
    osgTexture = dynamic_cast<osg::Texture2D *>
        (stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    osgTexEnv = dynamic_cast<osg::TexEnv *>
        (stateSet->getTextureAttribute(0, osg::StateAttribute::TEXENV));
    if (osgTexture)
    {
        // Check for a previous encounter with this texture
        vsTextureAttr = (vsTextureAttribute *)
            (attrMap->mapSecondToFirst(osgTexture));

        if (!vsTextureAttr)
        {
            // Haven't found this one before; create a new VESS texture
            // attribute around it

            // Create a new texture environment object for use by the texture
            // attribute. (We don't want to use the one that came with the
            // texture object, because it's possible that the TexEnv may have
            // been used in other places that the Texture wasn't.)
            if (!osgTexEnv)
            {
                osgTexEnv = new osg::TexEnv();
                osgTexEnv->setMode(osg::TexEnv::DECAL);
            }
            else
                osgTexEnv = new osg::TexEnv(*osgTexEnv);

            vsTextureAttr = 
                 new vsTextureAttribute((osg::Texture2D *)osgTexture, 
                     osgTexEnv);

            // Check the status of the override flag
            osgRefAttrPair = stateSet->getTextureAttributePair(0,
                osg::StateAttribute::TEXTURE);
            overrideFlag = osgRefAttrPair->second;
            if (overrideFlag & osg::StateAttribute::OVERRIDE)
                vsTextureAttr->setOverride(true);

            // Record that we've seen this texture, in case it comes up again
            attrMap->registerLink(vsTextureAttr, osgTexture);
        }

        // Recognized or not, add the texture to this node
        node->addAttribute(vsTextureAttr);
    }

    // Transparency
    // Check to see if a render bin has been specified for this node
    if (stateSet->useRenderBinDetails())
    {
        // Create a new transparency attribute on the node
        vsTransparencyAttr = new vsTransparencyAttribute();
        node->addAttribute(vsTransparencyAttr);

        // Copy the transparency setting
        if (stateSet->getRenderingHint() == osg::StateSet::TRANSPARENT_BIN)
            vsTransparencyAttr->enable();
        else
            vsTransparencyAttr->disable();
        
        // Check the status of the override flag
        overrideFlag = stateSet->getRenderBinMode();
        if (overrideFlag == osg::StateSet::OVERRIDE_RENDERBIN_DETAILS)
            vsTransparencyAttr->setOverride(true);
    }

    // Backface (Cull Face)
    // Check to see if the cull face mode for this node is not inherited
    cullfaceMode = stateSet->getMode(GL_CULL_FACE);
    if (!(cullfaceMode & osg::StateAttribute::INHERIT))
    {
        // Create a new backface attribute on the node
        vsBackfaceAttr = new vsBackfaceAttribute();
        node->addAttribute(vsBackfaceAttr);
        
        // Determine what the backface mode should be set to by
        // examining both the cull face enable mode and the state of
        // the osg CullFace object, if any.
        if (cullfaceMode & osg::StateAttribute::ON)
        {
            // Face culling is enabled; check for the presence of an
            // osg CullFace object
            osgCullFace = dynamic_cast<osg::CullFace *>
                (stateSet->getAttribute(osg::StateAttribute::CULLFACE));
        
            if (osgCullFace)
            {
                // If the state of the CullFace object is set to cull
                // back (or front and back), then disable backfacing
                // on the attribute. Otherwise, backfaces should still
                // be enabled. VESS can't really cull front faces, so
                // settings to that effect are ignored.
                if (osgCullFace->getMode() == osg::CullFace::FRONT)
                    vsBackfaceAttr->enable();
                else
                    vsBackfaceAttr->disable();
            }
            else
            {
                // If there's no CullFace object, then assume there's
                // no culling.
                vsBackfaceAttr->enable();
            }
        }
        else
        {
            // Face culling is disabled entirely; back faces will
            // be visible
            vsBackfaceAttr->enable();
        }

        // Check the status of the override flag
        if (cullfaceMode & osg::StateAttribute::OVERRIDE)
            vsBackfaceAttr->setOverride(true);
    }

    // Shading
    // Check for a shading model on this node
    osgShadeModel = dynamic_cast<osg::ShadeModel *>
        (stateSet->getAttribute(osg::StateAttribute::SHADEMODEL));
    if (osgShadeModel)
    {
        // Create a new shading attribute on the node
        vsShadingAttr = new vsShadingAttribute();
        node->addAttribute(vsShadingAttr);
        
        // Copy the shading mode
        if (osgShadeModel->getMode() == osg::ShadeModel::FLAT)
            vsShadingAttr->setShading(VS_SHADING_FLAT);
        else
            vsShadingAttr->setShading(VS_SHADING_GOURAUD);

        // Check the status of the override flag
        osgRefAttrPair =
            stateSet->getAttributePair(osg::StateAttribute::SHADEMODEL);
        overrideFlag = osgRefAttrPair->second;
        if (overrideFlag & osg::StateAttribute::OVERRIDE)
            vsShadingAttr->setOverride(true);
    }
    
    // Wireframe (Polygon Mode)
    // Check for a polygon mode attribute on this node
    osgPolyMode = dynamic_cast<osg::PolygonMode *>
        (stateSet->getAttribute(osg::StateAttribute::POLYGONMODE));
    if (osgPolyMode)
    {
        // Create a new wireframe attribute on the node
        vsWireframeAttr = new vsWireframeAttribute();
        node->addAttribute(vsWireframeAttr);

        // Copy the polygon mode
        if (osgPolyMode->getMode(osg::PolygonMode::FRONT_AND_BACK) ==
            osg::PolygonMode::FILL)
            vsWireframeAttr->disable();
        else
            vsWireframeAttr->enable();

        // Check the status of the override flag
        osgRefAttrPair =
            stateSet->getAttributePair(osg::StateAttribute::POLYGONMODE);
        overrideFlag = osgRefAttrPair->second;
        if (overrideFlag & osg::StateAttribute::OVERRIDE)
            vsWireframeAttr->setOverride(true);
    }
}

// ------------------------------------------------------------------------
// Private function
// Constructs a vsLODAttribute on the specified vsComponent, based on the
// data in the specified OSG LOD object
// ------------------------------------------------------------------------
void vsDatabaseLoader::convertLOD(vsComponent *lodComponent, osg::LOD *osgLOD)
{
    // * The complexity of this function comes from the fact that LOD
    // nodes in osg have a minimum and maximum range, rather than just
    // a maximum like VESS has, and they can be in any order, rather
    // than the closest-to-farthest order that VESS imposes. This means
    // that osg LOD ranges can overlap, even to the extent that one node's
    // range can completely encompass another's. This function handles
    // range overlaps and gaps, and rearranges the children of the
    // component to be in a format that a vsLODAttribute can accurately
    // represent the visibility of.

    // * This function operates by using the range values given in the
    // osg LOD node to come up with a list of key node distances. At
    // each key distance, at least one node is either beginning or
    // ending its range of visibility. Then, once these distances
    // are sorted, each pair of distances becomes a range within which
    // the visibility of the children does not change. The children that
    // are visible in each range are added to a (newly created) component,
    // which is then itself added to the original component that will
    // eventually receive the vsLODAttribute.

    float *rangeList;
    int rangeListSize;
    int loop, sloop;
    bool flag;
    float tempFloat;
    vsGrowableArray nodeList(10, 10);
    int nodeListSize;
    vsNode *childNode;
    vsComponent *childComponent;
    vsLODAttribute *lodAttr;
    float midpoint;
    
    // * Create a list of ranges out of the minimum and maximum range
    // values for each child, as specified by the osg LOD object. 
    rangeListSize = (osgLOD->getNumRanges() * 2) + 1;
    rangeList = (float *)(malloc(sizeof(float) * rangeListSize));

    // Artificially add the minimum-distance lower bound to the mix
    // of ranges, in case none of the osg LOD's minimum ranges go
    // that low
    rangeList[0] = 0.0;
    
    // Copy the range values from the osg LOD into our list
    for (loop = 0; loop < (int)osgLOD->getNumRanges(); loop++)
    {
        rangeList[(loop * 2) + 1] = osgLOD->getMinRange(loop);
        rangeList[(loop * 2) + 2] = osgLOD->getMaxRange(loop);
    }
    
    // * Sort the list of ranges, eliminating duplicates (bubble sort)
    flag = true;
    while (flag)
    {
        flag = false;
        
        for (loop = 0; loop < rangeListSize-1; loop++)
        {
            if (VS_EQUAL(rangeList[loop], rangeList[loop+1]))
            {
                // Delete one of the equal range values by copying
                // the last range value over it; the sorting process
                // will take care of putting the ranges back into order.
                rangeList[loop] = rangeList[rangeListSize - 1];
                rangeListSize--;
                flag = true;
            }
            else if (rangeList[loop] > rangeList[loop+1])
            {
                // Swap the range values
                tempFloat = rangeList[loop];
                rangeList[loop] = rangeList[loop+1];
                rangeList[loop+1] = tempFloat;
                flag = true;
            }
        }
    }
    
    // * Remove the children from the lodComponent and place them in
    // a holding list
    nodeListSize = lodComponent->getChildCount();
    for (loop = 0; loop < nodeListSize; loop++)
    {
        // Always getting the first node ensures that the nodes are
        // transferred in the correct order
        childNode = lodComponent->getChild(0);
        lodComponent->removeChild(childNode);
        nodeList[loop] = childNode;
    }

    // * For each range, determine which nodes should be visible within
    // that range; add those nodes to a new component that represents
    // that range.
    for (loop = 0; loop < (rangeListSize-1); loop++)
    {
        // Create a new component to represent the range
        childComponent = new vsComponent();
        lodComponent->addChild(childComponent);

        // Create a representative value for the range by calculating
        // the range's midpoint
        midpoint = (rangeList[loop] + rangeList[loop+1]) / 2.0;
        
        // For each node whose osg range includes the midpoint value, add
        // that node to the new component
        for (sloop = 0; sloop < nodeListSize; sloop++)
        {
            if ((midpoint > osgLOD->getMinRange(sloop)) &&
                (midpoint < osgLOD->getMaxRange(sloop)))
            {
                childNode = (vsNode *)(nodeList[sloop]);

                // If the child can't have any more parents, then we need
                // to add a clone of the child instead
                if ((childNode->getNodeType() == VS_NODE_TYPE_COMPONENT) &&
                    (childNode->getParentCount() > 0))
                    childNode = childNode->cloneTree();

                childComponent->addChild(childNode);
            }
        }
    }
    
    // * Create a vsLODAttribute and attach it to the lodComponent, and
    // configure the attribute's ranges with the values from the range list
    lodAttr = new vsLODAttribute();
    lodComponent->addAttribute(lodAttr);
    for (loop = 0; loop < (rangeListSize-1); loop++)
        lodAttr->setRangeEnd(loop, rangeList[loop+1]);

    // * Done. Clean up.
    free(rangeList);
}

// ------------------------------------------------------------------------
// Private function
// Attempts to create a vsDecalAttribute on the decalComponent and
// reorganize the children on the component so that each one is offset the
// proper amount. Uses the displacement data in the offsetValues array to
// determine the order in which the children should be placed on the
// decalComponent.
// ------------------------------------------------------------------------
void vsDatabaseLoader::convertDecal(vsComponent *decalComponent,
    double *offsetValues, int offsetValuesSize)
{
    double *offsetArray;
    int offsetArraySize;
    int loop, sloop;
    bool flag;
    double tempDouble;
    vsGrowableArray decalChildren(10, 10);
    vsNode *decalChild;
    double closestDist;
    int closestIdx;
    vsComponent *childComponent;
    
    // If the decalComponent already contains a grouping category attribute,
    // then we can't add a decal to it; abort. (Theoretically, it should
    // still be possible to add the decal in by adding in some more
    // groups at the right places, but the situation should come up so
    // infrequently that it's not worth the work to support it.)
    if (decalComponent->getCategoryAttribute(VS_ATTRIBUTE_CATEGORY_GROUPING, 0))
        return;

    // Verify that the number of offset values is equal to the number
    // of children on the decalComponent
    if (offsetValuesSize != decalComponent->getChildCount())
    {
        printf("vsDatabaseLoader::convertDecal: Inconsistency between "
            "offset array size and component child count\n");
        return;
    }
    
    // Copy the offset data from the offsetValues array to our new
    // offsetArray, adding in an extra zero entry
    offsetArraySize = offsetValuesSize + 1;
    offsetArray = (double *)(malloc(sizeof(double) * offsetArraySize));
    offsetArray[0] = 0.0;
    for (loop = 0; loop < offsetValuesSize; loop++)
        offsetArray[loop+1] = offsetValues[loop];

    // Sort the entries in the offsetArray, eliminating duplicate entries
    flag = true;
    while (flag)
    {
        flag = false;
        for (loop = 0; loop < offsetArraySize-1; loop++)
        {
            if (VS_EQUAL(offsetArray[loop], offsetArray[loop+1]))
            {
                // Copy the last entry in the list over the entry to
                // be deleted, and shrink the list. The sorting mechanism
                // will put the moved entry back into its proper place.
                offsetArray[loop+1] = offsetArray[offsetArraySize-1];
                offsetArraySize--;
                flag = true;
            }
            else if (offsetArray[loop] > offsetArray[loop+1])
            {
                // Swap
                tempDouble = offsetArray[loop];
                offsetArray[loop] = offsetArray[loop+1];
                offsetArray[loop+1] = tempDouble;
                flag = true;
            }
        }
    }
    
    // If there's only one entry left in the offsetArray, then all of
    // the offset values are (near) zero; there's no need to place
    // a decal attribute here.
    if (offsetArraySize == 1)
    {
        free(offsetArray);
        return;
    }
    
    // Remove the children from the decalComponent and place them in a
    // temporary holding array
    for (loop = 0; loop < offsetValuesSize; loop++)
    {
        decalChild = decalComponent->getChild(0);
        decalComponent->removeChild(decalChild);
        decalChildren[loop] = decalChild;
    }
    
    // Add a vsDecalAttribute to the decalComponent, and add a number of
    // new child components to the component equal to the size of what's
    // left of the offsets array
    decalComponent->addAttribute(new vsDecalAttribute());
    for (loop = 0; loop < offsetArraySize; loop++)
        decalComponent->addChild(new vsComponent());

    // Add the original children of the decalComponent to the new
    // child components of the decalComponent
    for (loop = 0; loop < offsetValuesSize; loop++)
    {
        // Determine which of the offsetArray values that the child's
        // offsetValue is closest to, and add that child to the decalComponent
        // child component corresponding to that closest value. (We have to
        // do this because the values may not be exactly equal; the
        // duplicate entry elimination routine can remove entries that
        // are very close to each other but not exactly equal.)
        closestDist = fabs(offsetValues[loop] - offsetArray[0]);
        closestIdx = 0;

        // Search for the closest entry
        for (sloop = 1; sloop < offsetArraySize; sloop++)
            if (fabs(offsetValues[loop] - offsetArray[sloop]) < closestDist)
            {
                closestDist = fabs(offsetValues[loop] - offsetArray[sloop]);
                closestIdx = sloop;
            }

        // Add the child to the newly determined 'closest' component
        childComponent = ((vsComponent *)
            (decalComponent->getChild(closestIdx)));
        childComponent->addChild((vsNode *)(decalChildren[loop]));
    }
    
    // Clean up
    free(offsetArray);
}

// ------------------------------------------------------------------------
// Private function
// Copies the geometry data from the specified osg array to the target
// vsGeometry object, using the data in the given osg PrimitiveSet, along
// with the array of data indices and other values
// ------------------------------------------------------------------------
int vsDatabaseLoader::copyData(vsGeometry *targetGeometry, int targetDataType,
    int startIdx, osg::PrimitiveSet *osgPrimitiveSet, int sourceBinding,
    osg::Array *sourceArray, osg::IndexArray *indexArray)
{
    int loop;
    osg::DrawArrays *osgDrawArraysPrim;
    osg::DrawArrayLengths *osgDrawLengthsPrim;
    osg::DrawElementsUByte *osgDrawByteElemsPrim;
    osg::DrawElementsUShort *osgDrawShortElemsPrim;
    osg::DrawElementsUInt *osgDrawIntElemsPrim;
    osg::Vec2Array *osgVec2Array;
    osg::Vec3Array *osgVec3Array;
    osg::Vec4Array *osgVec4Array;
    osg::Vec2 osgVec2;
    osg::Vec3 osgVec3;
    osg::Vec4 osgVec4;
    int idx;
    int vertCount, primCount;
    int osgArraySize;
    int copySize;
    
    // Figure out what type of data array we're dealing with (if any)
    if (sourceArray == NULL)
    {
        // No data to copy from; unbind the data type, empty the
        // corresponding list, and return.
        targetGeometry->setBinding(targetDataType, VS_GEOMETRY_BIND_NONE);
        targetGeometry->setDataListSize(targetDataType, 0);
        return 0;
    }
    else if (dynamic_cast<osg::Vec2Array *>(sourceArray))
    {
        osgVec2Array = (osg::Vec2Array *)sourceArray;
        osgArraySize = 2;
    }
    else if (dynamic_cast<osg::Vec3Array *>(sourceArray))
    {
        osgVec3Array = (osg::Vec3Array *)sourceArray;
        osgArraySize = 3;
    }
    else if (dynamic_cast<osg::Vec4Array *>(sourceArray))
    {
        osgVec4Array = (osg::Vec4Array *)sourceArray;
        osgArraySize = 4;
    }
    else
    {
        printf("vsDatabaseLoader::copyData: Parameter 'sourceArray' is an "
            "unrecognized OSG Array subtype\n");
        return -1;
    }

    // Get the number of primitives and vertices in the primitive set
    primCount = osgPrimitiveSet->getNumPrimitives();
    vertCount = osgPrimitiveSet->getNumIndices();

    // Set the binding of the data list and determine the number of
    // data entries to copy based on the source binding
    switch (sourceBinding)
    {
        case osg::Geometry::BIND_OFF:
            targetGeometry->setBinding(targetDataType,
                VS_GEOMETRY_BIND_NONE);
            copySize = 0;
            break;

        case osg::Geometry::BIND_OVERALL:
        case osg::Geometry::BIND_PER_PRIMITIVE_SET:
            targetGeometry->setBinding(targetDataType,
                VS_GEOMETRY_BIND_OVERALL);
            copySize = 1;
            break;

        case osg::Geometry::BIND_PER_PRIMITIVE:
            targetGeometry->setBinding(targetDataType,
                VS_GEOMETRY_BIND_PER_PRIMITIVE);
            copySize = primCount;
            break;

        case osg::Geometry::BIND_PER_VERTEX:
            targetGeometry->setBinding(targetDataType,
                VS_GEOMETRY_BIND_PER_VERTEX);
            copySize = vertCount;
            break;
    }
    
    // Set the new size of the vsGeometry's data list to the size we computed
    // above
    targetGeometry->setDataListSize(targetDataType, copySize);
    
    // Copy all of the data values this geometry uses
    for (loop = 0; loop < copySize; loop++)
    {
        // Determine the index of the data value to copy
        if (sourceBinding == osg::Geometry::BIND_PER_VERTEX)
        {
            // Use the information in the PrimitiveSet to determine the
            // index into the source data to copy from
            switch (osgPrimitiveSet->getType())
            {
                case osg::PrimitiveSet::DrawArraysPrimitiveType:
                    // Offset the index by the 'first index' value of
                    // the primitive set
                    osgDrawArraysPrim =
                        (osg::DrawArrays *)osgPrimitiveSet;
                    idx = loop + osgDrawArraysPrim->getFirst();
                    break;

                case osg::PrimitiveSet::DrawArrayLengthsPrimitiveType:
                    // Offset the index by the 'first index' value of
                    // the primitive set
                    osgDrawLengthsPrim =
                        (osg::DrawArrayLengths *)osgPrimitiveSet;
                    idx = loop + osgDrawLengthsPrim->getFirst();
                    break;

                case osg::PrimitiveSet::DrawElementsUBytePrimitiveType:
                    // Values to use are specified by index from the
                    // primitive's index array
                    osgDrawByteElemsPrim =
                        (osg::DrawElementsUByte *)osgPrimitiveSet;
                    idx = (*osgDrawByteElemsPrim)[loop];
                    break;

                case osg::PrimitiveSet::DrawElementsUShortPrimitiveType:
                    // Values to use are specified by index from the
                    // primitive's index array
                    osgDrawShortElemsPrim =
                        (osg::DrawElementsUShort *)osgPrimitiveSet;
                    idx = (*osgDrawShortElemsPrim)[loop];
                    break;

                case osg::PrimitiveSet::DrawElementsUIntPrimitiveType:
                    // Values to use are specified by index from the
                    // primitive's index array
                    osgDrawIntElemsPrim =
                        (osg::DrawElementsUInt *)osgPrimitiveSet;
                    idx = (*osgDrawIntElemsPrim)[loop];
                    break;
            } // switch (osgPrimitiveSet->getType())
        }
        else if (sourceBinding == osg::Geometry::BIND_OVERALL)
        {
            // Always use the first (and only) data value
            idx = 0;
        }
        else
        {
            // Calculate the source index from the 'startIdx' parameter
            idx = loop + startIdx;
        }
        
        // If the index array is specified, lookup the actual index
        // using the calculated index
        if (indexArray)
            idx = indexArray->index(idx);
        
        // Copy the data from the source array to the vsGeometry, accounting
        // for the size of the input data
        switch (osgArraySize)
        {
            case 2:
                osgVec2 = (*osgVec2Array)[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(osgVec2[0], osgVec2[1]));
                break;

            case 3:
                osgVec3 = (*osgVec3Array)[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(osgVec3[0], osgVec3[1], osgVec3[2]));
                break;

            case 4:
                osgVec4 = (*osgVec4Array)[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(osgVec4[0], osgVec4[1], osgVec4[2], osgVec4[3]));
                break;
        }
    } // for (loop = 0; loop < copySize; loop++)

    // Return the number of elements copied, so the source index can
    // be updated for the next primitive set
    return copySize;
}
