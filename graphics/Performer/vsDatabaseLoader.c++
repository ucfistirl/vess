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
#include <Performer/pfdu.h>
#include <Performer/pf/pfLOD.h>
#include <Performer/pf/pfSequence.h>
#include <Performer/pf/pfSwitch.h>
#include <Performer/pf/pfDCS.h>
#include <Performer/pr/pfLinMath.h>
#include <Performer/pf/pfBillboard.h>
#include <Performer/pr/pfFog.h>
#include <Performer/pr/pfMaterial.h>
#include <Performer/pr/pfTexture.h>
#include <Performer/pr/pfGeoState.h>
#include "vsDatabaseLoader.h++"
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsDecalAttribute.h++"
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
    // Important node name list starts empty
    nodeNameCount = 0;
    
    // Get the default loader path from the environment variable; the path
    // variable is intiialized to something just to give the clearPath()
    // function something to delete.
    loaderFilePath = strdup(".");
    clearPath();
    
    // Default database units are meters
    unitMode = VS_DATABASE_UNITS_METERS;
    
    // By default the AUTO_UNLIT mode is the only setting enabled
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
// Adds the given node name to the loader's list of "important" node names.
// Nodes with names appearing in the loader's name list are given special
// attention during the database loading process.
// ------------------------------------------------------------------------
void vsDatabaseLoader::addImportantNodeName(char *newName)
{
    // Allocate space for and duplicate the given name
    nodeNames[nodeNameCount] = strdup(newName);

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
    
    // Allocate memory for the new path string, which must be large enough
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
    strcat(fullPath, ":");
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
    if (envPath = getenv("PFPATH"))
        loaderFilePath = strdup(envPath);
    else
        loaderFilePath = strdup(".");
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
void vsDatabaseLoader::setLoaderMode(int whichMode, int modeVal)
{
    // OR the mode in if we're adding it, or ~AND it out if we're removing it
    if (modeVal)
        loaderModes |= whichMode;
    else
        loaderModes &= ~whichMode;
}

// ------------------------------------------------------------------------
// Retrieves the value of the specified loader mode
// ------------------------------------------------------------------------
int vsDatabaseLoader::getLoaderMode(int whichMode)
{
    // Check the desired mode against our mode variable
    if (loaderModes & whichMode)
        return VS_TRUE;
    else
        return VS_FALSE;
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
    pfNode *performerGraph;
    vsObjectMap *nodeMap, *attrMap;

    // Set the file search path
    pfFilePath(loaderFilePath);

    // Prepare the Performer loader/converter for the type of file
    // we're loading
    prepExtension(databaseFilename);

    // Check to see if this is an OpenFlight file
    if (classifyExtension(databaseFilename) == VS_DATABASE_TYPE_FLT)
    {
        // Is an OpenFlight file...  set the database units
        switch (unitMode)
        {
            case VS_DATABASE_UNITS_METERS:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_METERS);
                break;
            case VS_DATABASE_UNITS_FEET:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_FEET);
                break;
            case VS_DATABASE_UNITS_KILOMETERS:
                pfdConverterMode_flt(PFFLT_USEUNITS, FLT_KILOMETERS);
                break;
        }
    }

    // Load the specified file into a Performer scene graph
    performerGraph = pfdLoadFile(databaseFilename);
    if (!performerGraph)
    {
        printf("vsDatabaseLoader::loadDatabase: Load of '%s' failed\n",
	    databaseFilename);
        return NULL;
    }

    performerGraph->ref();

    // Create the object maps necessary for Performer-to-VESS conversion
    nodeMap = new vsObjectMap();
    attrMap = new vsObjectMap();

    // Convert the OSG scene graph into a VESS scene
    dbRoot = convertNode(performerGraph, nodeMap, attrMap);
    
    // Clean up the Performer scene graph (we don't need it any more)
    performerGraph->unref();
    pfMemory::checkDelete(performerGraph);

    // Dispose of the object maps
    delete nodeMap;
    delete attrMap;

    // Package the resulting database into its own component and return
    result = new vsComponent();
    result->addChild(dbRoot);

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Attempts to match the given filename extension (either a full filename
// or just the extension by itself) to the object's list of file types
// that need special attention during the loading process. Returns a
// constant specifying the type of the extension, or a generic constant
// if the extension is not recognized.
// ------------------------------------------------------------------------
int vsDatabaseLoader::classifyExtension(char *name)
{
    char *fileExtension;

    fileExtension = strrchr(name, '.');

    // OpenFlight
    if (!strcmp(name, "flt") ||
        (fileExtension && !strcmp(fileExtension, ".flt")))
        return VS_DATABASE_TYPE_FLT;

    // Default
    return VS_DATABASE_TYPE_DEFAULT;
}

// ------------------------------------------------------------------------
// Private function
// Performs last-minute initialization for the loader corresponding to the
// specified filename extension
// ------------------------------------------------------------------------
void vsDatabaseLoader::prepExtension(char *fileExtension)
{
    fltRegisterNodeT callbackPtr;
    void *callbackHandle;

    // Configure the proper performer converter based on the type
    // of file we're loading
    if (classifyExtension(fileExtension) == VS_DATABASE_TYPE_FLT)
    {
        // Configure the OpenFlight loader
        callbackPtr = fltLoaderCallback;
        callbackHandle = &callbackPtr;
        pfdConverterAttr_flt(PFFLT_REGISTER_NODE, callbackHandle);
        pfdConverterMode_flt(PFFLT_FLATTEN, PF_OFF);
        pfdConverterMode_flt(PFFLT_CLEAN, PF_OFF);
    }

}

// ------------------------------------------------------------------------
// static VESS internal function - Callback given to the OpenFlight loader
// When loading in an OpenFlight format file, this loader callback is used
// to prepare DOF beads to be 'fixed'; the standard translation from DOFs
// to Performer DCSs is incomplete. The function stores matrix data taken
// from the OpenFlight loader and stores it in the DCS's user data field,
// to be later retrieved by the convertNode function.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fltLoaderCallback(pfNode *node, int mgOp, int *cbs,
                                         COMMENTcb *comment, void *userData)
{
    // For each DCS node, pull the pre/post static transform matricies out
    // of the Flight loader data block and stuff them into the DCS's
    // user data field for later retrieval.

    DOFcb *loaderDOFBlock;
    pfDCS *currentDCS;
    vsdbMatrixBlock *myData;
    
    // Check the current bead's opcode
    switch (mgOp)
    {
        case CB_DOF:
            // DOF bead, cast the loader data to a DOFcb structure and the
            // node to a pfDCS
            loaderDOFBlock = (DOFcb *)cbs;
            currentDCS = (pfDCS *)node;

            // Create the matrix block and check for errors
            myData = (vsdbMatrixBlock *)
                (pfMemory::malloc(sizeof(vsdbMatrixBlock)));
            if (!myData)
            {
                printf("vsDatabaseLoader::fltLoaderCallback: Memory "
                    "allocation failure\n");
                return;
            }

            // Store the put matrices in the userdata field of the pfDCS
            strcpy(myData->magicString, "DOF");
            myData->aboveMatrix.copy(loaderDOFBlock->putinvmat);
            myData->belowMatrix.copy(loaderDOFBlock->putmat);
            currentDCS->setUserData(myData);

            // Delete the DOFcb structure and comment
            pfDelete(cbs);
            if (comment)
                pfDelete(comment);

            break;

        case CB_CLEANNODE:
            *cbs = TRUE;
        case CB_CLONE:
            break;
        
        default:
            if (cbs)
                pfDelete(cbs);
            if (comment)
                pfDelete(comment);
            break;
    }
}

