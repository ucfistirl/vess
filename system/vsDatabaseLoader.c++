// File vsDatabaseLoader.c++

#include "vsDatabaseLoader.h++"

#include <stdlib.h>
#include <string.h>
#include <Performer/pf/pfGeode.h>
#include <Performer/pf/pfBillboard.h>
#include <Performer/pf/pfDCS.h>
#include <Performer/pfdu.h>
#include "vsGeometry.h++"
#include "vsComponent.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructor - Adds the given file extension as the first in the loader's
// list of file extensions. Initializes the list of important node names.
// ------------------------------------------------------------------------
vsDatabaseLoader::vsDatabaseLoader(char *fileExtension) :
    extensions(1, 5), nodeNames(0, 50)
{
    extensions[0] = strdup(fileExtension);
    extensionCount = 1;
    nodeNameCount = 0;
    
    unitMode = VS_DATABASE_UNITS_METERS;
    
    importantXformMode = VS_FALSE;
    
    inittedFlag = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsDatabaseLoader::~vsDatabaseLoader()
{
    clearNames();
}

// ------------------------------------------------------------------------
// Adds the given filename extension to the loader's list of recognized
// extensions. Can only be called before a vsSystem object is created.
// ------------------------------------------------------------------------
void vsDatabaseLoader::addExtension(char *fileExtension)
{
    if (inittedFlag)
    {
        printf("vsDatabaseLoader::addExtension: Can't add extensions after "
            "loader initialization by vsSystem object\n");
        return;
    }

    extensions[extensionCount] = strdup(fileExtension);
    if (!(extensions[extensionCount]))
        printf("vsDatabaseLoader::addExtension: Error allocating space "
            "for extension string\n");
    else
        extensionCount++;
}

// ------------------------------------------------------------------------
// Adds the given node name to the loader's list of "important" node names.
// Nodes with names appearing in the loader's name list are given special
// attention during the database loading process.
// ------------------------------------------------------------------------
void vsDatabaseLoader::addImportantNodeName(char *newName)
{
    nodeNames[nodeNameCount] = strdup(newName);
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
    const char *performerPath;
    char fullPath[1000];
    
    performerPath = pfGetFilePath();
    if (!performerPath)
        strcpy(fullPath, ".");
    else
        strcpy(fullPath, pfGetFilePath());

    strcat(fullPath, ":");
    strcat(fullPath, filePath);
    
    pfFilePath(fullPath);
    printf("Path is: %s\n", fullPath);
}

// ------------------------------------------------------------------------
// Sets the specified loader mode to the given value
// ------------------------------------------------------------------------
void vsDatabaseLoader::setLoaderMode(int whichMode, int modeVal)
{
    switch (whichMode)
    {
	case VS_DATABASE_MODE_NAME_XFORM:
	    importantXformMode = modeVal;
	    break;
    }
}

// ------------------------------------------------------------------------
// Retrieves the value of the specified loader mode
// ------------------------------------------------------------------------
int vsDatabaseLoader::getLoaderMode(int whichMode)
{
    switch (whichMode)
    {
	case VS_DATABASE_MODE_NAME_XFORM:
	    return importantXformMode;
    }
    
    return 0;
}

// ------------------------------------------------------------------------
// Creates a VESS scene graph from the geometric data stored within the
// given named database file. The database file must have an extension
// that was specified as a valid extension to the loader before the
// vsSystem object was created.
// ------------------------------------------------------------------------
vsNode *vsDatabaseLoader::loadDatabase(char *databaseFilename)
{
    vsNode *result;
    pfNode *performerGraph;

    if (!inittedFlag)
    {
        printf("vsDatabaseLoader::loadDatabase: Can't load database until "
            "loader initialization by vsSystem object\n");
        return NULL;
    }

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

    performerGraph = pfdLoadFile(databaseFilename);
    if (!performerGraph)
    {
        printf("vsDatabaseLoader::loadDatabase: Load failed\n");
        return NULL;
    }

    if (classifyExtension(databaseFilename) == VS_DATABASE_TYPE_FLT)
    {
        // Is an OpenFlight file...  fix the DOF/DCS nodes
        fixPerformerFltDOF(performerGraph);
    }
    
    // Separate each geoset into a different geode. This may slow things down
    // slightly but ultimately the scene graph is easier to handle this way.
    fixGeodes(performerGraph);

    // Bottle the Performer graph into a VESS tree
    if (performerGraph->isOfType(pfGroup::getClassType()))
        result = new vsComponent((pfGroup *)performerGraph, this);
    else if (performerGraph->isOfType(pfGeode::getClassType()))
        result = new vsGeometry((pfGeode *)performerGraph);
    else
    {
        printf("vsDatabaseLoader::loadDatabase: No geometry found\n");
        pfDelete(performerGraph);
        result = NULL;
    }
    
    // * Replace all pfBillboards in the Performer scene with pfGeodes; the
    // database loader should have extracted all of the relavant information
    // into vsBillboardAttributes by now.
    replaceBillboards(performerGraph);
    
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
// "Fixes" Performer DCS nodes generated by the OpenFlight loader when
// reading in DOF beads. The fix involves creating a pair of SCS nodes
// around the DCS to allow for correct off-origin rotations.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fixPerformerFltDOF(pfNode *node)
{
    int loop;
    int childCount;
    pfNode *childNode;
    pfGroup *groupNode, *parentGroup;
    pfDCS *DCSNode;
    pfSCS *aboveSCS, *belowSCS;
    vsdbMatrixBlock *myData;

    // First, if this node is a group, recurse down through the scene
    if (node->isOfType(pfGroup::getClassType()))
    {
        groupNode = (pfGroup *)node;
        childCount = groupNode->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixPerformerFltDOF(groupNode->getChild(loop));
    }

    // Second, if this node is a DCS, apply the fix
    if (node->isOfType(pfDCS::getClassType()))
    {
        DCSNode = (pfDCS *)node;
        myData = (vsdbMatrixBlock *)(DCSNode->getUserData());
        if (!myData || (strcmp(myData->magicString, "DOF")))
        {
            // Error: loader matrix data not available
            printf("vsDatabaseLoader::fixPerformerFltDOF: Unable to obtain "
                "DOF data\n");
            return;
        }

        aboveSCS = new pfSCS(myData->aboveMatrix);
        belowSCS = new pfSCS(myData->belowMatrix);

        // Move all of the children of the DCS to the 'below' SCS
        while (DCSNode->getNumChildren() > 0)
        {
            childNode = DCSNode->getChild(0);
            DCSNode->removeChild(childNode);
            belowSCS->addChild(childNode);
        }

        parentGroup = DCSNode->getParent(0);
        parentGroup->replaceChild(DCSNode, aboveSCS);
        aboveSCS->addChild(DCSNode);
        DCSNode->addChild(belowSCS);

        // Clean up; delete the data and reset the user data pointer
        DCSNode->setUserData(NULL);
        pfMemory::free(myData);
    }
}

// ------------------------------------------------------------------------
// Private function
// "Fixes" Performer geodes by splitting the geosets within the geode
// into separate geodes. This action makes the VESS scene graph easier to
// manipulate.
// ------------------------------------------------------------------------
void vsDatabaseLoader::fixGeodes(pfNode *targetGraph)
{
    int loop, childCount;
    pfGeode *oldGeode, *newGeode;
    pfBillboard *oldBillboard, *newBillboard;
    pfVec3 data;
    pfGroup *newMasterGroup, *parentGroup;
    pfGeoSet *tempGeoset;
    
    if (targetGraph->isOfType(pfGroup::getClassType()))
    {
        childCount = ((pfGroup *)targetGraph)->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixGeodes(((pfGroup *)targetGraph)->getChild(loop));
    }
    else if (targetGraph->isOfType(pfGeode::getClassType()))
    {
        oldGeode = (pfGeode *)targetGraph;

        if (oldGeode->getNumGSets() > 1)
        {
            // 'Fix' the geode by creating one geode for every geoset
            newMasterGroup = new pfGroup();
	    while (oldGeode->getNumParents() > 0)
            {
                parentGroup = oldGeode->getParent(0);
                parentGroup->replaceChild(oldGeode, newMasterGroup);
            }
            
            while (oldGeode->getNumGSets() > 0)
            {
                tempGeoset = oldGeode->getGSet(0);
                if (oldGeode->isOfType(pfBillboard::getClassType()))
                {
                    // Copy the relevant old billboard data
                    oldBillboard = (pfBillboard *)oldGeode;
                    newBillboard = new pfBillboard();
                    oldBillboard->getPos(0, data);
                    newBillboard->setPos(0, data);
                    newBillboard->setMode(PFBB_ROT,
                        oldBillboard->getMode(PFBB_ROT));
                    oldBillboard->getAxis(data);
                    newBillboard->setAxis(data);
                    newGeode = newBillboard;
                }
                else
                    newGeode = new pfGeode();
                oldGeode->removeGSet(tempGeoset);
                newGeode->addGSet(tempGeoset);
                newMasterGroup->addChild(newGeode);
            }

            pfDelete(oldGeode);
        }
    }
}

// ------------------------------------------------------------------------
// Private function
// Replaces every billboard in the Performer scene graph with an ordinary
// geode. This is done after the billboard data has been collected by the
// VESS scene graph construction pass so that the Performer billboards no
// longer have any effect; their responsibility is taken up by the
// vsBillboardAttribute objects.
// ------------------------------------------------------------------------
void vsDatabaseLoader::replaceBillboards(pfNode *targetGraph)
{
    int childCount, loop;
    pfBillboard *oldBillboard;
    pfGeode *newGeode;
    pfGeoSet *geoset;
    vsGeometry *boundGeom;
    pfGroup *parentGroup;
    vsObjectMap *nodeMap;
    
    if (targetGraph->isOfType(pfGroup::getClassType()))
    {
        childCount = ((pfGroup *)targetGraph)->getNumChildren();
        for (loop = 0; loop < childCount; loop++)
            fixGeodes(((pfGroup *)targetGraph)->getChild(loop));
    }
    else if (targetGraph->isOfType(pfBillboard::getClassType()))
    {
        // Replace every encountered pfBillboard with a pfGeode.
        oldBillboard = (pfBillboard *)targetGraph;
        newGeode = new pfGeode();
        
        // Move all of the geosets from the billboard to the geode
        while (oldBillboard->getNumGSets() > 0)
        {
            geoset = oldBillboard->getGSet(0);
            oldBillboard->removeGSet(geoset);
            newGeode->addGSet(geoset);
        }
        
        // Replace the billboard with the geode for each parent of the
        // billboard
        while (oldBillboard->getNumParents() > 0)
        {
            parentGroup = oldBillboard->getParent(0);
            parentGroup->replaceChild(oldBillboard, newGeode);
        }
        
        // Replace the billboard with the geode in the global object map
        nodeMap = (vsSystem::systemObject)->getNodeMap();
        boundGeom = (vsGeometry *)
            (nodeMap->mapSecondToFirst(oldBillboard));
        if (boundGeom)
        {
            nodeMap->removeLink(boundGeom, VS_OBJMAP_FIRST_LIST);
            nodeMap->registerLink(boundGeom, newGeode);
        }
        
        // Clean up
        pfDelete(oldBillboard);
    }
}

// ------------------------------------------------------------------------
// VESS internal function - Called by the vsSystem object's constructor
// Further initializes the loader by initializing the loader DSO's for
// each file type the program is to be able to handle. Once this function
// is called the loader cannot accept any more filename extensions.
// ------------------------------------------------------------------------
void vsDatabaseLoader::init()
{
    int loop;
    fltRegisterNodeT callbackPtr;
    void *callbackHandle;

    for (loop = 0; loop < extensionCount; loop++)
    {
        if (!pfdInitConverter((char *)(extensions[loop])))
        {
            printf("vsDatabaseLoader::init: Unable to initialize '%s' loader\n",
                extensions[loop]);
            continue;
        }
        if (classifyExtension((char *)(extensions[loop])) ==
            VS_DATABASE_TYPE_FLT)
        {
            callbackPtr = fltLoaderCallback;
            callbackHandle = &callbackPtr;
            pfdConverterAttr_flt(PFFLT_REGISTER_NODE, callbackHandle);
            pfdConverterMode_flt(PFFLT_FLATTEN, PF_OFF);
            pfdConverterMode_flt(PFFLT_CLEAN, PF_OFF);
        }
    }

    inittedFlag = 1;
}

// ------------------------------------------------------------------------
// static VESS internal function - Callback given to the OpenFlight loader
// When loading in an OpenFlight format file, this loader callback is used
// to prepare DOF beads to be 'fixed'; the standard translation from DOFs
// to Performer DCSs is incomplete. The function stores matrix data taken
// from the OpenFlight loader and stores it in the DCS's user data field,
// to be later retrieved by the fixPerformerFltDOF function.
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
    
    switch (mgOp)
    {
        case CB_DOF:
            loaderDOFBlock = (DOFcb *)cbs;
            currentDCS = (pfDCS *)node;
            myData = (vsdbMatrixBlock *)
                (pfMemory::malloc(sizeof(vsdbMatrixBlock)));
            if (!myData)
            {
                printf("vsDatabaseLoader::fltLoaderCallback: Memory "
                    "allocation failure\n");
                return;
            }

            strcpy(myData->magicString, "DOF");
            myData->aboveMatrix.copy(loaderDOFBlock->putinvmat);
            myData->belowMatrix.copy(loaderDOFBlock->putmat);

            currentDCS->setUserData(myData);

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
    
    targetName = targetNode->getName();

    for (loop = 0; loop < nodeNameCount; loop++)
        if (!strcmp((char *)(nodeNames[loop]), targetName))
            return VS_TRUE;

    if (importantXformMode && targetNode->isOfType(pfDCS::getClassType()))
	return VS_TRUE;

    return VS_FALSE;
}