// ------------------------------------------------------------------------
// VESS internal function
// Checks to see if the given node's name is part of the loader's list of
// 'important' node names, or if the node is a DCS and the user has
// specified that DCS's are automatically important. The name check is case
// sensitive.
// ------------------------------------------------------------------------
int vsDatabaseLoader::importanceCheck(pfNode *targetNode)
{
    int loop;
    const char *targetName;
    
    // Get the name of the target node
    targetName = targetNode->getName();

    // Return TRUE immediately if the loader is set to make all nodes
    // important
    if (loaderModes & VS_DATABASE_MODE_NAME_ALL)
        return VS_TRUE;

    // Return TRUE if the node's name is in our important name list
    for (loop = 0; loop < nodeNameCount; loop++)
        if ( targetName && !strcmp((char *)(nodeNames[loop]), targetName))
            return VS_TRUE;

    // Return TRUE if the node is a transform node
    if ((targetNode->isOfType(pfDCS::getClassType())) &&
        (loaderModes & VS_DATABASE_MODE_NAME_XFORM))
        return VS_TRUE;

    // Otherwise, the node isn't important
    return VS_FALSE;
}

// ------------------------------------------------------------------------
// Private function
// Converts a Performer graph, rooted at the specified node, into a VESS
// graph.
// ------------------------------------------------------------------------
vsNode *vsDatabaseLoader::convertNode(pfNode *node, vsObjectMap *nodeMap,
    vsObjectMap *attrMap)
{
    vsNode *result;
    vsComponent *newComponent;
    pfGroup *performerGroup;
    vsNode *child;
    int loop, sloop;

    pfLOD *lodGroup;
    vsLODAttribute *lodAttr;

    pfSequence *sequenceGroup;
    vsSequenceAttribute *sequenceAttr;
    float speed;
    int nReps;
    int loopMode;
    int begin, end;
        
    pfSwitch *switchGroup;
    vsSwitchAttribute *switchAttr;

    vsDecalAttribute *decalAttr;

    pfSCS *scsGroup = (pfSCS *)node;
    pfMatrix performerMatrix;
    vsMatrix vessMatrix;
    pfDCS *dcsGroup;
    vsdbMatrixBlock *matrixBlock; 

    // First, make sure the node is valid
    if (node == NULL)
        return NULL;

    // Next, see if we've already converted this node before
    result = (vsNode *)(nodeMap->mapSecondToFirst(node));
    if (result)
        return result;

    // Haven't seen this node before, so we need to convert.  First,
    // determine the type of Performer node we're converting
    if (node->isOfType(pfGeode::getClassType()))
    {
        // pfGeodes (and pfBillboards) are handled by a different function
        result = convertGeode((pfGeode *)node, attrMap);
    }
    else if (node->isOfType(pfGroup::getClassType()))
    {
        // Cast the pfNode to a pfGroup
        performerGroup = (pfGroup *)node;

        // This is a pfGroup (or a subclass of it), start with a new
        // vsComponent and go from there
        newComponent = new vsComponent();

        // Handle the children of this group _first_, as some of the
        // attributes need to check the number of children on the group
        // as part of their sanity checking.
        for (loop = 0; loop < performerGroup->getNumChildren(); loop++)
        {
            // Convert the child and add it to the current node
            child = convertNode(performerGroup->getChild(loop), nodeMap,
                attrMap);
            newComponent->addChild(child);
        }

        // Determine which group subtype (if any) that this group is, and
        // add the appropriate attribute(s).
        if (node->isOfType(pfLOD::getClassType()))
        {
            // LOD node, cast the node to a pfLOD and create a vsLODAttribute
            lodGroup = (pfLOD *)node;
            lodAttr = new vsLODAttribute();

            // Add the attribute before setting its data, because the 
            // attribute needs to check the number of children on the 
            // component to properly configure itself
            newComponent->addAttribute(lodAttr);

            // Copy the pfLOD ranges to the LOD attribute
            for (loop = 0; loop < lodGroup->getNumChildren(); loop++)
            {
                lodAttr->setRangeEnd(loop, lodGroup->getRange(loop+1));
            }
        }
        else if (node->isOfType(pfSequence::getClassType()))
        {
            // Sequence node, cast the node to a pfSequence and create 
            // a vsSequenceAttribute
            sequenceGroup = (pfSequence *)node;
            sequenceAttr = new vsSequenceAttribute();
        
            // Add the attribute before setting its data, because the 
            // attribute needs to check the number of children on the 
            // component to properly configure itself
            newComponent->addAttribute(sequenceAttr);

            // Copy the pfSequence parameters to the vsSequenceAttribute
            sequenceGroup->getDuration(&speed, &nReps);
            sequenceAttr->setRepetitionCount(nReps);

            // Set the frame time for each child
            for (loop = 0; loop < newComponent->getChildCount(); loop++)
            {
                sequenceAttr->setChildTime(loop, 
                    sequenceGroup->getTime(loop) * speed);
            }

            // Set the cycle mode (forward only or swing)
            sequenceGroup->getInterval(&loopMode, &begin, &end);
            if (loopMode == PFSEQ_SWING)
                sequenceAttr->setCycleMode(VS_SEQUENCE_CYCLE_SWING);
            else
                sequenceAttr->setCycleMode(VS_SEQUENCE_CYCLE_FORWARD);
        }
        else if (node->isOfType(pfSwitch::getClassType()))
        {
            // Sequence node, cast the node to a pfSwitch and create 
            // a vsSwitchAttribute
            switchGroup = (pfSwitch *)node;
            switchAttr = new vsSwitchAttribute();

            // Add the attribute before setting its data, because the 
            // attribute needs to check the number of children on the 
            // component to properly configure itself
            newComponent->addAttribute(switchAttr);

            // Start with all children off
            switchAttr->disableAll();

            // Turn one child (or all children) on according to the pfSwitch's
            // setting
            if (switchGroup->getVal() == PFSWITCH_ON)
                switchAttr->enableAll();
            else if (switchGroup->getVal() != PFSWITCH_OFF)
                switchAttr->enableOne((int)(floor(switchGroup->getVal())));
        }
        else if (node->isOfType(pfLayer::getClassType()))
        {
            // Layer (decal) node, create a vsDecalAttribute
            decalAttr = new vsDecalAttribute();

            // Add the attribute and just use the default settings (no data 
            // to copy from the pfLayer)
            newComponent->addAttribute(decalAttr);
        }
        else if (node->isOfType(pfSCS::getClassType()))
        {
            // SCS (transform) node, cast the node to a pfSCS
            scsGroup = (pfSCS *)node;

            // Create a transform attribute
            vsTransformAttribute *transformAttr = new vsTransformAttribute();

            // Add the attribute to the component
            newComponent->addAttribute(transformAttr);

            // Check if this is a DCS, if so we may have extra work to do
            if (node->isOfType(pfDCS::getClassType()))
            {
                // Cast the node to a pfDCS instead
                dcsGroup = (pfDCS *)node;

                // Check for extra pre- and post-transform data attached 
                // by our OpenFlight loader callback.  These data come from
                // OpenFlight DOF beads.
                matrixBlock = (vsdbMatrixBlock *)(dcsGroup->getUserData());
                if ((matrixBlock != NULL) &&
                    (strcmp(matrixBlock->magicString, "DOF") == 0))
                {
                    // Set the pre- and post-transforms on the transform
                    // attribute (the dynamic transform will be set as
                    // a normal DCS below)
                    
                    // First, convert the pretransform matrix to a vess
                    // matrix  (the indices are transposed on purpose)
                    for (loop = 0; loop < 4; loop++)
                    {
                        for (sloop = 0; sloop < 4; sloop++)
                        {
                            vessMatrix[loop][sloop] = 
                                matrixBlock->aboveMatrix[sloop][loop];
                        }
                    }

                    // Set the attribute's pretransform
                    transformAttr->setPreTransform(vessMatrix);
                    
                    // Next, convert the posttransform matrix to a vess
                    // matrix  (the indices are transposed on purpose)
                    for (loop = 0; loop < 4; loop++)
                    {
                        for (sloop = 0; sloop < 4; sloop++)
                        {
                            vessMatrix[loop][sloop] = 
                                matrixBlock->belowMatrix[sloop][loop];
                        }
                    }

                    // Set the attribute's posttransform
                    transformAttr->setPostTransform(vessMatrix);

                    // Finally, we're done with the user data block, so
                    // remove it and free the memory
                    dcsGroup->setUserData(NULL);
                    pfMemory::free(matrixBlock);
                }
            }

            // Get the matrix from the pfSCS node
            scsGroup->getMat(performerMatrix);

            // Copy the performer matrix to a vess matrix.  The indices are
            // transposed to properly convert between the two formats.
            for (loop = 0; loop < 4; loop++)
                for (sloop = 0; sloop < 4; sloop++)
                    vessMatrix[loop][sloop] = performerMatrix[sloop][loop];

            // Assign the matrix to one of the transform attribute's transforms
            // If the pfSCS is actually a DCS (dynamic coordinate system), use
            // the attribute's dynamic transform.  Otherwise, use the pre-
            // transform.
            if (scsGroup->isOfType(pfDCS::getClassType()))
                transformAttr->setDynamicTransform(vessMatrix);
            else
                transformAttr->setPreTransform(vessMatrix);
        }

        // Prepare the new component to be returned
        result = newComponent;
    }

    // Finally, finish converting the node's parameters
    
    // Only copy the node name if the node is 'important'
    if ((importanceCheck(node)) && (node->getName() != NULL))
        result->setName(node->getName());

    // Set the node's intersect value
    result->setIntersectValue(node->getTravMask(PFTRAV_ISECT));

    // If the node is a geometry, store it in the nodeMap so we don't
    // convert it twice.  Other nodes (groups and subtypes) will need
    // to be converted again, since VESS only allows vsComponent's to
    // have one parent.
    if (result->getNodeType() == VS_NODE_TYPE_GEOMETRY)
        nodeMap->registerLink(result, node);

    return result;
}

// ------------------------------------------------------------------------
// Private function
// Converts the given pfGeode into a VESS graph
// ------------------------------------------------------------------------
vsNode *vsDatabaseLoader::convertGeode(pfGeode *geode, vsObjectMap *attrMap)
{
    vsComponent *geodeComponent;
    vsGeometry *geometry;
    int loop;
    pfGeoSet *geoSet;
    pfBillboard *performerBillboard;
    vsBillboardAttribute *billboardAttr;
    int primType, primCount;
    pfGeoState *performerState;
    pfVec3 vec3;
    int flatFlag;
    void *attrList;
    pfGeoSet *sourceGeoSet;
    unsigned short *indexList;
    vsShadingAttribute *flatShadeAttr;

    // Each pfGeode contains any number of pfGeoSets. This structure 
    // converts into a vsComponent (the Geode), with any number of child
    // vsGeometries (the GeoSets).

    // Create the component that will represent the pfGeode;
    geodeComponent = new vsComponent();

    // Add a billboard attribute if the geode is actually a billboard
    // node
    if (geode->isOfType(pfBillboard::getClassType()))
    {
        performerBillboard = (pfBillboard *)geode;
        billboardAttr = new vsBillboardAttribute();

        // Set the billboard rotation mode
        switch (performerBillboard->getMode(PFBB_ROT))
        {
            case PFBB_AXIAL_ROT:
                billboardAttr->setMode(VS_BILLBOARD_ROT_AXIS);
                break;
            case PFBB_POINT_ROT_EYE:
                billboardAttr->setMode(VS_BILLBOARD_ROT_POINT_EYE);
                break;
            case PFBB_POINT_ROT_WORLD:
                billboardAttr->setMode(VS_BILLBOARD_ROT_POINT_WORLD);
                break;
        }

        // Set the forward direction (always +Y in Performer)
        billboardAttr->setFrontDirection(vsVector(0.0, -1.0, 0.0));

        // Set the rotation axis
        performerBillboard->getAxis(vec3);
        if (VS_EQUAL(vec3.length(), 0.0))
            billboardAttr->setAxis(vsVector(0.0, 0.0, 1.0));
        else
            billboardAttr->setAxis(vsVector(vec3[0], vec3[1], vec3[2]));

        // Set the center point
        performerBillboard->getPos(0, vec3);
        billboardAttr->setCenterPoint(vsVector(vec3[0], vec3[1], vec3[2]));

        // Add the billboard to the new component
        geodeComponent->addAttribute(billboardAttr);
    }

    // Finally, start converting GeoSets
    for (loop = 0; loop < geode->getNumGSets(); loop++)
    {
        // Get the next GeoSet
        geoSet = geode->getGSet(loop);

        // Make sure it's valid
        if (geoSet)
        {
            // Create a new vsGeometry for this GeoSet
            geometry = new vsGeometry();

            // Initialize the flatFlag to false.  This will be set to
            // true if the Performer primitive type specifies flat
            // shading.
            flatFlag = VS_FALSE;

            // If this GeoSet has a GeoState attached, convert it now
            performerState = geoSet->getGState();
            if (performerState)
            {
                // Convert the attributes on the GeoSet's GeoState
                convertAttrs(geometry, performerState, attrMap);
            }

            // Set the primitive type
            primType = geoSet->getPrimType();
            switch (primType)
            {
                case PFGS_POINTS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
                    break;
                case PFGS_LINES:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINES);
                    break;
                case PFGS_LINESTRIPS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_STRIPS);
                    break;
                case PFGS_FLAT_LINESTRIPS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_LINE_STRIPS);
                    break;
                case PFGS_TRIS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRIS);
                    break;
                case PFGS_QUADS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_QUADS);
                    break;
                case PFGS_TRISTRIPS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_STRIPS);
                    break;
                case PFGS_FLAT_TRISTRIPS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_STRIPS);
                    break;
                case PFGS_TRIFANS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_FANS);
                    break;
                case PFGS_FLAT_TRIFANS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_TRI_FANS);
                    break;
                case PFGS_POLYS:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POLYS);
                    break;
                default:
                    geometry->setPrimitiveType(VS_GEOMETRY_TYPE_POINTS);
                    break;
            }

            // Set the primitive count
            primCount = geoSet->getNumPrims();
            geometry->setPrimitiveCount(primCount);

            // If the primitive type is variable-length, copy the lengths
            if ((primType != PFGS_POINTS) && (primType != PFGS_LINES) &&
                (primType != PFGS_TRIS) && (primType != PFGS_QUADS))
            {
                geometry->setPrimitiveLengths(geoSet->getPrimLengths());
            }

            // If the Performer primitive type is one of the "FLAT" types
            // we need to preprocess the geoSet of the Normal and Color
            // lists to get them into a VESS-friendly format before copying.
            // See the pfGeoSet man page for info on how the "FLAT" types 
            // work.
            if ((primType == PFGS_FLAT_LINESTRIPS) ||
                (primType == PFGS_FLAT_TRISTRIPS) ||
                (primType == PFGS_FLAT_TRIFANS))
            {
                // Set the flatFlag, so we remember to add a flat shading
                // attribute to the resulting vsGeometry
                flatFlag = VS_TRUE;

                // Preprocess the GeoSet to remove the "flat" features
                sourceGeoSet = inflateFlatGeometry(geoSet);
            }
            else
            {
                sourceGeoSet = geoSet;
            }

            // Copy Vertex data
            sourceGeoSet->getAttrLists(PFGS_COORD3, &attrList, &indexList);
            copyData(geometry, VS_GEOMETRY_VERTEX_COORDS, 
                sourceGeoSet, sourceGeoSet->getAttrBind(PFGS_COORD3), attrList, 
                indexList);

            // Copy Normal data
            sourceGeoSet->getAttrLists(PFGS_NORMAL3, &attrList, &indexList);
            copyData(geometry, VS_GEOMETRY_NORMALS, 
                sourceGeoSet, sourceGeoSet->getAttrBind(PFGS_NORMAL3), 
                attrList, indexList);

            // Copy Color data
            sourceGeoSet->getAttrLists(PFGS_COLOR4, &attrList, &indexList);
            copyData(geometry, VS_GEOMETRY_COLORS, 
                sourceGeoSet, sourceGeoSet->getAttrBind(PFGS_COLOR4), attrList, 
                indexList);

            // Copy Texture Coordinate data
            sourceGeoSet->getAttrLists(PFGS_TEXCOORD2, &attrList, &indexList);
            copyData(geometry, VS_GEOMETRY_TEXTURE_COORDS, 
                sourceGeoSet, sourceGeoSet->getAttrBind(PFGS_TEXCOORD2), 
                attrList, indexList);

            // Check the flatFlag, and add a flat-shading attribute if
            // it is set
            if (flatFlag)
            {
                flatShadeAttr = new vsShadingAttribute();
                flatShadeAttr->setShading(VS_SHADING_FLAT);
                geometry->addAttribute(flatShadeAttr);

                // We don't need the temporary GeoSet created by 
                // inflateFlatGeometry() anymore so,
                //pfDelete(sourceGeoSet);
            }

            // Add the new geometry to the top component
            geodeComponent->addChild(geometry);
        }
    }

    return geodeComponent;
}

// ------------------------------------------------------------------------
// Private function
// Converts the contents of the given pfGeoState into VESS attributes and
// attaches them to the given node
// ------------------------------------------------------------------------
void vsDatabaseLoader::convertAttrs(vsGeometry *geometry, pfGeoState *geoState,
    vsObjectMap *attrMap)
{
    pfFog *fog;
    vsFogAttribute *fogAttr;
    pfMaterial *frontMaterial, *backMaterial;
    vsMaterialAttribute *materialAttr;
    pfTexture *texture;
    pfTexEnv *texEnv;
    vsTextureAttribute *textureAttr;
    int transpMode;
    int transpFlag;
    vsTransparencyAttribute *transpAttr;
    int loop;
    vsVector color;
    int cullMode;
    vsBackfaceAttribute *backfaceAttr;
    int shadeMode;
    vsShadingAttribute *shadeAttr;
    int wireMode;
    vsWireframeAttribute *wireAttr;

    // Check for a fog attribute
    fog = (pfFog *)(geoState->getAttr(PFSTATE_FOG));
    if (fog != NULL)
    {
        // Fog state found, create a new vsFogAttribute on the node
        fogAttr = new vsFogAttribute();
        geometry->addAttribute(fogAttr);

        // Copy the fog data from the pfFog, starting with the fog mode.
        // If Performer's advanced SPLINE mode is set, this will be changed 
        // to use LINEAR mode since VESS does not support the SPLINE mode.
        switch (fog->getFogType())
        {
            case PFFOG_PIX_LIN:
            case PFFOG_PIX_SPLINE:
                fogAttr->setEquationType(VS_FOG_EQTYPE_LINEAR);
                break;
            case PFFOG_PIX_EXP:
                fogAttr->setEquationType(VS_FOG_EQTYPE_EXP);
                break;
            case PFFOG_PIX_EXP2:
                fogAttr->setEquationType(VS_FOG_EQTYPE_EXP2);
                break;
        }

        // Copy the fog color
        float r, g, b;
        fog->getColor(&r, &g, &b);
        fogAttr->setColor((double)r, (double)g, (double)b);

        // Set the fog ranges
        float fogNear, fogFar;
        fog->getRange(&fogNear, &fogFar);
        fogAttr->setRanges((double)fogNear, (double)fogFar);
    }

    // Start with no material attribute.  This value is checked later
    // in case we need to manufacture a transparency attribute due
    // to a translucent material.
    materialAttr = NULL;

    // Check for materials
    frontMaterial = (pfMaterial *)(geoState->getAttr(PFSTATE_FRONTMTL));
    backMaterial = (pfMaterial *)(geoState->getAttr(PFSTATE_BACKMTL));
    if (frontMaterial != NULL)
    {
        // Check the attribute map to see if we've encountered this
        // material before
        materialAttr = (vsMaterialAttribute *)attrMap->
            mapSecondToFirst(frontMaterial);

        // If the map yields NULL, we need to create a new attribute
        if (materialAttr == NULL)
        {
            // Haven't seen this material before, so we need to create a
            // new attribute.  VESS material require both front and back
            // to be specified, so if there is no back material, create
            // a duplicate of the front for the back.
            if (backMaterial == NULL)
            {
                backMaterial = new pfMaterial();
                backMaterial->copy(frontMaterial);
            }

            // Create a VESS material attribute using the two Performer
            // materials
            materialAttr = new vsMaterialAttribute(frontMaterial, 
                backMaterial);

            // Link the front pfMaterial to the new attribute in the 
            // attribute map, so we can find it if it's used again
            attrMap->registerLink(materialAttr, frontMaterial);
        }

        // Add the material to the target geometry
        geometry->addAttribute(materialAttr);
    }

    // Start with no texture attribute.  This value is checked later
    // in case we need to manufacture a transparency attribute due
    // to a translucent material.
    textureAttr = NULL;

    // Check for a texture
    texture = (pfTexture *)(geoState->getAttr(PFSTATE_TEXTURE));
    texEnv = (pfTexEnv *)(geoState->getAttr(PFSTATE_TEXENV));
    if (texture != NULL)
    {
        // Check the attribute map to see if we've encountered this
        // texture before
        textureAttr = (vsTextureAttribute *)
            (attrMap->mapSecondToFirst(texture));

        // If there is no associated texture attribute, create one now
        if (textureAttr == NULL)
        {
            // VESS texture attributes require an associated Performer
            // texture environment.  If the GeoState doesn't have one,
            // we need to create one.
            if (texEnv == NULL)
            {
                texEnv = new pfTexEnv();
            }

            // Create the texture attribute from the Performer texture
            // objects
            textureAttr = new vsTextureAttribute(texture, texEnv);

            // Link the pfTexture to the new texture attribute in the
            // attribute map, so we can find it if it's used again
            attrMap->registerLink(textureAttr, texture);
        }

        // Add the attribute to the target geometry
        geometry->addAttribute(textureAttr);
    }

    // Check the transparency state at this geometry (transparency, unlike
    // the above attributes, is a simple state in Performer, and does
    // not have its own class).  First, we see if it is not set to
    // inherit its state from the global state.
    if ((geoState->getInherit() & PFSTATE_TRANSPARENCY) == 0)
    {
        // Transparency is defined here (not inherited), so get the
        // transparency mode and create a transparency attribute
        // for this geometry.
        transpMode = geoState->getMode(PFSTATE_TRANSPARENCY);
        transpAttr = new vsTransparencyAttribute();

        // See if the PFTR_NO_OCCLUDE flag is set in the transparency
        // mode and set the transparency attribute accordingly.
        if ((transpMode & PFTR_NO_OCCLUDE) == 0)
            transpAttr->enableOcclusion();
        else
            transpAttr->disableOcclusion();

        // Strip the NO_OCCLUDE flag so we can interpret the rest of
        // the transparency mode
        transpMode &= ~PFTR_NO_OCCLUDE;

        // Set the transparency enabling and mode based on the current
        // Performer transparency mode
        switch (transpMode)
        {
            case PFTR_OFF:
                transpAttr->setQuality(VS_TRANSP_QUALITY_DEFAULT);
                transpAttr->disable();
                break;

            case PFTR_ON:
                transpAttr->setQuality(VS_TRANSP_QUALITY_DEFAULT);
                transpAttr->enable();
                break;

            case PFTR_FAST:
                transpAttr->setQuality(VS_TRANSP_QUALITY_FAST);
                transpAttr->enable();
                break;

            case PFTR_HIGH_QUALITY:
            case PFTR_BLEND_ALPHA:
            case PFTR_MS_ALPHA:
            case PFTR_MS_ALPHA_MASK:
                transpAttr->setQuality(VS_TRANSP_QUALITY_HIGH);
                transpAttr->enable();
                break;
        }

        // Add the new attribute to the target geometry
        geometry->addAttribute(transpAttr);
    }
    else
    {
        // No transparency attribute set.  We still need to check if we 
        // need a transparency attribute based on material, color, or
        // texture parameters.  First, assume no transparency.
        transpFlag = VS_FALSE;

        // Check the material alpha value for less than full opacity
        if ((materialAttr != NULL) &&
            (materialAttr->getAlpha(VS_MATERIAL_SIDE_FRONT) < 1.0))
        {
            transpFlag = VS_TRUE;
        }

        // If we haven't detected transparency yet...
        if (!transpFlag)
        {
            // Check the vertex colors for non-opaque values, stop if
            // we find one
            loop = 0;
            while ((!transpFlag) &&
                (loop < geometry->getDataListSize(VS_GEOMETRY_COLORS)))
            {
                // Get the next color
                color = geometry->getData(VS_GEOMETRY_COLORS, loop);

                // Check if the alpha value is not very close to 1.0
                // (opaque)
                if (fabs(color[3] - 1.0) > 1.0E-6)
                {
                    transpFlag = VS_TRUE;
                }
            }
        }

        // If we still haven't detected transparency and this geometry
        // has a texture, check the texture image for transparent texels
        if ((!transpFlag) && 
            (textureAttr->getApplyMode() != VS_TEXTURE_APPLY_DECAL))
        {
            long texLoop, pixelSize;
            int xSize, ySize, dataFormat;
            unsigned char *imageData;

            // Get the texture image data and parameters from the texture
            // attribute
            textureAttr->getImage(&imageData, &xSize, &ySize, &dataFormat);

            // Only scan the texture if it includes an alpha channel,
            // otherwise there can be no transparency
            if (dataFormat == VS_TEXTURE_DFORMAT_RGBA)
            {
                // Compute the number of pixels in the image and
                // initialize the loop counter
                pixelSize = ((long)xSize) * ((long)ySize);
                texLoop = 0;

                // Search each texel for an alpha less than 1.0
                while ((texLoop < pixelSize) && (!transpFlag))
                {
                    // Check the alpha value; if it's less than opaque,
                    // we need transparency
                    if (imageData[(texLoop * 4) + 3] < 255)
                    {
                        transpFlag = VS_TRUE;
                        break;
                    }
                }
            }
        }

        // If the transpFlag is set, we need transparency on this
        // geometry.  Create a new attribute and add it
        if (transpFlag)
        {
            transpAttr = new vsTransparencyAttribute();
            transpAttr->enable();
            geometry->addAttribute(transpAttr);
        }
    }

    // Check for backface/cull face settings
    if ((geoState->getInherit() & PFSTATE_CULLFACE) == 0)
    {
        // Get the face culling mode from the GeoState
        cullMode = geoState->getMode(PFSTATE_CULLFACE);

        // Create a new backface attribute and set its mode based
        // on the face culling mode
        backfaceAttr = new vsBackfaceAttribute();
        if (cullMode == PFCF_OFF)
            backfaceAttr->enable();
        else
            backfaceAttr->disable();

        // Add the attribute to the target geometry
        geometry->addAttribute(backfaceAttr);
    }

    // Check for shading settings, making sure there isn't already
    // a shading attribute set from the convertGeode function (the
    // pfGeoSet "FLAT" primitive type should supercede the pfGeoState 
    // settings).
    shadeAttr = (vsShadingAttribute *)(geometry->getTypedAttribute(
        VS_ATTRIBUTE_TYPE_SHADING, 0));
    if ((shadeAttr == NULL) && 
        ((geoState->getInherit() & PFSTATE_SHADEMODEL) == 0))
    {
        // Get the shading mode from the GeoState
        shadeMode = geoState->getMode(PFSTATE_SHADEMODEL);

        // Create a new shading attribute and set the mode accordingly
        shadeAttr = new vsShadingAttribute();
        if (shadeMode == PFSM_FLAT)
            shadeAttr->setShading(VS_SHADING_FLAT);
        else
            shadeAttr->setShading(VS_SHADING_GOURAUD);

        // Add the shading attribute to the target geometry
        geometry->addAttribute(shadeAttr);
    }
    
    // Check for wireframe settings
    if ((geoState->getInherit() & PFSTATE_ENWIREFRAME) == 0)
    {
        // Get the wireframe mode from the GeoState
        wireMode = geoState->getMode(PFSTATE_ENWIREFRAME);

        // Create a new shading attribute and set the mode accordingly
        wireAttr = new vsWireframeAttribute();
        if (wireMode == PF_ON)
            wireAttr->enable();
        else
            wireAttr->disable();

        // Add the shading attribute to the target geometry
        geometry->addAttribute(wireAttr);
    }
}

// ------------------------------------------------------------------------
// Private function
// Manipulates pfGeoSets that contain FLAT primitive types into a format
// that's easier for VESS to handle
// ------------------------------------------------------------------------
pfGeoSet *vsDatabaseLoader::inflateFlatGeometry(pfGeoSet *geoSet)
{
    pfGeoSet *tempGeoSet;
    int loop, primLoop;
    int jumpCount;
    int oldPosition, newPosition;
    int binding;
    int listSum;
    pfVec3 *vertexList;
    pfVec4 *colorList;
    pfVec3 *normalList;
    pfVec2 *texCoordList;
    ushort *indexList;
    pfVec3 *newVertices;
    pfVec4 *newColors;
    pfVec3 *newNormals;
    pfVec2 *newTexCoords;
    int *lengthsList;
    int *newLengths;
    int listSize;

    // Create a new GeoSet that will hold the inflated data
    tempGeoSet = new pfGeoSet();
    tempGeoSet->setNumPrims(geoSet->getNumPrims());
    tempGeoSet->setPrimType(geoSet->getPrimType());

    // First, copy the primitive lengths list
    lengthsList = geoSet->getPrimLengths();
    listSize = (pfMemory::getSize(lengthsList) / sizeof(int));
    newLengths = (int *)(pfMemory::malloc(listSize * sizeof(int)));
    for (loop = 0; loop < listSize; loop++)
        newLengths[loop] = lengthsList[loop];

    // Attach our new list to the new GeoSet
    tempGeoSet->setPrimLengths(newLengths);

    // Next, we need to de-index all four lists (if they are currently
    // indexed), otherwise we just copy the list to the new GeoSet

    // Get the vertex list
    geoSet->getAttrLists(PFGS_COORD3, (void **)(&vertexList), &indexList);

    // Make sure it is valid
    if (vertexList != NULL)
    {
        // Determine if vertices are indexed
        if (indexList != NULL)
        {
            // Convert from indexed to non-indexed values by selectively
            // copying from the data list to a new list based on the
            // index list indices
            listSize = (pfMemory::getSize(indexList) / sizeof(ushort));
            newVertices = (pfVec3 *)
                (pfMemory::malloc(sizeof(pfVec3) * listSize));
            for (loop = 0; loop < listSize; loop++)
                (newVertices[loop]).copy(vertexList[indexList[loop]]);
        }
        else
        {
            // Duplicate the non-indexed vertex list
            listSize = (pfMemory::getSize(vertexList) / sizeof(pfVec3));
            newVertices = (pfVec3 *)
                (pfMemory::malloc(listSize * sizeof(pfVec3)));
            pfMemory::copy(newVertices, vertexList);
        }

        // Attach the new vertices to the new GeoSet
        binding = geoSet->getAttrBind(PFGS_COORD3);
        tempGeoSet->setAttr(PFGS_COORD3, binding, (void *)newVertices, NULL);
    }
    else
    {
        // No vertex list, this should not happen with a working pfGeoSet
        return geoSet;
    }

    // Get the color list
    geoSet->getAttrLists(PFGS_COLOR4, (void **)(&colorList), &indexList);

    // Make sure it is valid
    if (colorList != NULL)
    {
        // Determine if colors are indexed
        if (indexList != NULL)
        {
            // Convert from indexed to non-indexed values by selectively
            // copying from the data list to a new list based on the
            // index list indices
            listSize = (pfMemory::getSize(indexList) / sizeof(ushort));
            newColors = (pfVec4 *)
                (pfMemory::malloc(sizeof(pfVec4) * listSize));
            for (loop = 0; loop < listSize; loop++)
                (newColors[loop]).copy(colorList[indexList[loop]]);
        }
        else
        {
            // Duplicate the non-indexed color list
            listSize = (pfMemory::getSize(colorList) / sizeof(pfVec4));
            newColors = (pfVec4 *)
                (pfMemory::malloc(listSize * sizeof(pfVec4)));
            pfMemory::copy(newColors, colorList);
        }

        // Attach our new color list to the geoset
        binding = geoSet->getAttrBind(PFGS_COLOR4);
        tempGeoSet->setAttr(PFGS_COLOR4, binding, (void *)newColors, NULL);
    }
    else
    {
        // Set the color list on the new GeoSet to NULL and the binding to
        // OFF
        tempGeoSet->setAttr(PFGS_COLOR4, PFGS_OFF, NULL, NULL);
    }

    // Get the normal list
    geoSet->getAttrLists(PFGS_NORMAL3, (void **)(&normalList), &indexList);

    // Make sure it is valid
    if (normalList != NULL)
    {
        // Determine if the normals are indexed
        if (indexList != NULL)
        {
            // Convert from indexed to non-indexed values by selectively
            // copying from the data list to a new list based on the
            // index list indices
            listSize = (pfMemory::getSize(indexList) / sizeof(ushort));
            newNormals = (pfVec3 *)
                (pfMemory::malloc(sizeof(pfVec3) * listSize));
            for (loop = 0; loop < listSize; loop++)
                (newNormals[loop]).copy(normalList[indexList[loop]]);
        }
        else
        {
            // Duplicate the non-indexed normal list
            listSize = (pfMemory::getSize(normalList) / sizeof(pfVec3));
            newNormals = (pfVec3 *)
                (pfMemory::malloc(listSize * sizeof(pfVec3)));
            pfMemory::copy(newNormals, normalList);
        }

        // Attach our new normal list to the geoset
        binding = geoSet->getAttrBind(PFGS_NORMAL3);
        tempGeoSet->setAttr(PFGS_NORMAL3, binding, (void *)newNormals, NULL);
    }
    else
    {
        // Set the normal list on the new GeoSet to NULL and the binding to
        // OFF
        tempGeoSet->setAttr(PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
    }

    // Get the texture coordinate list
    geoSet->getAttrLists(PFGS_TEXCOORD2, (void **)(&texCoordList), 
        &indexList);

    // Make sure it is valid
    if (texCoordList != NULL)
    {
        // Determine if the texture coordinates are indexed
        if (indexList)
        {
            // Convert from indexed to non-indexed values by selectively
            // copying from the data list to a new list based on the
            // index list indices
            listSize = (pfMemory::getSize(indexList) / sizeof(ushort));
            newTexCoords = (pfVec2 *)
                (pfMemory::malloc(sizeof(pfVec2) * listSize));
            for (loop = 0; loop < listSize; loop++)
                (newTexCoords[loop]).copy(texCoordList[indexList[loop]]);
        }
        else
        {
            // Duplicate the non-indexed texture coord list
            listSize = (pfMemory::getSize(texCoordList) / sizeof(pfVec2));
            newTexCoords = (pfVec2 *)
                (pfMemory::malloc(listSize * sizeof(pfVec2)));
            pfMemory::copy(newTexCoords, texCoordList);
        }

        // Attach our new texture coordinate list to the new GeoSet
        binding = geoSet->getAttrBind(PFGS_TEXCOORD2);
        tempGeoSet->setAttr(PFGS_TEXCOORD2, binding, 
            (void *)newTexCoords, NULL);
    }
    else
    {
        // Set the texture coordinate list on the new GeoSet to NULL and 
        // the binding to OFF
        tempGeoSet->setAttr(PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
    }

    // Determine how much data needs to be replicated based on the
    // flat primitive type
    switch (geoSet->getPrimType())
    {
        case PFGS_FLAT_LINESTRIPS:
            jumpCount = 1;
            break;
        case PFGS_FLAT_TRISTRIPS:
        case PFGS_FLAT_TRIFANS:
            jumpCount = 2;
            break;
        default:
            // Just return the copied GeoSet as is
            return tempGeoSet;
    }

    // Get the list of primitive lengths (number of lines or tris in each
    // strip or fan)
    lengthsList = tempGeoSet->getPrimLengths();

    // Calculate the number of vertices in the geometry, based on the
    // lengths of each primitive as stored in the primitive lengths list
    listSum = 0;
    for (loop = 0; loop < tempGeoSet->getNumPrims(); loop++)
        listSum += lengthsList[loop];

    // Inflate the color data, if present and bound per-vertex
    binding = tempGeoSet->getAttrBind(PFGS_COLOR4);
    tempGeoSet->getAttrLists(PFGS_COLOR4, (void **)(&colorList), &indexList);
    if (binding == PFGS_PER_VERTEX)
    {
        // Run through the list of colors, adding extra values in to
        // compensate for what the flat primitive type leaves out; this
        // is done once per primitive.

        // Start at the beginning for both lists
        oldPosition = 0;
        newPosition = 0;

        // Create a new list
        newColors = (pfVec4 *)(pfMemory::malloc(sizeof(pfVec4) * listSum));

        // Copy values from the old list to the new one
        for (primLoop = 0; primLoop < tempGeoSet->getNumPrims(); primLoop++)
        {
            // Replicate a number of data values equal to the number
            // of values the flat primitive type normally omits
            for (loop = 0; loop < jumpCount; loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition]);

            // Then copy the rest of the data values of the primitive
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newColors[newPosition++]).copy(colorList[oldPosition++]);
        }

        // Bind the new color list to the geoset
        tempGeoSet->setAttr(PFGS_COLOR4, binding, (void *)newColors, NULL);
        pfMemory::free(colorList);
    }

    // Inflate the normal data, if present and bound per-vertex
    binding = tempGeoSet->getAttrBind(PFGS_NORMAL3);
    tempGeoSet->getAttrLists(PFGS_NORMAL3, (void **)(&normalList), &indexList);
    if (binding == PFGS_PER_VERTEX)
    {
        // Run through the list of normals, adding extra values in to
        // compensate for what the flat primitive type leaves out; this
        // is done once per primitive.

        // Start at the beginning for both lists
        oldPosition = 0;
        newPosition = 0;

        // Create a new list
        newNormals = (pfVec3 *)(pfMemory::malloc(sizeof(pfVec3) * listSum));

        // Copy values from the old list to the new one
        for (primLoop = 0; primLoop < tempGeoSet->getNumPrims(); primLoop++)
        {
            // Replicate a number of data values equal to the number
            // of values the flat primitive type normally omits
            for (loop = 0; loop < jumpCount; loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition]);

            // Then copy the rest of the data values of the primitive
            for (loop = 0; loop < (lengthsList[primLoop] - jumpCount); loop++)
                (newNormals[newPosition++]).copy(normalList[oldPosition++]);
        }

        // Bind the new normal list to the geoset
        tempGeoSet->setAttr(PFGS_NORMAL3, binding, (void *)newNormals, NULL);
        pfMemory::free(normalList);
    }

    // Set the primitive type based on the GeoSet's primitive type
    switch (geoSet->getPrimType())
    {
        case PFGS_FLAT_LINESTRIPS:
            tempGeoSet->setPrimType(PFGS_LINESTRIPS);
            break;
        case PFGS_FLAT_TRISTRIPS:
            tempGeoSet->setPrimType(PFGS_TRISTRIPS);
            break;
        case PFGS_FLAT_TRIFANS:
            tempGeoSet->setPrimType(PFGS_TRIFANS);
            break;
    }

    return tempGeoSet;
}

// ------------------------------------------------------------------------
// Private function
// Copies the geometry data from the specified Performer attribute lists
// to the target vsGeometry object.
// ------------------------------------------------------------------------
void vsDatabaseLoader::copyData(vsGeometry *targetGeometry, int targetDataType,
    pfGeoSet *geoSet, int sourceBinding, void *sourceArray, ushort *indexArray)
{
    int loop;
    int arraySize;
    pfVec2 *pfVec2Array;
    pfVec3 *pfVec3Array;
    pfVec4 *pfVec4Array;
    int primCount; 
    int vertCount;
    int max;
    int copySize;
    int idx;
    pfVec2 vec2;
    pfVec3 vec3;
    pfVec4 vec4;

    // Figure out what type of data we're dealing with (if any)
    if (sourceArray == NULL)
    {
        // No data to copy from; unbind the data type, empty the 
        // corresponding list and return
        targetGeometry->setBinding(targetDataType, VS_GEOMETRY_BIND_NONE);
        targetGeometry->setDataListSize(targetDataType, 0);
        return;
    }
    else if (targetDataType == VS_GEOMETRY_VERTEX_COORDS)
    {
        pfVec3Array = (pfVec3 *)sourceArray;
        arraySize = 3;
    }
    else if (targetDataType == VS_GEOMETRY_NORMALS)
    {
        pfVec3Array = (pfVec3 *)sourceArray;
        arraySize = 3;
    }
    else if (targetDataType == VS_GEOMETRY_COLORS)
    {
        pfVec4Array = (pfVec4 *)sourceArray;
        arraySize = 4;
    }
    else if (targetDataType == VS_GEOMETRY_TEXTURE_COORDS)
    {
        pfVec2Array = (pfVec2 *)sourceArray;
        arraySize = 2;
    }
    else
    {
        printf("vsDatabaseLoader::copyData: Unrecognized target data type\n");
        return;
    }

    // Get primitive and vertex counts
    primCount = geoSet->getNumPrims();
    vertCount = geoSet->getAttrRange(PFGS_COORD3, NULL, &max);
    vertCount = PF_MAX2(vertCount, max + 1);

    // Set the binding of the data list and determine the number of
    // data entries to copy based on the source binding
    switch (sourceBinding)
    {
        case PFGS_OFF:
            targetGeometry->setBinding(targetDataType, 
                VS_GEOMETRY_BIND_NONE);
            copySize = 0;
            break;

        case PFGS_OVERALL:
            targetGeometry->setBinding(targetDataType, 
                VS_GEOMETRY_BIND_OVERALL);
            copySize = 1;
            break;

        case PFGS_PER_PRIM:
            targetGeometry->setBinding(targetDataType, 
                VS_GEOMETRY_BIND_PER_PRIMITIVE);
            copySize = primCount;
            break;

        case PFGS_PER_VERTEX:
            targetGeometry->setBinding(targetDataType, 
                VS_GEOMETRY_BIND_PER_VERTEX);
            copySize = vertCount;
            break;
    }

    // Set the geometry data list for the given data to the proper size
    targetGeometry->setDataListSize(targetDataType, copySize);

    // Copy all of the data values this geometry uses
    for (loop = 0; loop < copySize; loop++)
    {
        if (sourceBinding == PFGS_OVERALL)
        {
            // Use the first (only) data value
            idx = 0;
        }
        else 
        {
            // Per-primitive or per-vertex binding, just use the index as is
            idx = loop;
        }

        // If the index array is specified, lookup the actual index
        // using the calculated index
        if (indexArray)
            idx = (int)indexArray[idx];

        // Copy the data from the source array to the vsGeometry, accounting
        // for the size of the input data
        switch (arraySize)
        {
            case 2:
                vec2 = pfVec2Array[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(vec2[0], vec2[1]));
                break;

            case 3:
                vec3 = pfVec3Array[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(vec3[0], vec3[1], vec3[2]));
                break;

            case 4:
                vec4 = pfVec4Array[idx];
                targetGeometry->setData(targetDataType, loop,
                    vsVector(vec4[0], vec4[1], vec4[2], vec4[3]));
                break;
        }
    }
}
